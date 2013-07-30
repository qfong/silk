// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
#include <sys/mman.h>
sk_ptr_t
sk_shm_alloc(sk_shm_t *shm)
{
	shm->addr = (u_char *)mmap(NULL,shm->size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_SHARED,-1,0);
	if(shm->addr == MAP_FAILED){
		return SK_ERR;
	}
	return SK_OK;
}


sk_ptr_t
sk_shmtx_create(sk_shmtx_t *mtx,void *addr,u_char *name)
{
	mtx->lock = addr;
	if(mtx->spin == (sk_uptr_t) -1){
		return SK_OK;
	}
	mtx->spin = 2048;
	return SK_OK;
}

sk_uptr_t
sk_shmtx_trylock(sk_shmtx_t *mtx)
{
	sk_atomic_uint_t val;

	val = *mtx->lock;

	return ((val & 0x80000000) ==0
			&& sk_atomic_cmp_set(mtx->lock,val,val|0x80000000));
}

sk_uptr_t
sk_shmtx_lock(sk_shmtx_t *mtx)
{
	sk_uptr_t			i,n;
	sk_atomic_uint_t	val;

	for(;;){
		val = *mtx->lock;

		if((val & 0x80000000) == 0
				&& sk_atomic_cmp_set(mtx->lock,val,val|0x80000000)){
			return SK_OK;
		}
		if(sk_ncpu>1){
			for(n = 1; n < mtx->spin; n<<=1){
				for(i = 0; i < n;i++){
					sk_cpu_pause();
				}
				val= *mtx->lock;
				if((val &0x80000000)==0
						&& sk_atomic_cmp_set(mtx->lock,val,val|0x80000000)){
					return SK_OK;
				}

			}
		}
		sk_sched_yield();
	}
}


void
sk_shmtx_unlock(sk_shmtx_t *mtx)
{
	sk_atomic_uint_t 			val,old,wait;
	if(mtx->spin != (sk_uptr_t) -1){
		//log
	}
	for(;;){
		old = *mtx->lock;
		wait = old & 0x7fffffff;
		val = wait ? wait-1 :0;

		if(sk_atomic_cmp_set (mtx->lock,old,val)){
			break;
		}
	}
}
