// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
#include <arpa/inet.h>


void
sk_process_detect_events(sk_cycle_t *cycle)
{
	//body检测
	char str[PARSERULE_SIZE+1] = {0};
	int len, ret;



	memset(str, 0, sizeof(str));
	if(sk_shmtx_trylock(&sk_mempool_mutex)){
		ret = mempool_list_pull(str, &len);
	}

	sk_shmtx_unlock(&sk_mempool_mutex);
	if(ret == 0)
	{
		struct in_addr		cin;
		sk_list_t				*list;

		list = malloc(sizeof(sk_list_t));
		list->sid = cin.s_addr = (((u32_t*)(str))[0]);
		if(list->sid <= 0) return ;
		list->current = str+ 4;
		list->len = len - 4;
		list->current_status = "body";

		fpdetect_search_rule(list);

		if(list->found > 1){

			u8_t * ip;
			ip = malloc(IPADDR_LEN);
			memset(ip,0,IPADDR_LEN);

			memcpy(ip,inet_ntoa(cin),strlen(inet_ntoa(cin)));
			save_msg_log("Vulnerability body packet: source ip: %s, status: %s, content: %s vul num: %d .",
						ip,list->current_status ,\
						list->current,list->found);
			free(ip);
		}
		if(list->found > 5){

				sk_blacklist_event_add(list->sid);

		}
		free(list);

		}


	return ;
}
