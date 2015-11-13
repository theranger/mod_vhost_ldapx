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
#include "ldap.h"

static 	LDAP *ld = NULL;
static int msgid = 0;
static LDAPMessage* msg = NULL;
static char *dn = NULL;
static struct berval **values = NULL;

int ldap_connect(const char *url) {
	return ldap_initialize(&ld, url);
}

int ldap_bind(const char *bindDN, char *bindPW) {
	struct berval cred;
	cred.bv_val = bindPW;
	cred.bv_len = strlen(bindPW);

	return ldap_sasl_bind_s(ld, bindDN, LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
}

int ldap_search(const char *baseDN, int scope, const char *filter, char **attributes) {
	return ldap_search_ext_s(ld, baseDN, scope, filter, attributes, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &msg);
}

const char * ldap_get_entry() {
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

void ldap_disconnect() {
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

struct berval ** ldap_get_values(const char *attribute) {
	if(values != NULL) {
		ldap_value_free_len(values);
		values = NULL;
	}

	if(msg == NULL) return NULL;

	values = ldap_get_values_len(ld, msg, attribute);
	return values;
}
