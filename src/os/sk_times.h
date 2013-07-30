// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#ifndef _SK_TIMES_H_INCLUDE_
#define _SK_TIMES_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
void sk_time_update(void);
#define sk_gettimeofday(tp) (void)gettimeofday(tp,NULL);
typedef sk_uptr_t		sk_msec_t;
#endif
