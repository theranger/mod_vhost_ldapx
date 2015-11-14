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

#ifndef MOD_VHOST_LDAPX_CONFIG_H
#define MOD_VHOST_LDAPX_CONFIG_H

#include "settings.h"

typedef struct {
	char *		dn;					// NULL if unsuccessful
	char *		server_name;
	char *		server_admin;
	char *		document_root;
	char *		home_directory;
	char *		user;				// User logon name
	uid_t		uid;
	gid_t		gid;
	int 		ttl;				// Lifetime retrieved from LDAP settings
	apr_time_t	timestamp;			// When entry was stored in cache
} vhx_request_t;

vhx_request_t * vhx_config_get(vhx_settings_t *settings, request_rec *r);
int vhx_config_ldap_search(vhx_settings_t *settings, request_rec *r, vhx_request_t *v);

#endif //MOD_VHOST_LDAPX_CONFIG_H
