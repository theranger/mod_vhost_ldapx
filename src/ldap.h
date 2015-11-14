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

#ifndef MOD_VHOST_LDAPX_LDAP_H
#define MOD_VHOST_LDAPX_LDAP_H

#include <httpd.h>

int vhx_ldap_connect(const char *url);
int vhx_ldap_bind(const char *bindDN, char *bindPW);
int vhx_ldap_search(const char *baseDN, int scope, const char *filter, char **attributes);
const char * vhx_ldap_get_entry();
struct berval ** vhx_ldap_get_values(const char *attribute);
void vhx_ldap_disconnect();
int vhx_ldap_set_version(int version);
int vhx_ldap_set_logging(server_rec *srv);

#endif //MOD_VHOST_LDAPX_LDAP_H
