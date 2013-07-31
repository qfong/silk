// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_EVENT_H_INCLUDE_
#define _SK_EVENT_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
typedef void(*sk_event_handler_pt)(sk_event_t *ev);
sk_ptr_t sk_handle_read_event(sk_event_t *rev,sk_uptr_t flags);
sk_ptr_t sk_event_module_init(sk_cycle_t *cycle);
struct sk_event_s{
	void				*data;
	unsigned			write:1;
	unsigned			accept:1;

	unsigned			instance:1;

	unsigned			complete:1;

	unsigned			timeout:1;
	unsigned			timer_set:1;

	sk_rbtree_node_t	timer;

	unsigned			closed:1;
	unsigned			channel:1;

	unsigned			available:1;
	unsigned			disabled:1;
	unsigned			active:1;
	unsigned			ready:1;

	sk_event_handler_pt handler;
	sk_uptr_t			index;
	sk_log_t			*log;
	sk_event_t			*next;
	sk_event_t			**prev;


};

extern sig_atomic_t		sk_event_timer_alarm;
extern sk_atomic_t		*sk_accept_mutex_ptr;

extern sk_ptr_t			sk_accept_disabled;
extern sk_uptr_t			sk_accept_mutex_held;
extern sk_shmtx_t			sk_mempool_mutex;
extern sk_shmtx_t			sk_accept_mutex;
extern sk_uptr_t			sk_accept_events;
extern sk_msec_t			sk_accept_mutex_delay;

extern sk_uptr_t			sk_event_flags;
extern volatile sk_msec_t			sk_current_msec;





#endif
