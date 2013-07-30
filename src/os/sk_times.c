// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

static sk_atomic_t			sk_time_lock;
void
sk_init_time(void)
{
	sk_time_update();
}
void
sk_time_update(void)
{
	time_t			sec;
	sk_uptr_t		msec;
	//sk_time_t		*tp;


	struct timeval	tv;

	if(!sk_trylock(&sk_time_lock)) return ;
	sk_gettimeofday(&tv);

	sec= tv.tv_sec;
	msec = tv.tv_usec/1000;

	sk_current_msec = (sk_msec_t) sec *1000 +msec;

	__sync_synchronize();
	sk_unlock(&sk_time_lock);
}

