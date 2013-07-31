// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"


static sk_ptr_t
sk_enable_accept_events(sk_cycle_t *cycle)
{
	sk_uptr_t					i;
	sk_listening_t			*ls;
	sk_connection_t			*c;
	sk_event_t					*read;

	ls = cycle->listening.elts;

	for(i = 0;i<cycle->listening.nelts;i++){
		c = ls[i].connection;

		read = c->read;
		if(sk_epoll_add_event(c->read,SK_READ_EVENT,0) == SK_ERR){
			return SK_ERR;
		}

	}
	return SK_OK;
}
static sk_ptr_t
sk_disable_accept_events(sk_cycle_t *cycle)
{
	sk_uptr_t					i;
	sk_listening_t			*ls;
	sk_connection_t			*c;
	sk_event_t					*read;

	ls = cycle->listening.elts;

	for(i = 0;i<cycle->listening.nelts;i++){
		c = ls[i].connection;
		if(!c->read->active){
			continue;
		}


		if(sk_epoll_del_event(c->read,SK_READ_EVENT,0) == SK_ERR){
			return SK_ERR;
		}
		return SK_OK;
	}
	return SK_OK;
}
sk_ptr_t
sk_trylock_accept_mutex(sk_cycle_t *cycle)
{

	if(sk_shmtx_trylock(&sk_accept_mutex)){

		if(sk_accept_mutex_held){

			return SK_OK;
		}

		if(sk_enable_accept_events(cycle) == SK_ERR){
			sk_shmtx_unlock(&sk_accept_mutex);
			return SK_ERR;
		}
		sk_accept_mutex_held = 1;
		return SK_OK;
	}

	if(sk_accept_mutex_held){

		if(sk_disable_accept_events(cycle)== SK_ERR){
			return SK_ERR;
		}

		sk_accept_mutex_held = 0;
	}
	return SK_OK;
}

void
sk_event_accept(sk_event_t *ev)
{
	socklen_t				socklen;
	sk_t					err,s;
	sk_log_t				*log;
	sk_event_t				*rev,*wev;
	sk_listening_t		*ls;
	sk_connection_t		*c,*lc;
	u_char					sa[SK_SOCKADDRLEN];

	lc = ev->data;
	ls = lc->listening;
	ev->ready = 0;
	ev->available = 0;
	do{
		socklen = SK_SOCKADDRLEN;
		s = accept(ls->fd,(struct sockaddr *)sa,&socklen);
		if(s == -1){
			err = errno;
			if(err == EAGAIN){
				return ;
			}
			if(err == ECONNABORTED){
				if(ev->available){
					continue;
				}
			}
			return;
		}

	sk_accept_disabled = sk_cycle->connection_n /8 - sk_cycle->free_connection_n;
	//printf("accept ---\n");
	c = sk_get_connection(s,ev->log);
	if(c == NULL) close(s);

	sk_nonblocking(s);

	c->log = sk_cycle->log;
	c->listening = ls;
	rev = c->read;
	wev = c->write;

	wev->ready =1;

	wev->log = c->log;
	rev->log = c->log;
	ls->handler(c);
	}while(ev->available);

}
