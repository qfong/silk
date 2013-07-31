// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_IDS_REQUEST_H_INCLUDE_
#define _SK_IDS_REQUEST_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
void sk_init_connection(sk_connection_t *c);

typedef struct sk_request_s 		sk_request_t;
typedef struct sk_http_header_s 	sk_http_header_t;
typedef struct sk_http_line_s		sk_http_line_t;
typedef struct sk_http_s				sk_http_t;
typedef struct sk_header_detect_s 	sk_header_detect_t;
typedef struct sk_list_s				sk_list_t;
typedef void(*sk_detect_handler_pt)(sk_list_t *list);
struct sk_http_line_s{
	u8_t					*method;
	u8_t					*url;
	u8_t					*version;
};

struct sk_http_header_s{

	u8_t					*accept;
	u8_t					*referer;
	u8_t					*accept_language;
	u8_t					*user_agent;
	u8_t					*content_type;
	u8_t					*host;
	sk_t					content_len;
	u8_t					*conn_status;//connection status
	u8_t					*cache_control;
	u8_t					*cookie;

};
struct sk_http_s{
	u8_t					*source_ip;
	sk_http_line_t		*http_line;
	sk_http_header_t		*http_header;
	sk_buf_t				*http_body;
};

struct sk_request_s{
	sk_connection_t		*connection;
	sk_buf_t				*header;
	sk_buf_t				*data;
	size_t					len;
	sk_pool_t				*pool;
	u8_t					*ip;
	u32_t				netip;
	sk_buf_t					*response;

	sk_http_t				*http;

	unsigned				ready:1;


};

struct sk_list_s{
	sk_list_t				*next;
	u8_t				 	*current;
	size_t					len;
	u8_t					*current_status;
	sk_uptr_t				sid;
	sk_t					found;
};

struct sk_header_detect_s{
	sk_list_t 				*content;
	sk_detect_handler_pt handler;

};
void sk_blacklist_event_add(sk_uptr_t netip);
#endif
