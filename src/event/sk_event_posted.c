// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"

sk_event_t 	*sk_posted_accept_events;
sk_event_t		*sk_posted_events;
void
sk_event_process_posted(sk_cycle_t *cycle,sk_event_t **posted)
{
	sk_event_t	*ev;
	for(;;){
		ev = (sk_event_t *)*posted;
		if(ev == NULL) return ;
		sk_delete_posted_event(ev);
		ev->handler(ev);
	}
}
