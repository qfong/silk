// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;




#include "sk_config.h"
#include "sk_core.h"


#define sk_calloc_buf(pool) sk_pcalloc(pool,sizeof(sk_buf_t));
sk_buf_t *
sk_create_temp_buf(sk_pool_t *pool,size_t size)
{
	/*
	 * 	u_char			*pos; //这段数据开始的位置
	 * 	u_char			*last;//这段数据结束的位置
	 *
	 * 	off_t			file_pos;//这段数据的开始位置在文件中的偏移量
	 * 	off_t			file_last;//这段数据的结束位置在文件中的偏移量
	 *
	 * 	u_char			*start; //这块内存的开始地址
	 * 	u_char			*end;//这块内存的结束地址
	 */

	sk_buf_t		*b;
	b = sk_calloc_buf(pool);
	if(b == NULL){
		return NULL;
	}

	b->start = sk_palloc(pool,size);
	if(b->start == NULL){
		return NULL;
	}
	b->pos = b->start;
	b->last = b->start;
	b->end = b->last + size;
	b->temporary = 1;
	return b;

}
