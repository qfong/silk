// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
void *
sk_array_push(sk_array_t *a)
{
	void				*elt,*new;
	size_t				size;
	sk_pool_t			*p;
	// a,array full
	if(a->nelts == a->nalloc) {
		size = a->size * a->nalloc;
		p = a->pool;

		if((u_char *)a->elts + size == p->d.last &&
				p->d.last + a->size <= p->d.end){
			//还有空间，若内存池的last指针指向数据数据区域的末尾，
			//且内存池未使用的区域可以分配一个size的大小
			p->d.last += a->size;
			a->nalloc ++;
		}else{
			//原数据区域扩展为原来的2倍

			new  = sk_palloc(p,2 * size);
			if(new == NULL )return NULL;

			sk_memcpy(new,a->elts,size);
			a->elts = new;
			a->nalloc *= 2;

		}
	}
	elt = (u_char*)a->elts + a->size*a->nelts;
	a->nelts ++;
	return elt;
}
