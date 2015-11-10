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

#ifndef MOD_VHOST_LDAPX_SETTINGS_H
#define MOD_VHOST_LDAPX_SETTINGS_H

#include <printf.h>
#include <httpd.h>
#include <http_config.h>

static char *trim(char *str);
const char * set_char(cmd_parms *parms, void *mconfig, const char *w);
const char * set_flag(cmd_parms *parms, void *mconfig, int on);


int print_host_info(const struct printf_info *info, size_t n, int *argtypes, int *size);
int print_host(FILE *stream, const struct printf_info *info,const void *const *args);

extern const command_rec settings[];

typedef struct {
	uint8_t		enable;				// enable or disable the module
	char *		default_host;		// default host to redirect to
	char *		ldap_url;			// string representation of LDAP URL
	char *		ldap_basedn;
	char *		ldap_binddn;		// set null to bind anonymously
	char *		ldap_bindpw;		// set null if no password needed
	char *		default_user;		// default user to run ITK child
	char *		default_filter;		// default ldap search filter
	int			default_ttl;		// default ttl for cache entries
} vhx_settings_t;

#endif //MOD_VHOST_LDAPX_SETTINGS_H
