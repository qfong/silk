// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef _SK_SOCKET_H_INCLUDE_
#define _SK_SOCKET_H_INCLUDE_
ssize_t sk_nc_recv(sk_connection_t *c,u_char *buf,size_t size);
ssize_t sk_nc_send(sk_connection_t *c,u_char *buf,size_t size);
#endif
