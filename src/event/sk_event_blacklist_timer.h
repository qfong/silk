// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;



#ifndef _NEOSILK_EVENT_blacklist_TIMER_H_INCLUDE_
#define _NEOSILK_EVENT_blacklist_TIMER_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
typedef struct sk_event_blacklist_s		sk_event_blacklist_t;
extern sk_rbtree_t							sk_event_blacklist_timer_rbtree;
extern sk_rbtree_t							sk_event_blacklist_rbtree;
typedef struct list_s 						list_t;
extern list_t  								sroot;
typedef struct listnode 						Listnode;


struct listnode{
	Listnode					*next;
	sk_event_blacklist_t 	*point;
};

struct list_s{
	Listnode				*head;
	Listnode				*tail;
	int 					count;
};
void list_insert(list_t *sroot, struct listnode* listnode);
sk_ptr_t sk_event_timer_init(sk_log_t *log);
void sk_event_blacklist_expire_timer(void);

#define SK_TIMER_LAZY_DELAY 300

struct sk_event_blacklist_s{
	void				*data;

	unsigned			timedout:1;
	unsigned			timer_set:1;

	sk_uptr_t			index;
	sk_log_t			*log;
	sk_rbtree_node_t	timer;
	sk_rbtree_node_t	blacklist;


	sk_event_t			*next;
	sk_event_t			**prev;
	//u8_t*				blacklist_value;
};

sk_event_blacklist_t *
sk_event_blacklist_find(sk_rbtree_key_t key);

static inline void
sk_event_del_blacklist_timer(sk_event_blacklist_t *ev)
{

	sk_rbtree_delete(&sk_event_blacklist_timer_rbtree,&ev->timer);
	ev->timer_set = 0;
}
static inline void
sk_event_del_blacklist(sk_event_blacklist_t *ev)
{

	sk_rbtree_delete(&sk_event_blacklist_rbtree,&ev->timer);
	ev->timer_set = 0;
}

void sk_event_add_blacklist_timer(sk_event_blacklist_t *ev,sk_msec_t timer);
void sk_event_add_blacklist(sk_event_blacklist_t *sev,sk_rbtree_key_t key);
#endif /* _NEOSILK_EVENT_blacklist_TIMER_H_INCLUDE_ */
