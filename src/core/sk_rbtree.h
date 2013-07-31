// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_RBTREE_H_INCLUDE_
#define _SK_RBTREE_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

typedef sk_uptr_t						sk_rbtree_key_t;
typedef sk_ptr_t						sk_rbtree_key_int_t;


typedef struct sk_rbtree_node_s 	sk_rbtree_node_t;
typedef struct sk_rbtree_s			sk_rbtree_t;
typedef void(*sk_rbtree_insert_pt)(sk_rbtree_node_t *root,sk_rbtree_node_t *node,
		sk_rbtree_node_t *sentinel);

struct sk_rbtree_node_s{
		sk_rbtree_key_t		key;
		sk_rbtree_node_t		*left;
		sk_rbtree_node_t		*right;
		sk_rbtree_node_t		*parent;
		u_char					color;
		u_char					data;
};
struct sk_rbtree_s{
	sk_rbtree_node_t			*root;
	sk_rbtree_node_t			*sentinel;
	sk_rbtree_insert_pt 		insert;
};





#define sk_init_rbtree(tree,s,i)				\
	sk_rbtree_sentinel_init(s);					\
	(tree)->root = s;								\
	(tree)->sentinel = s;						\
	(tree)->insert = i;

#define sk_rbt_red(node)			((node)->color = 1)
#define sk_rbt_black(node)		((node)->color = 0)
#define sk_rbt_is_red(node)		((node)->color)
#define sk_rbt_is_black(node)	(!sk_rbt_is_red(node))
#define sk_rbt_copy_color(n1,n2) (n1->color = n2->color)

//为哨兵染黑色
#define sk_rbtree_sentinel_init(node) sk_rbt_black(node)

static inline sk_rbtree_node_t *
sk_rbtree_min(sk_rbtree_node_t *node,sk_rbtree_node_t *sentinel){
	while(node->left != sentinel){
		node = node->left;
	}
	return node;
}

void sk_rbtree_insert_timer_value(sk_rbtree_node_t *temp,sk_rbtree_node_t * node,
		sk_rbtree_node_t *sentinel);
#endif
