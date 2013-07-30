// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

static sk_uptr_t		sk_timer_resolution;

sk_atomic_t			*sk_accept_mutex_ptr;
volatile	sk_msec_t	sk_current_msec;
sk_uptr_t				sk_use_accept_mutex;
sk_ptr_t				sk_accept_disabled;
sk_shmtx_t				sk_accept_mutex;
sk_shmtx_t				sk_mempool_mutex;
sk_uptr_t				sk_accept_mutex_held;
sk_uptr_t				sk_accept_events;
sk_msec_t				sk_accept_mutex_delay;

sig_atomic_t			sk_event_timer_alarm;
sk_uptr_t				sk_event_flags;

static  sk_atomic_t	connection_counter = 1;
sk_atomic_t			*sk_connection_counter = &connection_counter;

sk_ptr_t
sk_event_module_init(sk_cycle_t *cycle)
{
		sk_shm_t			shm;
		size_t				size,cl;
		u_char				*shared;
		int					limit;
		struct				rlimit rlmt;


		if(getrlimit(RLIMIT_NOFILE,&rlmt) == -1){
			//ERR LOG
		}

		if(sk_accept_mutex_ptr){
			return SK_OK;
		}
		cl = 128;
		size = cl /*sk_accept_mutex*/
				+cl /*sk_connection_counter*/
				+cl;/*sk_temp_number*/


		shm.size = size;
		shm.name.len = sizeof("sk_share_zone");
		shm.name.data = (u_char*)"sk_share_zone";
		shm.log = cycle->log;
		if(sk_shm_alloc(&shm) !=  SK_OK){
			FATAL_ERR("malloc shmtx error");
			return SK_ERR;
		}
		shared = shm.addr;
		sk_accept_mutex_ptr = (sk_atomic_t *)shared;
		sk_accept_mutex.spin = (sk_uptr_t) -1;
		if(sk_shmtx_create(&sk_accept_mutex,shared,cycle->lock_file.data)!= SK_OK){
			FATAL_ERR("malloc shmtx error");
			return SK_ERR;
		}
		sk_mempool_mutex.spin = (sk_uptr_t) -1;
		if(sk_shmtx_create(&sk_mempool_mutex,shared,cycle->lock_file.data)!= SK_OK){
			FATAL_ERR("malloc shmtx error");
			return SK_ERR;
			}
		sk_connection_counter = (sk_atomic_t *)(shared +1 * cl);
		(void)sk_atomic_cmp_set(sk_connection_counter,0,1);

		//sk_temp_number = (sk_atomic_t *)(shared+2 * cl);


		cycle->connection_n = 1024;
		return SK_OK;
}

void
sk_timer_signal_handler(int signo)
{
	sk_event_timer_alarm = 1;
}

sk_ptr_t
sk_init_event_process(sk_cycle_t *cycle)
{
	sk_uptr_t				m,i;
	sk_connection_t		*c,*next,*old;
	sk_event_t				*rev,*wev;
	sk_listening_t		*ls;

	sk_timer_resolution = 600;
	sk_accept_mutex_delay = 500;
	sk_use_accept_mutex = 1;
	sk_accept_mutex_held = 0;

	if(sk_timer_resolution){
		struct sigaction 	sa;
		struct itimerval	itv;

		sk_memzero(&sa,sizeof(struct sigaction));
		sa.sa_handler = sk_timer_signal_handler;
		sigemptyset(&sa.sa_mask);

		if(sigaction(SIGALRM,&sa,NULL) == -1) return SK_ERR;

		itv.it_interval.tv_sec = sk_timer_resolution/1000;
		itv.it_interval.tv_usec = (sk_timer_resolution %1000) * 1000;
		itv.it_value.tv_sec = sk_timer_resolution /1000;
		itv.it_value.tv_usec = (sk_timer_resolution %1000) *1000;

		if(setitimer(ITIMER_REAL,&itv,NULL) == -1 )return SK_ERR;

	}
	cycle->connection_n = 1024;
	cycle->connections = malloc(sizeof(sk_connection_t)*cycle->connection_n);

	c = cycle->connections;

	cycle->read_events = malloc(sizeof(sk_event_t) * cycle->connection_n);

	if(cycle->read_events == NULL) return SK_ERR;

	rev = cycle->read_events;
	for(i = 0; i < cycle->connection_n; i++){
		rev[i].closed = 1;
		rev[i].instance = 1;
	}

	cycle->write_events = malloc(sizeof(sk_event_t) * cycle->connection_n);

	if(cycle->write_events == NULL) return SK_ERR;

	wev = cycle->write_events;
	for(i = 0; i < cycle->connection_n; i++){
		wev[i].closed = 1;
		wev[i].instance = 1;
	}

	i = cycle->connection_n;
	next = NULL;
	do{
		i--;
		c[i].data = next;
		c[i].read = &cycle->read_events[i];
		c[i].write = &cycle->write_events[i];
		c[i].fd = (int) -1;
		next = &c[i];
	}while(i);


	cycle->free_connections = next;
	cycle->free_connection_n = cycle->connection_n;

	ls = cycle->listening.elts;
	for (i=0; i< cycle->listening.nelts;i++){
		c = sk_get_connection(ls[i].fd,cycle->log);
		if(c == NULL) return SK_ERR;

		c->listening = &ls[i];
		ls[i].connection = c;
		rev = c->read;
		rev->log = c->log;
		rev->accept = 1;

		rev->handler = sk_event_accept;

		if(sk_use_accept_mutex){
			continue;
		}
		if(sk_epoll_add_event(rev,SK_READ_EVENT,0)==SK_ERR) return SK_ERR;



	}
	return SK_OK;
}


sk_ptr_t
sk_handle_read_event(sk_event_t *rev,sk_uptr_t flags)
{
	if(!rev->active && !rev->ready){

			if(sk_epoll_add_event(rev,SK_READ_EVENT,SK_CLEAR_EVENT) == SK_ERR) return SK_ERR;
			return SK_OK;
	}
	return SK_OK;
}

sk_ptr_t
sk_handle_write_event(sk_event_t *wev,sk_uptr_t flags)
{
	if(!wev->active && !wev->ready){
			if(sk_epoll_add_event(wev,SK_WRITE_EVENT,SK_CLEAR_EVENT) == SK_ERR) return SK_ERR;
			return SK_OK;
	}
	return SK_OK;
}


void
sk_process_events_and_times(sk_cycle_t *cycle)
{
	sk_uptr_t				flags;
	sk_msec_t				timer,delta;

	if(sk_timer_resolution){
		timer = SK_TIMER_INFINITE;
		flags = 0;
	}
	if(sk_use_accept_mutex){

		if(sk_accept_disabled > 0){
			sk_accept_disabled--;
		}else{

			if(sk_trylock_accept_mutex(cycle) == SK_ERR){
				_exit();
			}
			if(sk_accept_mutex_held){
				flags |= SK_POST_EVENTS;
			}else{

				if(timer == SK_TIMER_INFINITE || timer > sk_accept_mutex_delay){
					timer = sk_accept_mutex_delay;
				}
			}

		}
	}
	delta = sk_current_msec;

	(void) sk_process_events(cycle,timer,flags);
	delta = sk_current_msec - delta;

	if(sk_posted_accept_events){
		sk_event_process_posted(cycle,&sk_posted_accept_events);
	}

	if(sk_accept_mutex_held){
		sk_shmtx_unlock(&sk_accept_mutex);
	}

	if(delta){

		sk_event_blacklist_expire_timer();
		sk_event_expire_timer();

	}

	if(sk_posted_events){
		sk_event_process_posted(cycle,&sk_posted_events);
		sk_event_t	*ev;
		ev = (sk_event_t *)sk_posted_events;
		if(ev == NULL){
			sk_process_detect_events(cycle);
			return ;
		}
	}
}
