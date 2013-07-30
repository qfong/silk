// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;



#include "sk_config.h"
#include "sk_core.h"


static sk_ptr_t sk_save_argv(sk_cycle_t *cycle, int argc, char *const *argv);
sk_t main(sk_t argc,sk_t *argv[])
{
	sk_log_t				*log;
	sk_pid_t				sk_pid;
	sk_cycle_t				*cycle,init_cycle;

	if (argc) {}
	if (argv) {}

	/*
	 * initiation log record func;
	 */
	log = sk_init_log();

	/*
	 * initiation time stamp;
	 */
	//sk_init_time();
	/*
	 * get current process id; getpid();
	 */
	sk_pid = getpid();

	init_ctx.log = log;
	sk_ctx = &init_ctx;

	init_ctx.pool = sk_create_pool(1024,log);


	FATAL_ERR_CHECK(set_default_conf(sk_ctx),"set defalult conf error");


	FATAL_ERR_CHECK(sk_detect_create(),"create detecte error");

	FATAL_ERR_CHECK(sk_parse_rule_file(CONF_FILE_PATH, 0),"parse rule file error");

	FATAL_ERR_CHECK(sk_detect_pre_rule(),"pre load detect error");
	FATAL_ERR_CHECK(mempool_list_init(),"init memory pool error");

	init_cycle.log = log;
	sk_cycle = &init_cycle;

	init_cycle.pool = sk_create_pool(1024,log);

	if (sk_save_argv(&init_cycle, argc, argv) != SK_OK) {
		return SK_ERR;
	}
	sk_init_setproctitle(log);

	cycle = sk_init_cycle(&init_cycle);


	sk_cycle= cycle;
    
    //sk_init_rules(cycle);

	FATAL_ERR_CHECK (sk_master_process_cycle(cycle),"master process init error");


	while(1){
		sleep(10);
	}



	return SK_OK;
}


static sk_ptr_t
sk_save_argv(sk_cycle_t *cycle, int argc, char *const *argv)
{

    size_t     len;
    sk_ptr_t  i;

    sk_os_argv = (char **) argv;
    sk_argc = argc;

    sk_argv = sk_alloc((argc + 1) * sizeof(char *));
    if (sk_argv == NULL) {
        return SK_ERR;
    }

    for (i = 0; i < argc; i++) {
        len = sk_strlen(argv[i]) + 1;

        sk_argv[i] = sk_alloc(len);
        if (sk_argv[i] == NULL) {
            return SK_ERR;
        }

        (void) sk_cpystrn((u_char *) sk_argv[i], (u_char *) argv[i], len);
    }

    sk_argv[i] = NULL;



    //sk_os_environ = environ;

    return SK_OK;
}

