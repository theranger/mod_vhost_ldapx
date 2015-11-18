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

#include <apr_strings.h>
#include "mod_vhost_ldapx.h"
#include "config.h"
#include "log.h"

#include "hook_vhost.h"


int vhx_hook_vhost(request_rec *r) {
	if(r->hostname == NULL) return DECLINED;

	vhx_settings_t *settings = (vhx_settings_t *) ap_get_module_config(r->server->module_config, &vhost_ldapx_module);
	if (!settings->enable) {
		VHX_INFO(r->server, "VirtualHost %s VHX disabled", r->hostname);
		return DECLINED;
	}

	vhx_request_t *v = vhx_config_get(settings, r);
	if(v == NULL) {
		VHX_ERROR(r->server, "Module configuration error");
		return  HTTP_INTERNAL_SERVER_ERROR;
	}

	if(v->dn == NULL) {
		VHX_INFO(r->server, "VirtualHost %s not found", r->hostname);
		return DECLINED;
	}

	if(v->document_root == NULL) {
		VHX_INFO(r->server, "VirtualHost %s not configured", r->hostname);
		return DECLINED;
	}

	VHX_INFO(r->server, "VirtualHost %s found, documentRoot=%s, serverAdmin=%s", r->hostname, v->document_root, v->server_admin?v->server_admin:"<>");
	ap_set_document_root(r, v->document_root);
	if(v->server_admin) r->server->server_admin = apr_pstrdup(r->pool, v->server_admin);

	return DECLINED;
}
