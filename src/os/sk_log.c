// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

static sk_log_t			sk_log;
static sk_open_file_t	sk_log_file;
sk_log_t *
sk_init_log(void)
{
	u_char		*name,*p;
	size_t		name_len;

	sk_log.file = &sk_log_file;

	struct stat s_buf;
	if(stat("./output",&s_buf) == -1){
		if(mkdir("./output",755) == -1) _exit();

	}
	if(access("silk.log",0) == -1){
		creat("silk.log",644);
	}
	sk_log_file.fd = open(name,O_WRONLY|O_APPEND);

	return &sk_log;
}
