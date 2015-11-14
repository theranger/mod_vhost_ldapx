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

#include <httpd.h>
#include <http_request.h>
#include <http_protocol.h>

#include "settings.h"
#include "hook_vhost.h"
#include "hook_init.h"
#include "hook_itk.h"

#include "mod_vhost_ldapx.h"

#define VHX_DEFAULT_FILTER		"(apacheServerName=%v)"
#define VHX_DEFAULT_TTL_SEC		300

static void *vhx_create_settings(apr_pool_t *p, server_rec *s);
static void *vhx_merge_settings(apr_pool_t *p, void *parent_settings, void *child_settings);
static void vhx_register_hooks(apr_pool_t *p);

AP_MODULE_DECLARE_DATA module vhost_ldapx_module = {
		STANDARD20_MODULE_STUFF,
		NULL,
		NULL,
		vhx_create_settings,
		vhx_merge_settings,
		vhx_settings,
		vhx_register_hooks
};

static void * vhx_create_settings(apr_pool_t *p, server_rec *s) {
	vhx_settings_t *vhr = (vhx_settings_t *) apr_pcalloc(p, sizeof(vhx_settings_t));
	vhr->default_filter = VHX_DEFAULT_FILTER;
	vhr->default_ttl = VHX_DEFAULT_TTL_SEC;
	return vhr;
}

static void * vhx_merge_settings(apr_pool_t *p, void *parent_settings, void *child_settings) {
	vhx_settings_t *parent = (vhx_settings_t *) parent_settings;
	vhx_settings_t *child = (vhx_settings_t *) child_settings;
	vhx_settings_t *conf = (vhx_settings_t *) apr_pcalloc(p, sizeof(vhx_settings_t));

	conf->enable = child->enable ? child->enable : parent->enable;
	conf->ldap_url = child->ldap_url ? child->ldap_url : parent->ldap_url;
	conf->default_host = child->default_host ? child->default_host : parent->default_host;
	conf->ldap_binddn = child->ldap_binddn ? child->ldap_binddn : parent->ldap_binddn;
	conf->ldap_bindpw = child->ldap_bindpw ? child->ldap_bindpw : parent->ldap_bindpw;
	conf->default_filter = child->default_filter ? child->default_filter : parent->default_filter;
	conf->default_ttl = child->default_ttl ? child->default_ttl : parent->default_ttl;

	return conf;
}

static void vhx_register_hooks(apr_pool_t *p) {
	ap_hook_translate_name(vhx_hook_vhost, NULL, NULL, APR_HOOK_FIRST);
	ap_hook_child_init(vhx_hook_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_read_request(vhx_hook_itk, NULL, NULL, APR_HOOK_REALLY_FIRST);

	// %v tries to locate hostname from request_rec_t
	register_printf_specifier('v', vhx_print_host, vhx_print_host_info);
}
