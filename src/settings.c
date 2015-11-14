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

#include <ctype.h>
#include <apr_strings.h>

#include "mod_vhost_ldapx.h"
#include "settings.h"

typedef enum {
	VHX_ENABLE,
	VHX_LDAP_URL,
	VHX_BIND_DN,
	VHX_BIND_PW,
	VHX_DEFAULT_USER,
	VHX_DEFAULT_TTL,
} vhx_config_v;

const command_rec vhx_settings[] = {
	AP_INIT_FLAG("VHXEnable", vhx_set_flag, (void *)VHX_ENABLE, RSRC_CONF, "Enable VHX module lookups"),
	AP_INIT_TAKE1("VHXLdapUrl", vhx_set_char, (void *)VHX_LDAP_URL, RSRC_CONF, "LDAP server URL"),
	AP_INIT_TAKE1("VHXBindDN", vhx_set_char, (void *)VHX_BIND_DN, RSRC_CONF, "Bind DN"),
	AP_INIT_TAKE1("VHXBindPW", vhx_set_char, (void *)VHX_BIND_PW, RSRC_CONF, "Bind password"),
	AP_INIT_TAKE1("VHXDefaultUser", vhx_set_char, (void *)VHX_BIND_PW, RSRC_CONF, "Bind password"),
	AP_INIT_TAKE1("VHXDefaultTTL", vhx_set_char, (void *)VHX_DEFAULT_TTL, RSRC_CONF, "Default TTL for cache"),
	{ NULL }
};

const char * vhx_set_char(cmd_parms *cmd, void *mconfig, const char *arg) {
	vhx_settings_t *settings = (vhx_settings_t *)ap_get_module_config(cmd->server->module_config, &vhost_ldapx_module);

	switch((vhx_config_v)cmd->info) {
		case VHX_LDAP_URL:
			settings->ldap_url = apr_pstrdup(cmd->pool, arg);
			return NULL;

		case VHX_BIND_DN:
			settings->ldap_binddn = apr_pstrdup(cmd->pool, arg);
			return NULL;

		case VHX_BIND_PW:
			settings->ldap_bindpw = apr_pstrdup(cmd->pool, arg);
			return NULL;

		case VHX_DEFAULT_USER:
			settings->default_user = apr_pstrdup(cmd->pool, arg);
			return NULL;

		case VHX_DEFAULT_TTL:
			settings->default_ttl = atoi(arg);
			return NULL;

		default:
			return NULL;
	}
}

const char * vhx_set_flag(cmd_parms *cmd, void *mconfig, int on) {
	vhx_settings_t *settings = (vhx_settings_t *)ap_get_module_config(cmd->server->module_config, &vhost_ldapx_module);

	switch((vhx_config_v)cmd->info) {
		case VHX_ENABLE:
			settings->enable = (uint8_t)on;
			return NULL;

		default:
			return NULL;
	}
}

static char *trim(char *str) {
	char *end;
	while(isspace(*str)) str++;

	// All whitespace?
	if(*str == 0) return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	*(end + 1) = 0;
	return str;
}

int vhx_print_host_info(const struct printf_info *info, size_t n, int *argtypes, int *size) {
	if(n > 0) {
		argtypes[0] = PA_POINTER;
		size[0] = sizeof(request_rec *);
	}
	return 1;
}

int vhx_print_host(FILE *stream, const struct printf_info *info, const void *const *args) {
	const request_rec *r = *(request_rec **)args[0];
	return fprintf(stream, "%s", r->hostname);
}
