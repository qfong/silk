// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#ifndef _SK_IDS_H_INCLUDE_
#define _SK_IDS_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
#include "sk_ids_log.h"
#include "sk_tools.h"
#include "sk_mempool_list.h"
#include "sk_mem_pool.h"
#include "sk_rule.h"
#include "mwm.h"
#include "bitop.h"

#define FILE_LINE_LEN 	2048
#define PARSERULE_SIZE  8129
#define FILE_PATH_LEN 	1024
#define RULES_FILE_DIR 	"/var/local/silk/rules"
#define CONF_FILE_PATH 	"/var/local/silk/silk.conf"

#define STD_LEN 				512
#define DEFAULT_BUF_LEN 		1024

#define NIDS_SID_LEN    		10
#define NIDS_SEND_LEN   		512

 enum INCLEVEL
 {
     MAIN_CONF = 0,
     RULE_CONF = 1,
 };

typedef enum RULE_TYPE
{
     SK_RULE_VAR = 0x01,
     SK_RULE_INCLUDE = 0x02,
     SK_RULE_CONTENT = 0x03,
}RULE_TYPE;




typedef struct{
	sk_pool_t			*pool;
	sk_log_t			*log;
    //server
	sk_t 				port;
	sk_t				process;//fork process number;
	sk_t 				max_listen_num;
	//mempool
	sk_t 				mempool_num;
	sk_t 				mempool_size;

	//nids
	sk_t 				ids_addr[20];
	sk_t 				ids_port;

	//log
	sk_t 				log_dir[DEFAULT_BUF_LEN];
	sk_t 				err_log[DEFAULT_BUF_LEN];
	sk_t 				detect_log[DEFAULT_BUF_LEN];

	//rules
	char 				rules_file_dir[DEFAULT_BUF_LEN];
}sk_ctx_t;


sk_ctx_t	*sk_ctx,init_ctx;

sk_ptr_t set_default_conf(sk_ctx_t *sk_ctx);
sk_ptr_t sk_parse_rule(FILE* thefp, char* prule, sk_t inclevel);
sk_ptr_t sk_parse_rule_file(char* file, sk_t inclevel);
#endif
