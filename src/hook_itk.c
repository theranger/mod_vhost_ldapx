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
#include <http_config.h>
#include <apr_strings.h>

#include "mod_vhost_ldapx.h"
#include "config.h"
#include "log.h"

#include "hook_itk.h"

#define ITK_MODULE_NAME "mpm_itk.c"

int vhx_hook_itk(request_rec *r) {
	vhx_settings_t *settings = (vhx_settings_t *) ap_get_module_config(r->server->module_config, &vhost_ldapx_module);
	if (!settings->enable) {
		VHX_INFO(r->server, "VHX Disabled %s", r->hostname);
		return OK;
	}

	vhx_request_t *v = vhx_config_get(settings, r);
	if(v == NULL) {
		VHX_ERROR(r->server, "Module configuration error");
		return  HTTP_INTERNAL_SERVER_ERROR;
	}

	if(v->dn == NULL) {
		VHX_INFO(r->server, "VHX Not Found %s", r->hostname);
		return OK;
	}

	// UID and GID cannot be provided without a user name. Otherwise we will crash
	if(v->user == NULL) return OK;

	module *mpm_itk_module = ap_find_linked_module(ITK_MODULE_NAME);
	if(mpm_itk_module == NULL) {
		VHX_ERROR(r->server, "Could not load ITK module");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	itk_per_dir_conf *cfg = (itk_per_dir_conf *) ap_get_module_config(r->per_dir_config, mpm_itk_module);
	if(cfg == NULL) {
		VHX_ERROR(r->server, "Could not get ITK module config");
		return HTTP_INTERNAL_SERVER_ERROR;
	}

	cfg->username = apr_pstrdup(r->pool, v->user);
	if(v->uid > 0) cfg->uid = v->uid;
	if(v->gid > 0) cfg->gid = v->gid;

	VHX_INFO(r->server, "Configured ITK to user=%s, uid=%u, gid=%u", cfg->username, cfg->uid, cfg->gid);
	return OK;
}
