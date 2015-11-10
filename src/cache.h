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

#ifndef MOD_VHOST_LDAPX_CACHE_H
#define MOD_VHOST_LDAPX_CACHE_H

#include "config.h"

void cache_init(apr_pool_t *p);
vhx_request_t * cache_search(const char *key);
void cache_add(const char *key, vhx_request_t *v);
int cache_count();
apr_pool_t * cache_get_pool();
apr_time_t cached_entry_timer(const vhx_request_t *v);


#endif //MOD_VHOST_LDAPX_CACHE_H
