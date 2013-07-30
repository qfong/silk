// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#ifndef _SK_ALLOC_H_INCLUDE_
#define _SK_ALLOC_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

#define MAX_ALLOC		0x50000000
#define ALLOC_CHECK_SIZE(_s) do {\
	if((_s) > MAX_ALLOC )\
		printf("bad alloc request:%u buyes",(_s));\
}while (0)

#define ALLOC_CHECK_RESULT(_r,_s) do{\
	if(!(_r))\
		printf("out of memory: can't allocate %u bytes",(_s));\
}while (0)

#define sk_memzero(buf,n)	(void) memset(buf,0,n)
#define sk_memset(buf,c,n)	(void) memset(buf,c,n)

#define ALLOC_MAGIC 		0xFF00
#define ALLOC_C(_ptr)	(((u16_t*)(_ptr))[-3])
#define ALLOC_S(_ptr)	(((u32_t*)(_ptr))[-1])
/*
 * void 	-> 8 bytes |1 | 2 | 3 |4 | 5 | 6 | 7 | 8 |
 * u16_t	-> 2 bytes |1|2|  |3|4|  |5|6|   |7|8|
 * u32_t	-> 4 buyes |1|2|3|4|     |5|6|7|8|
 *
 * size + 6
 *   	ret->		| size of | 6 |
 *  				| 6 | size of |
 *  	ret+6		-- >|
 *
 * 				Stack 4	|1_|2_|3_|4_|5_|6_|7_|8_|
 * 				Stack 5	|1_|2_|3_|4_|5_|6_|7_|8_|
 * 				Stack 6	|1_|2_|3_|4_|5_|6_|7_|8_|
 * 	u16_t(ret)[-3] ----------->|xff00|
 * 	u32_t(ret)[-1] ----------------->|   sizeof  |
 * 	ret->		Stack 7	|1_|2_|3_|4_|5_|6_|7_|8_|
 *
 *
 */

static inline void* __DEL_ck_alloc(u32_t size){
	void* ret;

	if(!size) return NULL;

	ALLOC_CHECK_SIZE(size);
	ret = malloc(size+6);
	ALLOC_CHECK_RESULT(ret,size);

	ret += 6;

	ALLOC_C(ret) = ALLOC_MAGIC;
	ALLOC_S(ret) = size;

	//return memset(ret,0,size);
	return ret;
}

static inline void* __DEL_ck_realloc(void* orig,u32_t size){
	void* ret;
	u32_t old_size = 0;
	if(!size){
		if(orig) free(orig-6);
		return NULL;
	}

	if(orig){
		if (ALLOC_C(orig) != ALLOC_MAGIC ) printf("Bad alloc canary");
		old_size = ALLOC_S(orig);
		orig -= 6;
	}

	ALLOC_CHECK_SIZE(size);
	ret = realloc(orig,size+6);
	ALLOC_CHECK_RESULT(ret,size);

	ret += 6;

	ALLOC_C(ret) = ALLOC_MAGIC;
	ALLOC_S(ret) = size;

	if(size > old_size )
		memset(ret+old_size,0,size-old_size);

	return ret;

}

static inline void __DEL_ck_free(void *mem){
	if (mem){
		if (ALLOC_C(mem)!= ALLOC_MAGIC ) printf("Bad alloc canary");
		free(mem-6);
	}
}
#define sk_alloc		__DEL_ck_alloc
#define sk_realloc	__DEL_ck_realloc
#define ck_free		__DEL_ck_free
#endif
