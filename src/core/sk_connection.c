// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;


#include "sk_config.h"
#include "sk_core.h"
 int sk_nonblocking(int s)
{
     int nb;
     nb = 1;
    return ioctl(s,FIONBIO,&nb);
}


void sk_init_connection(sk_connection_t *c);
sk_ptr_t
sk_init_listening(sk_cycle_t *cycle)
{
	sk_listening_t		*ls;
	sk_uptr_t				i;

	sk_ptr_t				port;

	port = sk_ctx->port;
	if(port < 0 ) return SK_ERR;

	for (i = port; i<port +1; i++){
		ls = sk_array_push(&cycle->listening);
		if(ls == NULL )return SK_ERR;

		ls->sockaddr.sin_port = htons(i);
		ls->sockaddr.sin_family = AF_INET;
		ls->type = SOCK_STREAM;
		ls->fd = -1;
		ls->handler = sk_init_connection;

		ls->log = cycle->log;
		ls->pool_size = 256;
		ls->rcvbuf = 65535;
		ls->sndbuf = 32768;
		ls->post_accept_timeout = 6000;
	}
	return SK_OK;
}

/*
 * sk_open_listening_sockets
 * bind & listen the port;
 */

sk_ptr_t
sk_open_listening_sockets(sk_cycle_t *cycle)
{
	int						reuseaddr;
	sk_uptr_t				i,tries,failed;
	sk_t					err;
	sk_log_t				*log;
	sk_t					s;
	sk_listening_t		*ls;


	reuseaddr = 1 ;
	log = cycle->log;

	for(tries = 5;tries; tries--){
		failed = 0;
		ls = cycle->listening.elts;
		for(i =0;i<cycle->listening.nelts;i++){
			if(ls[i].fd != -1) continue;

			s = socket(ls[i].sockaddr.sin_family,ls->type,0);
			if(s == -1 ){
				FATAL_ERR("socket error");
				return SK_ERR;
			}
			if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,
					(const void *)&reuseaddr,sizeof(int))< 0){
				save_msg_log("setsockopt,SO_REUSEADDR,warning,");
			}

			if(setsockopt(ls[i].fd,SOL_SOCKET,SO_RCVBUF,
					(const void *) &ls[i].rcvbuf,sizeof(int))< 0){
				save_msg_log("setsockopt,SO_RCVBUF,warning,");
			}
			if(setsockopt(ls[i].fd,SOL_SOCKET,SO_SNDBUF,
					(const void *) &ls[i].sndbuf,sizeof(int))< 0){
				save_msg_log("setsockopt,SO_RCVBUF,warning,");
			}
			if(bind(s,(struct sockaddr *)&(ls[i].sockaddr),
					sizeof(ls[i]. sockaddr))== -1 ){
					failed = 1;
					continue;
			}
			if(listen(s,ls->backlog) == -1){
				FATAL_ERR("listen error");
				return SK_ERR;
			}

			ls[i].listen = 1;
			ls[i].fd = s;
			if(!failed) return SK_OK;


		}

	}
	return SK_ERR;
}


sk_connection_t	*
sk_get_connection(sk_t s,sk_log_t *log)
{
	sk_uptr_t				instance;
	sk_event_t				*rev,*wev;
	sk_connection_t		*c;

	c = sk_cycle->free_connections;
	if(c == NULL) {
		c = sk_cycle->free_connections;
	}
	if(c == NULL) return NULL;

	sk_cycle->free_connections = c->data;
	sk_cycle->free_connection_n --;

	if(sk_cycle->files){
		sk_cycle->files[s] = c;
	}

	rev = c->read;
	wev = c->write;

	sk_memzero(c,sizeof(sk_connection_t));

	c->read = rev;
	c->write = wev;
	c->fd = s;
	c->log = log;

	instance = rev->instance;

	sk_memzero(rev,sizeof(sk_event_t));
	sk_memzero(wev,sizeof(sk_event_t));

	rev->instance = !instance;
	wev->instance = !instance;

	rev->index = 0xd0d0d0d0;
	wev->index = 0xd0d0d0d0;

	rev->data = c;
	wev->data = c;

	wev->write = 1;

	return c;
}

void
sk_free_connection(sk_connection_t *c)
{
	c->data = sk_cycle->free_connections;
	sk_cycle->free_connections = c;
	sk_cycle->free_connection_n ++;

}

void
sk_close_connection(sk_connection_t *c)
{
	int			err;
	sk_uptr_t	log_err,level;
	int			fd;

	if(c->fd == -1){
		return ;
	}

	if(c->read->timer_set){
		sk_event_del_timer(c->read);
	}

	if(c->write->timer_set){
		sk_event_del_timer(c->write);
	}



		if(c->read->active || c->read->disabled){
			sk_epoll_del_event(c->read,SK_READ_EVENT,SK_CLOSE_EVENT);
		}
		if(c->write->active || c->write->disabled){
			sk_epoll_del_event(c->read,SK_WRITE_EVENT,SK_CLOSE_EVENT);
		}


	if(c->read->prev){
		sk_delete_posted_event(c->read);
	}
	if(c->write->prev){
		sk_delete_posted_event(c->write);
	}
	c->read->closed =1;
	c->write->closed =1;

	//sk_reuseable_connection(c,0);
	sk_free_connection(c);

	//sk_destroy_pool(c->pool);

	log_err = c->log_err;
	fd = c->fd;
	c->fd = (int) -1;

	if(close(fd) == -1){
		err = errno;
		if(err == ECONNRESET||err == ENOTCONN){
			/*
			switch(log_err){
			case SK_ERROR_INFO:
					level = SILK_LOG_INFO;
					break;
			case SK_ERROR_ERR:
					level = SILK_LOG_ERR;
					break;
			default:
					level = SILK_LOG_CRIT;
					break;
			}
		}else{
			level = SILK_LOG_CRIT;
		}
		*/
		}
	}



}
