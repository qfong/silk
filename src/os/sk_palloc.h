// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_PALLOC_H_INCLUDE_
#define _SK_PALLOC_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
#define  SK_POOL_ALGINMENT	16
#ifndef SK_ALIGNMENT
#define SK_ALIGNMENT 			sizeof(unsigned long)
#endif

typedef void (*sk_pool_cleanup_pt)(void *data);
struct sk_pool_cleanup_s{
	sk_pool_cleanup_pt	handler;
	sk_pool_cleanup_t		*next;
	void					*data;
};


struct sk_pool_large_s{
	sk_pool_large_t		*next;
	void					*alloc;
};


typedef struct{
	u_char					*last;	//last  为内存池开始分配的地址
	u_char					*end;	//end   为内存池结束的地址
	sk_pool_t				*next;
	sk_uptr_t				failed;
}sk_pool_data_t;

struct sk_pool_s{
	sk_pool_data_t		d;
	size_t					max;
	sk_pool_t				*current;
	sk_chain_t				*chain;
	sk_pool_large_t		*large;
	sk_pool_cleanup_t		*cleanup;
	sk_log_t				*log;
};

void * sk_palloc(sk_pool_t *pool,size_t size);
void * sk_pnalloc(sk_pool_t *pool,size_t size);
#endif
