// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;


#ifndef _NEOSILK_BUF_H_INCLUDE_
#define _NEOSILK_BUF_H_INCLUDE_

#include "sk_core.h"
#include "sk_config.h"

typedef struct{
	sk_ptr_t				num;
	size_t					size;
}sk_bufs_t;

typedef struct {
	u_char			*pos; //这段数据开始的位置
	u_char			*last;//这段数据结束的位置

	off_t			file_pos;//这段数据的开始位置在文件中的偏移量
	off_t			file_last;//这段数据的结束位置在文件中的偏移量

	u_char			*start; //这块内存的开始地址
	u_char			*end;//这块内存的结束地址


	unsigned 		temporary:1;//内容不能被改变

}sk_buf_t;

sk_buf_t * sk_create_temp_buf(sk_pool_t *pool,size_t size);
struct sk_chain_s{
	sk_buf_t				*buf;
	sk_chain_t				*next;
};

#endif /* _NEOSILK_BUF_H_INCLUDE_ */
