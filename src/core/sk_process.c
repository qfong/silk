// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

sk_pid_t	sk_pid;
sk_uptr_t	count_num ;

int              	sk_argc;
char           	**sk_argv;
char          	**sk_os_argv;

sk_pid_t
sk_spawn_process(sk_cycle_t *cycle,sk_spawn_proc_pt proc,void * data, char *name,sk_ptr_t respawn)
{
	sk_ptr_t			s;
	sk_pid_t			pid;
	if(respawn > 0){
		s = respawn;
	}
	pid = fork();
	switch (pid){
	case 0:
		sk_pid = getpid();
		proc(cycle,data);
		break;
	default:
		break;
	}


}

