// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_EVENT_ACCEPT_H_INCLUDE_
#define _SK_EVENT_ACCEPT_H_INCLUDE_
#include "sk_config.h"
#include "sk_core.h"
#define SK_SOCKADDRLEN sizeof(struct sockaddr_in)
void sk_event_accept(sk_event_t *ev);
sk_ptr_t sk_trylock_accept_mutex(sk_cycle_t *cycle);
#endif
