// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_EVENT_TIMER_H_INCLUDE_
#define _SK_EVENT_TIMER_H_INCLUDE_
#include <sk_config.h>
#include "sk_core.h"


#define SK_TIMER_INFINITE (sk_msec_t)-1
#define SK_TIMER_LAZY_DELAY	300
typedef sk_rbtree_key_int_t	sk_msec_int_t;
extern sk_rbtree_t			sk_event_timer_rbtree;

static inline void
sk_event_del_timer(sk_event_t *ev)
{
	sk_rbtree_delete(&sk_event_timer_rbtree,&ev->timer);
	ev->timer_set = 0;
}

sk_ptr_t sk_init_event_timer(sk_log_t *log);
void sk_event_expire_timer(void);
#endif
