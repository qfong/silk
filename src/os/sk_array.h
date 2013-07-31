// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_ARRAY_H_INCLUDE_
#define _SK_ARRAy_H_INCLUDE_
struct sk_array_s{
	void			*elts; //元素首地址
	sk_uptr_t		nelts; //已分配的个数
	size_t			size;  //每个元素的大小
	sk_uptr_t		nalloc;//数组容量
	sk_pool_t		*pool; //在pool所在内存池地址
};
#endif
