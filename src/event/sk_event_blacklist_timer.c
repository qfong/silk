// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;



#include "sk_config.h"
#include "sk_core.h"
#include <stddef.h>


sk_rbtree_t					sk_event_blacklist_timer_rbtree;

static sk_rbtree_node_t		sk_event_blacklist_timer_sentinel;

sk_rbtree_t					sk_event_blacklist_rbtree;
static sk_rbtree_node_t		sk_event_blacklist_sentinel;


list_t		sroot;

sk_atomic_t 					 *sk_event_timer_mutex;

/*
 * sroot blacklist root;
 */

void list_init(list_t * sroot)
{
	sroot->count = 0;
	sroot->head = NULL;
	sroot->tail = NULL;

}

void list_insert(list_t *sroot, Listnode *listnode)
{
	sroot->count ++;
	//sk_log_debug(8,sk_cycle->log,0,"The current line number of connections ",sroot->count);
	listnode->next = NULL;
	if(sroot->head == NULL) {
		sroot->head = listnode;
		sroot->tail = listnode;
	}
	else{
		sroot->tail->next = listnode;
		sroot->tail = listnode;
	}

	return ;
}

int list_delete(list_t * sroot,Listnode *listnode)
{
	Listnode * p_listnode = sroot->head;
	Listnode * pre_listnode;

	if(p_listnode == NULL){
		return 0;
	}
	if(p_listnode == listnode)
	{
		sroot->count --;
		//sroot has only one node
		if(sroot->tail == listnode)
		{

			sroot->head = NULL;
			sroot->tail = NULL;
		}
		else{
			sroot->head = p_listnode->next;
		}
		return 1;
	}
	while(p_listnode != listnode && p_listnode->next != NULL){
		pre_listnode = p_listnode;
		p_listnode = p_listnode->next;
	}

	if(p_listnode == listnode)
	{

		pre_listnode->next = p_listnode->next;
		sroot->count --;

		return 1;
	}else{
		return 0;
	}

}
void
list_event(sk_event_blacklist_t *p)
{


	if(p != NULL ){
		struct listnode* p_listnode = sroot.head;

		while(p_listnode != NULL){
				p_listnode = p_listnode->next;
			}

		p_listnode = sroot.head;

		while(p_listnode->next != NULL && p_listnode->point != p){
			p_listnode = p_listnode->next;
		}

		if(p_listnode->point == p )
		{

			list_delete (&sroot,p_listnode);
			free(p_listnode);
			p_listnode = sroot.head;
			while(p_listnode != NULL){
				p_listnode = p_listnode->next;
			}
		}else{
			return ;
		}
	}else{
		return ;
	}

}

sk_ptr_t
sk_event_blacklist_timer_init(sk_log_t *log)
{

	if(&sk_event_blacklist_rbtree == NULL) return SK_ERR;
	if(&sk_event_blacklist_timer_rbtree == NULL) return SK_ERR;
	sk_init_rbtree(&sk_event_blacklist_rbtree,&sk_event_blacklist_sentinel,sk_rbtree_insert_timer_value);
	sk_init_rbtree(&sk_event_blacklist_timer_rbtree,&sk_event_blacklist_timer_sentinel,sk_rbtree_insert_timer_value);
	//list_init(&sroot);
	return SK_OK;
}
static inline sk_rbtree_node_t *
sk_rbtree_search(sk_rbtree_node_t *root, sk_rbtree_key_t key)
{

	sk_rbtree_node_t *node = root;
	sk_rbtree_node_t *sentinel = sk_event_blacklist_rbtree.sentinel;
	int ret;
	while(node != sentinel && node){

		ret = node->key - key;

		if(0 < ret){
			node = node->left;

		}else if(0 > ret){
			node = node->right;

		}else{

			return node;
		}
	}

	return NULL;
}

sk_event_blacklist_t *
sk_event_blacklist_find(sk_rbtree_key_t key)
{
	sk_event_blacklist_t	*sev;
	sk_rbtree_node_t 	*node,*root,*sentinel;

	if(sk_event_blacklist_rbtree.root == &sk_event_blacklist_sentinel){
		return NULL;
	}
	/*
	if(sk_event_blacklist_rbtree.root->key == key){

			node =  sk_event_blacklist_rbtree.root ;
			sev =(sk_event_blacklist_t *)((char *)node -offsetof(sk_event_blacklist_t,blacklist));

			return sev;
	}
	 */
	root =sk_event_blacklist_rbtree.root;

	node = sk_rbtree_search(root,key);

	if (node){


		sev =(sk_event_blacklist_t *)((char *)node -offsetof(sk_event_blacklist_t,blacklist));

		return sev;

	}else{

		//sk_log_debug(8,sk_cycle->log,1,"keyqq",key);
		return NULL;
	}

}


/*
 * 处理红黑树所有超时事件
 */
void
sk_event_blacklist_expire_timer(void)
{
	sk_event_blacklist_t			*ev;
	sk_rbtree_node_t	*node,*root,*sentinel;

	sentinel = sk_event_blacklist_timer_rbtree.sentinel;

	for( ;; ){
		//sk_mutex_lock(sk_event_timer_mutex);

		root = sk_event_blacklist_timer_rbtree.root;
		if(root == sentinel){
			return ;
		}
		node = sk_rbtree_min(root,sentinel);//获得key最小的节点

		/*	node->key <= sk_current_tine */

		if((sk_msec_int_t)node->key - (sk_msec_int_t)sk_current_msec <= 0){
			/*
			 * 通过偏移来获取当前timer所在的event
			 */

			ev =(sk_event_blacklist_t *)((char *)node -offsetof(sk_event_blacklist_t,timer));

			//list_event(ev);

			sk_rbtree_delete(&sk_event_blacklist_timer_rbtree,&ev->timer);
			sk_rbtree_delete(&sk_event_blacklist_rbtree,&ev->blacklist);

			if(ev) free(ev);
			//ev->handler(ev);
			continue;
		}
		break;
	}
//	sk_mutex_unlock(sk_event_timer_mutex);
}



void
sk_event_add_blacklist_timer(sk_event_blacklist_t *ev,sk_msec_t timer)
{
	sk_msec_t			key;
	sk_msec_int_t		diff;
	key = sk_current_msec + timer;

	if(ev->timer_set){
		diff = (sk_msec_int_t) (key - ev->timer.key);
		if(abs(diff) < SK_TIMER_LAZY_DELAY){
			return ;
		}
		//sk_log_debug_u(8,sk_cycle->log,0,"event timer key ",ev->timer.key);
		sk_event_del_blacklist_timer(ev);
	}
	ev->timer.key = key;

	//sk_mutex_lock(sk_event_timer_mutex);

	sk_rbtree_insert(&sk_event_blacklist_timer_rbtree,&ev->timer);

	//sk_mutex_unlock(sk_event_timer_mutex);
	ev->timer_set = 1;

}


void
sk_event_add_blacklist(sk_event_blacklist_t *bev,sk_rbtree_key_t key)
{

	bev->blacklist.key = key;

	sk_rbtree_insert(&sk_event_blacklist_rbtree,&bev->blacklist);


}


