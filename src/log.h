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

#ifndef MOD_VHOST_LDAPX_LOG_H
#define MOD_VHOST_LDAPX_LOG_H

#include <http_log.h>

// Following log levels are sorted by priority, highest first.
#define VHX_ERROR(server_rec, fmt, args...)	ap_log_error(APLOG_MARK, APLOG_ERR, 0, server_rec, fmt, ## args)
#define VHX_NOTICE(server_rec, fmt, args...)	ap_log_error(APLOG_MARK, APLOG_NOTICE, 0, server_rec, fmt, ## args)
#define VHX_INFO(server_rec, fmt, args...)	ap_log_error(APLOG_MARK, APLOG_INFO, 0, server_rec, fmt, ## args)
#define VHX_DEBUG(server_rec, fmt, args...)	ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, server_rec, fmt, ## args)

#endif //MOD_VHOST_LDAPX_LOG_H
