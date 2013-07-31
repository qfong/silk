// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_EVENT_POSTED_H_INCLUDE_
#define _SK_EVENT_POSTED_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

extern sk_event_t 	*sk_posted_accept_events;
extern sk_event_t		*sk_posted_events;

#define sk_locked_post_event(ev,queue)		\
	if(ev->prev == NULL){					\
		ev->next = (sk_event_t *)*queue;	\
		ev->prev = (sk_event_t **)queue;	\
		*queue = ev;							\
		if(ev->next){							\
			ev->next->prev = &ev->next;		\
		}										\
	}else{										\
	}

#define sk_delete_posted_event(ev)			\
		*(ev->prev) = ev->next;				\
		if(ev->next){							\
			ev->next->prev = ev->prev;		\
		}										\
		ev->prev = NULL;

#define sk_post_event(ev,queue)		sk_locked_post_event(ev,queue);

void sk_event_process_posted(sk_cycle_t *cycle,sk_event_t **posted);

#endif

