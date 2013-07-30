// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

sk_rbtree_t						sk_event_timer_rbtree;

static sk_rbtree_node_t			sk_event_timer_sentinel;
sk_atomic_t						*sk_event_timer_mutex;



sk_ptr_t
sk_init_event_timer(sk_log_t *log)
{
	if(&sk_event_timer_rbtree == NULL) return SK_ERR;
	sk_init_rbtree(&sk_event_timer_rbtree,&sk_event_timer_sentinel,
			sk_rbtree_insert_timer_value);
	return SK_OK;
}


sk_msec_t
sk_event_find_timer(void)
{
	sk_msec_int_t		timer;
	sk_rbtree_node_t	*node,*root,*sentinel;

	if(sk_event_timer_rbtree.root == &sk_event_timer_sentinel){
		return SK_TIMER_INFINITE;
	}

	root = sk_event_timer_rbtree.root;
	sentinel = sk_event_timer_rbtree.sentinel;

	node = sk_rbtree_min(root,sentinel);

	timer = (sk_msec_int_t)node->key - (sk_msec_int_t)sk_current_msec;

	return (sk_msec_t)(timer > 0 ?timer :0);
}


void
sk_event_expire_timer(void)
{
	sk_event_t			*ev;
	sk_rbtree_node_t	*node,*root,*sentinel;

	sentinel = sk_event_timer_rbtree.sentinel;
	for(;;){
		root = sk_event_timer_rbtree.root;
		if(root == sentinel){
			return ;
		}
		node = sk_rbtree_min(root,sentinel);

		if((sk_msec_int_t)node->key -(sk_msec_int_t)sk_current_msec <= 0){
			ev = (sk_event_t *)((char *)node - offsetof(sk_event_t,timer));

			sk_rbtree_delete(&sk_event_timer_rbtree,&ev->timer);
			ev->timer_set = 0;
			ev->timeout = 1;
			ev->handler(ev);
			continue;
		}
		break;
	}
}


void
sk_event_add_timer(sk_event_t *ev,sk_msec_t timer)
{
	sk_msec_t				key;
	sk_msec_int_t			diff;

	key = sk_current_msec + timer;

	if(ev->timer_set){
		diff = (sk_msec_int_t)(key - ev->timer.key);
		if(abs(diff) < SK_TIMER_LAZY_DELAY){
			return;
		}
		sk_event_del_timer(ev);
	}
	ev->timer.key = key;

	sk_rbtree_insert(&sk_event_timer_rbtree,&ev->timer);

	ev->timer_set = 1;
}
