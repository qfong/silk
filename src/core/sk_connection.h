// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;


#ifndef _SK_CONNECTION_H_INCLUDE_
#define _SK_CONNECTION_H_INCLUDE_


#include "sk_config.h"
#include "sk_core.h"

typedef ssize_t (*sk_recv_pt)(sk_connection_t *c ,u_char *buf,size_t size);

typedef ssize_t (*sk_recv_chain_pt)(sk_connection_t *c ,sk_chain_t *in);
typedef ssize_t (*sk_send_pt)(sk_connection_t *c ,u_char *buf,size_t size);
typedef sk_chain_t (*sk_send_chain_pt)(sk_connection_t *c ,sk_chain_t *in);

typedef void(*sk_connection_handler_pt)(sk_connection_t *c);

struct sk_listening_s{
	sk_t				fd;
	struct sockaddr_in	sockaddr;
	socklen_t			socklen;
	sk_t				type;
	sk_t				rcvbuf;
	sk_t				sndbuf;
	sk_t				backlog;


	sk_connection_handler_pt	 handler;


	void				*servers;
	sk_log_t			*log;
	size_t				pool_size;
	sk_msec_t			post_accept_timeout;
	unsigned			listen:1;

	sk_listening_t	*prev;
	sk_connection_t	*connection;
};

struct sk_queue_s{
	sk_queue_t			*prev;
	sk_queue_t			*next;
};

struct sk_connection_s{
	void				*data;
	sk_event_t			*read;
	sk_event_t			*write;

	sk_t				fd;
	sk_recv_pt			recv;
	sk_send_pt			send;
	sk_recv_chain_pt	recv_chain;
	sk_send_chain_pt	send_chain;

	sk_listening_t	*listening;

	//off_t				send;
	sk_log_t			*log;

	sk_pool_t			*pool;
	struct sockaddr	*sockaddr;

	socklen_t			socklen;

	sk_str_t			addr_text;
	//sk_buf_t			*buffer;
	sk_queue_t			queue;

	//sk_atomic_uint_t	number;

	sk_uptr_t			request;

	unsigned			buffered:8;
	unsigned			log_err:3;
	unsigned			timeout:1;
	unsigned			error:1;

	unsigned			idle:1;
	unsigned			reuseable:1;
	unsigned			close:1;
	unsigned			sendfile:1;
};

sk_ptr_t sk_init_listening(sk_cycle_t *cycle);
sk_connection_t	* sk_get_connection(sk_t s,sk_log_t *log);
void sk_close_connection(sk_connection_t *c);
#endif
