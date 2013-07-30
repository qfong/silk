// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;


#ifndef _SK_CYCLE_H_INCLUDE_
#define _SK_CYCLE_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

struct sk_cycle_s{
	void					*cnf_ctx;
	sk_log_t				*log;
	sk_array_t				listening;
	sk_pool_t				*pool;
	sk_str_t				prefix;
	sk_cycle_t				*old_cycle;
	sk_connection_t		**files;
	sk_connection_t		**free_connections;
	sk_uptr_t				free_connection_n;
	sk_uptr_t				connection_n;
	sk_connection_t		*connections;

	sk_event_t				*read_events;
	sk_event_t				*write_events;

	sk_str_t				lock_file;
};

extern volatile sk_cycle_t		*sk_cycle;

sk_cycle_t * sk_init_cycle(sk_cycle_t *old_cycle);
#endif
