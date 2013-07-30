// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#ifndef _SK_EVENT_EPOLL_H_INCLUDE_
#define _SK_EVENT_EPOLL_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
#include <sys/epoll.h>
#define SK_USE_EPOLL_EVENT			0x00000040
#define SK_USE_RESIG_EVENT			0x00000080
#define SK_USE_TIMER_EVENT			0x00000800
#define SK_USE_GREEDY_EVENT			0x00000020
#define SK_USE_CLEAR_EVENT			0x00000004

#define SK_CLOSE_EVENT				1
#define SK_POST_EVENT				2

#define SK_READ_EVENT				EPOLLIN
#define SK_WRITE_EVENT				EPOLLOUT
#define SK_CLEAR_EVENT				EPOLLET
#define SK_UPDATE_TIME				1
#define SK_POST_EVENTS				2


sk_ptr_t 	sk_init_epoll(sk_cycle_t *cycle);
sk_ptr_t 	sk_epoll_add_event(sk_event_t *ev,sk_ptr_t event ,sk_uptr_t flags);
sk_ptr_t	sk_epoll_del_event(sk_event_t *ev,sk_ptr_t event ,sk_uptr_t flags);
sk_ptr_t 	sk_process_events(sk_cycle_t *cycle,sk_msec_t timer,sk_uptr_t flags);

#endif
