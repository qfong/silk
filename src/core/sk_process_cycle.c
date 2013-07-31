// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

static u_char master_process[] = "master process";
u_long	cpu_affinity;

typedef struct{
		int 	signo;
		char	*signame;
		char	*name;
		void	(*handler)(int signo);
}sk_signal_t;

void
sk_signal_handler(int signo)
{
    char            *action;
    sk_ptr_t       ignore;
    int		        err;
    sk_signal_t    *sig;

    ignore = 0;

    err = errno;

    switch (signo) {

	case sk_signal_value(SK_SHUTDOWN_SIGNAL):
	//	sk_quit = 1;
	  //  action = ", shutting down";
		break;
	case SIGINT:
		//sk_terminate = 1;
		break;
    }



}


void
sk_signal_handler(int signo);

sk_signal_t	signals[]={
		/*
		{ sk_signal_value(SK_RECONFIGURE_SIGNAL),
				"sig" sk_values(SK_RECONFIGURE_SIGNAL),
				"reload",
				sk_signal_handler
		},
		*/
		{ sk_signal_value(SK_SHUTDOWN_SIGNAL),
			"SIG" sk_values(SK_SHUTDOWN_SIGNAL),
			"quit",
			sk_signal_handler
		},
		//{SIGALRM,"SIGALRM","",sk_signal_handler},
		{SIGINT,"SIGINT","",sk_signal_handler},
		{SIGIO,"SIGIO","",sk_signal_handler},
		{SIGCHLD,"SIGCHLD","",sk_signal_handler},
		{ SIGSYS,"SIGSYS, SIG_IGN","",SIG_IGN},
		{ SIGPIPE, "SIGPIPE,SIG_IGN","",SIG_IGN},
		{ 0, NULL, "",NULL}
};
sk_ptr_t
sk_init_signals(sk_log_t *log){
	sk_signal_t		*sig;
	struct sigaction 	sa;

	for(sig= signals;sig->signo!= 0;sig++){
		sk_memzero(&sa,sizeof(struct sigaction));
		sa.sa_handler = sig->handler;
		sigemptyset(&sa.sa_mask);
		if(sigaction(sig->signo,&sa,NULL)==-1){
			return SK_ERR;
		}

	}
	return SK_OK;

}

static void
sk_worker_process_init(sk_cycle_t *cycle,sk_ptr_t priority)
{
	if(setpriority(PRIO_PROCESS,0,0)==-1){
			//error log;
		}
	FATAL_ERR_CHECK(sk_init_epoll(cycle),"epoll event init error");

	FATAL_ERR_CHECK(sk_init_event_timer(cycle->log),"event timer init error");
	FATAL_ERR_CHECK(sk_event_blacklist_timer_init(cycle->log),
			"blacklist timer init error");
	FATAL_ERR_CHECK(sk_init_event_process(cycle),"event process init error");

	sk_init_signals(sk_cycle->log);
}

static void
sk_worker_process_cycle(sk_cycle_t *cycle,void *data)
{
	sk_setproctitle("worker process");

	sk_worker_process_init(cycle,1);

	for(;;){
		sk_process_events_and_times(cycle);
	}

}


static void
sk_start_worker_process(sk_cycle_t *cycle,sk_ptr_t n,sk_ptr_t type)
{
	sk_ptr_t				i;
	for (i = 0; i < n; i++ ){
		sk_spawn_process(cycle,sk_worker_process_cycle,NULL,"worker process",type);

	}
	return ;
}
sk_ptr_t
sk_master_process_cycle(sk_cycle_t *cycle)
{
	sk_t					n;
	sk_t					cpu_n;

	char					*title;
	u_char					*p;
	size_t					size;
	sk_ptr_t				i;
	sigset_t				set;
	// struct itimerval	itv;
	sk_uptr_t				live;
	sk_msec_t				delay;
	sk_listening_t		*ls;



	sigemptyset(&set);		//初始化set指向的信号集，清除其中所有信号
	sigaddset(&set,SIGCHLD);//在一个进程终止或停止时，将SIGCHLD信号发送给父进程
	sigaddset(&set,SIGALRM);//在alarm函数设置的计数器超时时，产生此信号
	sigaddset(&set,SIGIO);//此信号指示一个异步I/O事件。
	sigaddset(&set,SIGINT);//用户按中断键，终端驱动程序产生此信号，并送至前台进程组中的每一个进程
	sigaddset(&set,sk_signal_value(SK_RECONFIGURE_SIGNAL));
	sigaddset(&set,sk_signal_value(SK_REOPEN_SIGNAL));
	sigaddset(&set,sk_signal_value(SK_NOACCEPT_SIGNAL));
	sigaddset(&set,sk_signal_value(SK_TERMINATE_SIGNAL));
	sigaddset(&set,sk_signal_value(SK_SHUTDOWN_SIGNAL));
	sigaddset(&set,sk_signal_value(SK_CHANGEBIN_SIGNAL));
	/*
	if(sigprocmask(SIG_BLOCK,&set,NULL) == -1){
	//error log
	}
	sigemptyset(&set);
	*/
	size = sizeof(master_process);
	title = sk_pnalloc(cycle->pool,size);
	//设置process title；
	p = sk_cpymem(title,master_process,sizeof(master_process)-1);
	//sk_setproctitle(title);
	for (i = 0; i < sk_argc; i++) {
		*p++ = ' ';
		p = sk_cpystrn(p, (u_char *) sk_argv[i], size);
	}

	sk_setproctitle(title);

	cpu_n =  sysconf(_SC_NPROCESSORS_ONLN);

	n = sk_ctx->process;

	if ( n < 0 && n > cpu_n) FATAL_ERR("fork the process number error");

	sk_start_worker_process(cycle,n,-3);

	return SK_OK;
	/*
	for( ;; ){
	}
	*/
}

