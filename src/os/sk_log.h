// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_LOG_H_INCLUDE_
#define _SK_LOG_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"

struct sk_open_file_s{
	sk_fd_t				fd;
	sk_str_t				name;
	u_char					*buf;
};

struct sk_log_s{
	sk_open_file_t			*file;
	void 						*data;
};


sk_log_t * sk_init_log(void);
#endif
