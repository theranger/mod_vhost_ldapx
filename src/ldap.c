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

#include <lber.h>
#include <ldap.h>
#include "ldap.h"
#include "log.h"

static 	LDAP *ld = NULL;
static int msgid = 0;
static LDAPMessage* msg = NULL;
static char *dn = NULL;
static struct berval **values = NULL;
static server_rec *s = NULL;

int vhx_ldap_connect(const char *url) {
	return ldap_initialize(&ld, url);
}

int vhx_ldap_bind(const char *bindDN, char *bindPW) {
	if(ld == NULL) return LDAP_CONNECT_ERROR;

	struct berval cred;
	cred.bv_val = bindPW;
	cred.bv_len = strlen(bindPW);

	return ldap_sasl_bind_s(ld, bindDN, LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
}

int vhx_ldap_search(const char *baseDN, int scope, const char *filter, char **attributes) {
	return ldap_search_ext_s(ld, baseDN, scope, filter, attributes, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &msg);
}

static void vhx_ldap_log(LDAP_CONST char *data) {
	if(data == NULL || s == NULL) return;
	VHX_DEBUG(s, "LDAP: %s", data);
}

int vhx_ldap_set_logging(server_rec *srv) {
	s = srv;

	if(ber_set_option(NULL, LBER_OPT_LOG_PRINT_FN, (void *) vhx_ldap_log) != LBER_OPT_SUCCESS) return LBER_OPT_ERROR;

	int arg = srv == NULL ? 0 : 5;
	return ldap_set_option(NULL, LDAP_OPT_DEBUG_LEVEL, &arg);
}

int vhx_ldap_set_version(int version) {
	if(ld == NULL) return LDAP_OPT_ERROR;
	if(version != LDAP_VERSION1 && version != LDAP_VERSION2 && version != LDAP_VERSION3) return LDAP_OPT_ERROR;

	return ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &version);
}

int vhx_ldap_count_entries() {
	if(ld == NULL || msg == NULL) return 0;
	return ldap_count_entries(ld, msg);
}

const char * vhx_ldap_get_entry() {
	if(values != NULL) {
		ldap_value_free_len(values);
		values = NULL;
	}

	if(dn == NULL) {
		msg = ldap_first_entry(ld, msg);
	}
	else {
		ldap_memfree(dn);
		dn = NULL;
		msg = ldap_next_entry(ld, msg);
	}

	if(msg == NULL) return NULL;

	dn = ldap_get_dn(ld, msg);
	return dn;
}

void vhx_ldap_disconnect() {
	if(values != NULL) {
		ldap_value_free_len(values);
		values = NULL;
	}

	if(dn != NULL) {
		ldap_memfree(dn);
		dn = NULL;
	}

	if(msg != NULL) {
		ldap_msgfree(msg);
		msg = NULL;
	}

	if(ld != NULL) {
		ldap_unbind_ext_s(ld, NULL, NULL);
		ld = NULL;
	}
}

struct berval ** vhx_ldap_get_values(const char *attribute) {
	if(values != NULL) {
		ldap_value_free_len(values);
		values = NULL;
	}

	if(msg == NULL) return NULL;

	values = ldap_get_values_len(ld, msg, attribute);
	return values;
}
