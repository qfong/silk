// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
ssize_t sk_nc_recv(sk_connection_t *c,u_char *buf,size_t size)
{
	ssize_t 		n;
	int				err;
	sk_event_t		*rev;

	rev = c->read;

	do{
		n = recv(c->fd,buf,size,0);

		if(n == 0){
			rev->ready = 0;
			return n;
		}else if( n > 0 ){

			if((size_t) n < size && !(sk_event_flags & SK_USE_GREEDY_EVENT)){
				rev->ready = 0;
			}
			return n;
		}
		err = errno;
		if(err == EAGAIN || err == EINTR){
			n = SK_AGAIN;
		}else{
			//error log
			break;
		}

	}while(err == EINTR);

	rev->ready = 0;

	if(n == SK_ERR){
		//rev->error = 1;
	}

	return n;

}


ssize_t
sk_nc_send(sk_connection_t *c,u_char *buf,size_t size)
{
	ssize_t		n;
	int				err;
	sk_event_t		*wev;
	wev = c->write;
	for( ;; ){
		n = send(c->fd,buf,size,0);

		err = errno;
		//sk_log_debug(8,sk_cycle->log,0,"send() n ",n);
		//sk_log_debug_str(8,sk_cycle->log,0,"send()  buf",buf);

		if(n > 0){
			if( n <(ssize_t) size){
				wev->ready = 0;
			}
			c->send += n;
			return n;
			}
		if(n == 0){
			wev->ready = 0;
			return n;
		}
		if(err == EAGAIN || err == EINTR){
			wev->ready = 0;
			if(err == EAGAIN){
				//sleep(1);
				//continue;
				return SK_AGAIN;
			}
		}else{
			//wev->error = 1;
			// log error
			return SK_ERR;
		}
	}
}

