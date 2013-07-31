// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com
// Thanks to China Standard software Co.,Ltd ;


#include "sk_config.h"
#include "sk_core.h"

volatile sk_cycle_t	*sk_cycle;

#if 1
/*
 * sk_init_cycle
 * initiation process module & listen
 */
sk_cycle_t *
sk_init_cycle(sk_cycle_t *old_cycle)
{
	sk_uptr_t			i,n;
	sk_log_t			*log;
	sk_pool_t			*pool;
	sk_cycle_t			*cycle,**old;


	log = old_cycle->log;
	pool = sk_create_pool(16384,log);

	if(pool == NULL){
		FATAL_ERR("create pool error");
		return  NULL;
	}

	pool->log = log;

	cycle = sk_palloc(pool,sizeof(sk_cycle_t));
	if(cycle == NULL) return NULL;

	cycle->pool = pool;
	cycle->log = log;
	cycle->old_cycle = old_cycle;

	n = 1;
	cycle->listening.elts = sk_pcalloc(pool,n*sizeof(sk_listening_t));
	cycle->listening.nelts = 0;
	cycle->listening.size = sizeof(sk_listening_t);
	cycle->listening.nalloc = n;
	cycle->listening.pool = pool;

	FATAL_ERR_CHECK(sk_init_listening(cycle),"init listening port error");

	FATAL_ERR_CHECK(sk_open_listening_sockets(cycle),"bind socket error");

	FATAL_ERR_CHECK(sk_event_module_init(cycle),"mondule init error");

	return cycle;

}
#endif
