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
#include <apr_hash.h>

#include "cache.h"
#include "config.h"

static apr_hash_t *cache = NULL;

apr_status_t cache_destroy(void *data){
	cache = NULL;
	return APR_SUCCESS;
}

void cache_init(apr_pool_t *p) {
	if(cache != NULL) return;

	apr_pool_t *pool;
	apr_pool_create(&pool, p);
	if(pool == NULL) return;

	cache = apr_hash_make(pool);
}

apr_pool_t * cache_get_pool() {
	return apr_hash_pool_get(cache);
}

vhx_request_t * cache_search(const char *key) {
	if(cache == NULL) return NULL;

	vhx_request_t *v = apr_hash_get(cache, key, APR_HASH_KEY_STRING);
	if(v == NULL) return NULL;

	return v;
}

apr_time_t cached_entry_timer(const vhx_request_t *v) {
	return apr_time_sec(v->timestamp + apr_time_from_sec(v->ttl) - apr_time_now());
}

int cache_count() {
	return apr_hash_count(cache);
}

void cache_add(const char *key, vhx_request_t *v) {
	if(cache == NULL) return;
	if(v == NULL || key == NULL) return;

	v->timestamp = apr_time_now();
	apr_hash_set(cache, key, APR_HASH_KEY_STRING, v);
}
