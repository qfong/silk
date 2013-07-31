// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_CONFIG_H_INCLUDE_
#define _SK_CONFIG_H_INCLUDE_


#include <sys/resource.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <stdio.h>
#include <grp.h>
#include <signal.h>
#include <unistd.h>

#include <errno.h>
#include "sk_define.h"
#include "sk_core.h"

#include "sk_ids_log.h"
#include "sk_tools.h"
#include "sk_mempool_list.h"
#include "sk_mem_pool.h"
#include "sk_rule.h"
#include "mwm.h"
#include "bitop.h"
#include "sk_ids.h"


#include "sk_buf.h"
#include "sk_alloc.h"
#include "sk_ids_request.h"
#include "sk_body_detect.h"
#include "sk_detect.h"


#include "sk_log.h"
#include "sk_array.h"
#include "sk_cycle.h"
#include "sk_palloc.h"

#include "sk_times.h"
#include "sk_rbtree.h"
#include "sk_shmtx.h"
#include "sk_connection.h"
#include "sk_process.h"
#include <sk_process_cycle.h>
#include "sk_event_posted.h"
#include "sk_event_epoll.h"
#include "sk_event_accept.h"
#include "sk_event.h"
#include "sk_event_timer.h"
#include "sk_event_blacklist_timer.h"



sk_ptr_t  sk_ncpu;

#endif
