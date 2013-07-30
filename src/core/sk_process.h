// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#ifndef _SK_PROCESS_H_INCLUDE_
#define _SK_PROCESS_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

int              	sk_argc;
char           	**sk_argv;
char          	**sk_os_argv;

extern  sk_pid_t	sk_pid;
extern sk_uptr_t	count_num;
typedef void(*sk_spawn_proc_pt)(sk_cycle_t *cycle,void *data);
sk_pid_t sk_spawn_process(sk_cycle_t *cycle,sk_spawn_proc_pt proc,
		void * data, char *name,sk_ptr_t respawn);

typedef struct{
	sk_pid_t				pid;
	int						status;
	int						channel[2];

	sk_spawn_proc_pt		proc;
	void					*data;
	void					*name;

	unsigned				respawn;

}sk_process_t;

#endif
