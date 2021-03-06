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
#include <ap_expr.h>

#ifndef MOD_VHOST_LDAPX_HOOK_ITK_H
#define MOD_VHOST_LDAPX_HOOK_ITK_H

// From ITK source
typedef struct {
	uid_t				uid;
	gid_t				gid;
	char *				username;
	int					nice_value;
	ap_expr_info_t *	uid_expr;
	ap_expr_info_t *	gid_expr;
} itk_per_dir_conf;

int vhx_hook_itk(request_rec *r);

#endif //MOD_VHOST_LDAPX_HOOK_ITK_H
