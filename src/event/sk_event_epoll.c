// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
#include <sys/epoll.h>

static int 					ep = -1;
static struct epoll_event 	*event_list;


static sk_uptr_t				nevents;

sk_ptr_t
sk_init_epoll(sk_cycle_t *cycle)
{
	if(ep == -1) {
		ep = epoll_create(512);
	}
	nevents = 1024;
	event_list = malloc(sizeof(struct epoll_event) * nevents);
	if (event_list == NULL )return SK_ERR;
	sk_event_flags = SK_USE_CLEAR_EVENT | SK_USE_GREEDY_EVENT|SK_USE_EPOLL_EVENT;
	return SK_OK;

}

sk_ptr_t
sk_epoll_add_event(sk_event_t *ev,sk_ptr_t event ,sk_uptr_t flags)
{
		sk_t					op,err;
		sk_event_t				*e;
		sk_connection_t		*c;
		struct epoll_event	ee;
		u32_t					events,prev;
		c = ev->data;

		events = (u32_t)event;


		if(event == SK_READ_EVENT){
			e = c->write;
			prev = EPOLLOUT;
#if(SK_READ_EVENT != EPOLLIN)
			events = EPOLLIN;
#endif
		}else{
			e = c->read;
			prev = EPOLLIN;
#if(SK_WRITE_EVENT != EPOLLOUT)
			events = EPOLLOUT;
#endif
		}

		if(e->active){
			op = EPOLL_CTL_MOD;
			events |= prev;
		}else{
			op = EPOLL_CTL_ADD;
		}

		ee.events = events |(u32_t) flags;

		ee.data.ptr = (void *)((sk_uptr_t)c |ev->instance);


		if(epoll_ctl(ep,op,c->fd,&ee) == -1){
			return SK_ERR;
		}
		ev->active = 1;
		return SK_OK;
}

sk_ptr_t
sk_epoll_del_event(sk_event_t *ev,sk_ptr_t event ,sk_uptr_t flags)
{
		sk_t					op,err;
		sk_event_t				*e;
		sk_connection_t		*c;
		struct epoll_event	ee;
		u32_t					events,prev;

		if(flags &SK_CLOSE_EVENT){
			ev->active = 0;
			return SK_OK;
		}
		c = ev->data;

		if(event == SK_READ_EVENT){
			e = c->write;
			prev = EPOLLOUT;

		}else{
			e = c->read;
			prev = EPOLLIN;
		}

		if(e->active){
			op = EPOLL_CTL_MOD;
			ee.events = prev|(u32_t)flags;
			ee.data.ptr = (void *)((sk_uptr_t)c |ev->instance);
		}else{
			op = EPOLL_CTL_DEL;
			ee.events = 0;
			ee.data.ptr = NULL;
		}

		if(epoll_ctl(ep,op,c->fd,&ee) == -1){
			return SK_ERR;
		}
		ev->active = 0;
		return SK_OK;
}


static sk_ptr_t
sk_epoll_process_events(sk_cycle_t *cycle,sk_msec_t timer,sk_uptr_t flags)
{
	sk_t				events,err;
	sk_ptr_t			instance,i;
	u32_t				revents;
	sk_event_t			*rev,*wev,**queue;
	sk_connection_t	*c;


	events = epoll_wait(ep,event_list,(int)nevents,timer);

	err = (events == -1)?errno:0;
	if(flags & SK_UPDATE_TIME ||sk_event_timer_alarm){

		sk_time_update();
	}

	if(err){
		if(err == EINTR){
			if(sk_event_timer_alarm){
				sk_event_timer_alarm = 0;
				return SK_OK;
			}
		}else{
			//log
		}
		return SK_ERR;
	}

	if(events ==0){
			if(timer != SK_TIMER_INFINITE){
				return SK_OK;
			}
			return SK_ERR;
	}

	for(i = 0; i< events; i++){
		c = event_list[i].data.ptr;

		instance = (sk_uptr_t) c & 1;
		c = (sk_connection_t *)((sk_uptr_t)c &(sk_uptr_t)~1);
		rev = c->read;

		if(c->fd == -1|| rev->instance != instance){
			continue;
		}
		revents = event_list[i].events;

		if(revents & (EPOLLERR|EPOLLHUP)){
			//sk_log
		}
		if((revents &(EPOLLERR|EPOLLHUP))
				&& (revents &(EPOLLIN|EPOLLOUT))==0){
			revents |= EPOLLIN|EPOLLOUT;
			//sk-log
		}

		if((revents & EPOLLIN) && rev->active){

			rev->ready = 1;
			if(flags & SK_POST_EVENTS){
				queue = (sk_event_t **)(rev->accept ?
						&sk_posted_accept_events:&sk_posted_events);

				sk_locked_post_event(rev,queue);
			}else{
				rev->handler(rev);
			}
		}
		wev = c->write;
		if((revents & EPOLLOUT) && wev->active){
			if(c->fd == -1 ||wev->instance != instance){
				continue;
			}
			wev->ready = 1;
			if(flags & SK_POST_EVENTS){
				sk_locked_post_event(wev,&sk_posted_events);
			}else{
				wev->handler(wev);
			}
		}
	}
	return SK_OK;

}
sk_ptr_t
sk_process_events(sk_cycle_t *cycle,sk_msec_t timer,sk_uptr_t flags)
{
	sk_epoll_process_events(cycle,timer,flags);
}
