/*
   Copyright 2015 The Ranger <ranger@risk.ee>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <ldap.h>
#include <httpd.h>
#include <apr_strings.h>
#include <pwd.h>

#include "ldap.h"
#include "cache.h"
#include "log.h"

#include "config.h"
#include "settings.h"

#define MAX_BUF_LEN					1024

#define LDAP_ATTR_SERVER_NAME		"apacheServerName"
#define LDAP_ATTR_DOCUMENT_ROOT		"apacheDocumentRoot"
#define LDAP_ATTR_SERVER_ADMIN		"apacheServerAdmin"
#define LDAP_ATTR_UID				"apacheUidNumber"
#define LDAP_ATTR_GID				"apacheGidNumber"
#define LDAP_ATTR_USER				"apacheUser"
#define LDAP_ATTR_TTL				"apacheTTL"

static char *attributes[] = {
		LDAP_ATTR_SERVER_NAME,
		LDAP_ATTR_DOCUMENT_ROOT,
		LDAP_ATTR_SERVER_ADMIN,
		LDAP_ATTR_UID,
		LDAP_ATTR_GID,
		LDAP_ATTR_USER,
		LDAP_ATTR_TTL,
		NULL
};

int fill_user_info(vhx_request_t *v, uid_t uid, gid_t gid) {
	apr_pool_t *pool = cache_get_pool();
	if(pool == NULL) return -1;

	if(v->user != NULL) {
		struct passwd *pwd = getpwnam(v->user);
		if(pwd != NULL) {
			v->uid = pwd->pw_uid;
			v->gid = gid > 0 ? gid : pwd->pw_gid;
			return 0;
		}

		v->user =  NULL;
	}

	if(uid > 0) {
		struct passwd *pwd = getpwuid(uid);
		if(pwd != NULL) {
			v->user = apr_pstrdup(pool, pwd->pw_name);
			v->uid = pwd->pw_uid;
			v->gid = gid > 0 ? gid : pwd->pw_gid;
			return 0;
		}
	}

	if(gid > 0) v->gid = gid;
	return 0;
}

vhx_request_t * config_get(vhx_settings_t *settings, request_rec *r) {

	vhx_request_t *v;
	if((v = cache_search(r->hostname)) != NULL) {
		apr_time_t timer = cached_entry_timer(v);
		if(timer > 0) {
			VHX_INFO(r->server, "CACHED entry for %s found, TTL=%ld/%d", r->hostname, timer, v->ttl);
			return v;
		}

		if(config_ldap_search(settings, r, v) < 0) {
			VHX_INFO(r->server, "STALE cached entry for %s found, TTL=%ld/%d", r->hostname, timer, v->ttl);
			return v;
		}

		VHX_INFO(r->server, "REFRESHED entry for %s, TTL=%ld/%d", r->hostname, timer, v->ttl);
		cache_add(r->hostname, v);
		return v;
	}

	apr_pool_t *pool = cache_get_pool();
	if(pool == NULL) return NULL;

	v = apr_pcalloc(pool, sizeof(vhx_request_t));
	if(v == NULL) return NULL;

	if((config_ldap_search(settings, r, v)) < 0) return NULL;

	VHX_INFO(r->server, "LDAP entry for %s found", r->hostname);
	cache_add(r->hostname, v);
	return v;
}

int config_ldap_search(vhx_settings_t *settings, request_rec *r, vhx_request_t *v) {
	if(v == NULL) {
		VHX_DEBUG(r->server, "Request container uninitialized");
		return -1;
	}

	apr_pool_t *pool = cache_get_pool();
	if(pool == NULL) {
		VHX_DEBUG(r->server, "Cache pool not set");
		return -1;
	}

	if(settings->ldap_url == NULL) {
		VHX_ERROR(r->server, "Could not find LDAP host information");
		return -1;
	}

	if(settings->ldap_binddn == NULL || settings->ldap_bindpw == NULL) {
		VHX_ERROR(r->server, "Could not find binding credentials");
		return -1;
	}

	char buf[MAX_BUF_LEN] = {0};
	snprintf(buf, sizeof(buf), settings->ldap_url, r);

	LDAPURLDesc *url_desc;
	if(ldap_url_parse(buf, &url_desc) != 0) {
		VHX_ERROR(r->server, "Failed parsing LDAP URL %s", buf);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	if(url_desc->lud_port)
		apr_snprintf(buf, sizeof(buf), "%s://%s:%d", url_desc->lud_scheme, url_desc->lud_host, url_desc->lud_port);
	else
		apr_snprintf(buf, sizeof(buf), "%s://%s", url_desc->lud_scheme, url_desc->lud_host);

	if(ldap_connect(buf) != 0) {
		VHX_ERROR(r->server, "Could not connect to LDAP host %s", buf);
		goto err_free_urldesc;
	}

	if(ldap_bind(settings->ldap_binddn, settings->ldap_bindpw) != LDAP_SUCCESS) {
		VHX_ERROR(r->server, "Could not bind to LDAP host");
		goto err_free_urldesc;
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), url_desc->lud_filter ? url_desc->lud_filter : settings->default_filter, r);

	VHX_DEBUG(r->server, "Searching dn=%s scope=%d, filter=%s", url_desc->lud_dn, url_desc->lud_scope, buf);
	if(ldap_search(url_desc->lud_dn, url_desc->lud_scope, buf, attributes) != LDAP_SUCCESS) {
		VHX_ERROR(r->server, "Could not make search query");
		goto err_free_urldesc;
	}

	const char *dn;
	struct berval **values;

	while((dn = ldap_get_entry()) != NULL) {
		v->dn = apr_pstrdup(pool, dn);

		values = ldap_get_values(LDAP_ATTR_SERVER_NAME);
		if(values) v->server_name = apr_pstrdup(pool, values[0]->bv_val);

		values = ldap_get_values(LDAP_ATTR_SERVER_ADMIN);
		if(values) v->server_admin = apr_pstrdup(pool, values[0]->bv_val);

		values = ldap_get_values(LDAP_ATTR_DOCUMENT_ROOT);
		if(values) v->document_root = apr_pstrdup(pool, values[0]->bv_val);

		values = ldap_get_values(LDAP_ATTR_USER);
		if(values) v->user = apr_pstrdup(pool, values[0]->bv_val);

		values = ldap_get_values(LDAP_ATTR_UID);
		uid_t uid = (uid_t) (values ? atoi(values[0]->bv_val) : 0);

		values = ldap_get_values(LDAP_ATTR_GID);
		gid_t gid = (gid_t) (values ? atoi(values[0]->bv_val) : 0);

		values = ldap_get_values(LDAP_ATTR_TTL);
		if(values) {
			int ttl = atoi(values[0]->bv_val);
			v->ttl = ttl ? ttl : settings->default_ttl;
		}
		else {
			v->ttl = settings->default_ttl;
		}

		if(fill_user_info(v, uid, gid) != 0 && settings->default_user != NULL) {
			v->user = apr_pstrdup(pool, values[0]->bv_val);
			fill_user_info(v, uid, gid);
		}

		break;
	}

	ldap_disconnect();
	ldap_free_urldesc(url_desc);
	return 1;

err_free_urldesc:
	ldap_free_urldesc(url_desc);
	return -1;
}