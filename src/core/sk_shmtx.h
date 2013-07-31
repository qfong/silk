// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_SHMTX_H_INCLUDE_
#define _SK_SHMTX_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

typedef long				sk_atomic_int_t;
typedef unsigned long	sk_atomic_uint_t;

typedef struct{
	u_char				*addr;
	size_t				size;
	sk_str_t			name;
	sk_log_t			*log;
	sk_uptr_t			exists;
}sk_shm_t;

typedef volatile sk_atomic_uint_t		sk_atomic_t;

typedef struct{
	sk_atomic_t		*lock;
	sk_uptr_t			spin;
}sk_shmtx_t;

#define sk_atomic_cmp_set(lock,old,set)	\
	__sync_bool_compare_and_swap(lock,old,set)

#define sk_atomic_fentch_add(value,add)	\
	__sync_fetch_and_add(value,add)

//amd 64	=>x86-64,x64
//i386 	=>32bits

#if(__i386__||__i386 || __amd64__ ||__amd64)
#define sk_cpu_pause()			__asm__("pause")
#else
#define sk_cpu_pause()
#endif


#define sk_trylock(lock)		(*(lock) == 0 && sk_atomic_cmp_set(lock,0,1))
#define sk_unlock(lock)		*(lock) =0

#define sk_sched_yield()		sched_yield()
#endif
