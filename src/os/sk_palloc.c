
// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include <stddef.h>
#include "sk_config.h"
#include "sk_core.h"

static void * sk_palloc_block(sk_pool_t *pool,size_t size);

static void * sk_palloc_large(sk_pool_t *pool,size_t size);

void *
sk_memalign(size_t alignment,size_t size,sk_log_t *log)
{
	sk_err_t		err;
	void			*p;
	err = posix_memalign(&p,alignment,size);
	if(err){
		p = NULL;
	}
	if(p == NULL){
		return NULL;
	}
	return p;
}


sk_pool_t	*
sk_create_pool(size_t size,sk_log_t *log)
{
	sk_pool_t			*p;
	sk_err_t			err;

	p = sk_memalign(SK_POOL_ALGINMENT,size,log);

	p->d.last = (u_char *)p +sizeof(sk_pool_t);
	p->d.end = (u_char *)p + size;
	p->d.next = NULL;
	p->d.failed = 0;

	size = size-sizeof(sk_pool_t);
	p->max = (size <(4096-1)?size:(4096-1));
	p->current = p;
	p->chain = NULL;
	p->large = NULL;
	p->cleanup = NULL;
	p->log = log;
	return p;

}

void *
sk_palloc(sk_pool_t *pool,size_t size)
{
	u_char				*m;
	sk_pool_t			*p;

	if(size <=  pool->max){

		p = pool->current;
		do{
			m = sk_align_ptr(p->d.last,SK_ALIGNMENT);
			if((size_t)(p->d.end - m)>= size){
				p->d.last = m + size;
				return m;
			}
			p = p->d.next;
		}while(p);
		return sk_palloc_block(pool,size);
	}

	return sk_palloc_large(pool,size);
}


void *
sk_pcalloc(sk_pool_t *pool,size_t size)
{
	void		*p;
	p = sk_palloc(pool,size);

	if(p){
		sk_memzero(p,size);
	}
	return p;
}
void *
sk_pnalloc(sk_pool_t *pool,size_t size)
{
	u_char			*m;
	sk_pool_t		*p;

	if(size <= pool->max){
		p = pool->current;
		do{
			m = p->d.last;
			if((size_t)(p->d.end -m )>= size){
				p->d.last = m + size;
				return m;
			}
			p = p->d.next;
		}while(p);
		return sk_palloc_block(pool,size);
	}

	return sk_palloc_large(pool,size);
}

static void *
sk_palloc_block(sk_pool_t *pool,size_t size)
{
	u_char			*m;
	size_t			psize;
	sk_pool_t		*p,*new,*current;

	psize = (size_t)(p->d.end - (u_char *)pool);

	m = sk_memalign(SK_POOL_ALGINMENT,psize,pool->log);
	if(m == NULL) return NULL;

	new = (sk_pool_t *)m;
	new->d.end = m + size;
	new->d.next = NULL;
	new->d.failed = 0;

	m += sizeof(sk_pool_data_t);
	m = sk_align_ptr(m,SK_ALIGNMENT);
	new->d.last = m + size;

	current = pool ->current;

	for(p = current;p->d.next; p = p->d.next){
		if(p->d.failed++ > 4){
			current = p->d.next;
		}
	}

	p->d.next = new;
	pool->current = current ? current:new;
	return m;
}


static void *
sk_palloc_large(sk_pool_t *pool,size_t size)
{
	void				*p;
	sk_uptr_t			n;
	sk_pool_large_t	*large;

	p = malloc(size);
	if(p == NULL) return NULL;

	n = 0;

	for(large = pool->large;large;large = large->next ){
		if(large->alloc == NULL){
			large->alloc = p;
			return p;
		}
		if(n++ > 3){
			break;
		}
	}

	large = sk_palloc(pool,sizeof(sk_pool_large_t));
	if(large == NULL){
		free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}
/*
 * destroy memory pool;
 */
void
sk_destroy_pool(sk_pool_t *pool)
{
	sk_pool_t			*p,*n;
	sk_pool_large_t	*l;
	sk_pool_cleanup_t	*c;

	for(c= pool->cleanup;c;c=c->next){
		if(c->handler){
			c->handler(c->data);
		}
	}
	for(l = pool->large;l;l=l->next){
		if(l->alloc){
			free(l->alloc);
		}
	}

	for(p = pool,n=pool->d.next;/*void*/;p=n,n=n->d.next){
		free(p);
		if(n == NULL) break;
	}
}
