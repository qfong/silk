// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
#include "cJSON.h"
#include "json_http.h"
#include <arpa/inet.h>
#include <netinet/in.h>

sk_uptr_t count ;

#define HEAD_LEN	8*(sizeof(u8_t))
void sk_response_handler(sk_event_t *wev);

static void
sk_close_request(sk_request_t *r,int rc)
{
	sk_connection_t		*c;
	c = r->connection;
	if(c->fd < 0) return ;
	//sk_free_request(r,rc);
	sk_destroy_pool(r->pool);

	sk_close_connection(c);

	return ;


}
void
sk_blacklist_event_add(sk_uptr_t netip)
{
	sk_event_blacklist_t *evb;
	evb= malloc(sizeof(sk_event_blacklist_t));
	memset(evb,0,sizeof(sk_event_blacklist_t));

	sk_event_add_blacklist_timer(evb,5000);
	sk_event_add_blacklist(evb,netip);
	return ;

}
sk_ptr_t
sk_blacklist_event_find(sk_uptr_t netip)
{

	sk_event_blacklist_t *ebl;

	ebl = sk_event_blacklist_find (netip);
	if(ebl != NULL){
		return SK_OK;
	}
	return SK_ERR;
}



sk_detect_header(sk_request_t *r)
{
	sk_connection_t		*c;
	sk_event_t				*rev,*wev;
	sk_header_detect_t	*detect;

	sk_list_t				*list_url,*list_cookie,*list;
	c = r->connection;;

	wev = c->write;




	detect = sk_palloc(r->pool,sizeof(sk_header_detect_t));
	if(r->http->http_line->url == NULL) return SK_ERR;
	list_url = sk_palloc(r->pool,sizeof(sk_list_t));
	list_cookie = sk_palloc(r->pool,sizeof(sk_list_t));

	detect->handler = fpdetect_search_rule;

	list_url->current =r->http->http_line->url;
	list_url->len = strlen(list_url->current);
	list_url->current_status = "url";
	list_url->next = NULL;

	detect->content = list_url;

	if(r->http->http_header->cookie != NULL){
	list_cookie->current =r->http->http_header->cookie;
	list_cookie->len = strlen(list_cookie->current);
	list_cookie->current_status = "cookie";
	list_url->next = list_cookie;


	list_cookie->next = NULL;
	}

	list = list_url;
	for(;;){
		if(list != NULL){

			detect->handler(list);


			if(list->found > 1){
			u8_t	*ip;
			ip = malloc(IPADDR_LEN);
			sk_memzero(ip,IPADDR_LEN);
			memcpy(ip,r->ip,strlen(r->ip));
			save_msg_log("Vulnerability packet, source ip: %s, host: %s, status: %s, content: %s vul num: %d .",
					ip,r->http->http_header->host,	list->current_status ,\
					list->current,list->found);
			free(ip);
			}

			if(list_url &&list->current_status == "url" && list->found >2 ){

				sk_blacklist_event_add(r->netip);
				return SK_ERR;
			}
			if(list &&list->current_status == "cookie" && list->found >3 ){

				sk_blacklist_event_add(r->netip);

				return SK_ERR;
			}
			list = list->next;

		}else {
			//pull body to mem pool
			if(r->http->http_header->content_len <= 0) return SK_OK;

			char *mp;
			int len  = 4 +r->http->http_header->content_len;

			mp = malloc(len);
			sk_memzero(mp,len);

			(((u32_t*)(mp))[0]) = r->netip;

			memcpy(mp+4,r->http->http_body->pos,r->http->http_header->content_len);


			for(;;){
				if(sk_shmtx_trylock(&sk_mempool_mutex)){

					mempool_list_push(mp,r->http->http_header->content_len);
					sk_shmtx_unlock(&sk_mempool_mutex);

					return SK_OK;
				}
			}
			free (mp);

			break;
		}

	}


}

sk_recv_event(sk_event_t *rev, int flags) {

	size_t n, size;
	sk_connection_t *c;
	sk_request_t *r;
	sk_buf_t *b;
	c = rev->data;
	r = c->data;
	if (flags == 1) {
		b = r->header;
	} else {
		b = r->data;
	}

	size = b->end - b->last;
	if (size < 0)
		return SK_ERR;

	n = b->last - b->pos;

	if (rev->ready) {
		n = sk_nc_recv(c, b->last, size);

	} else {
		n = SK_AGAIN;
	}

	if (n == SK_AGAIN) {
		if (!rev->timer_set) {
			sk_event_add_timer(rev, 500);
		}
		if (sk_handle_read_event(rev, 0) != SK_OK) {
			sk_close_connection(c);
			return SK_ERR;
		}

		return SK_AGAIN;
	}
	if (n == 0 || n == SK_ERR) {
		sk_close_connection(c);
		//free
		return SK_OK;
	}

	b->last += n;

	return n;

}


sk_ptr_t
sk_parse_data(sk_request_t *r){
	if(r == NULL)  return SK_ERR ;

	/*
	 * include json_format  library function for parse data;
	 */

	cJSON *cjson_http = cJSON_Parse(r->data->pos);
	r->http =sk_palloc(r->pool,sizeof(sk_http_t));
	r->http->http_header =sk_palloc(r->pool,sizeof(sk_http_header_t));
	r->http->http_line =sk_palloc(r->pool,sizeof(sk_http_line_t));

	if(get_http_data_from_json(cjson_http, r) == SK_ERR)
		return SK_ERR;


#if 0
	printf("curl, %s\n",r->http->http_line->url);
	printf("method, %s\n",r->http->http_line->method);
	printf("version, %s\n",r->http->http_line->version);
	printf("accept, %s\n",r->http->http_header->accept);
	printf("accept_language, %s\n",r->http->http_header->accept_language);
	printf("cache_control, %s\n",r->http->http_header->cache_control);
	printf("conn_status, %s\n",r->http->http_header->conn_status);
	printf("len, %d\n",r->http->http_header->content_len);
	printf("cookie, %s\n",r->http->http_header->cookie);
	printf("host, %s\n",r->http->http_header->host);
	printf("referer, %s\n",r->http->http_header->referer);
	printf("user_agent, %s\n",r->http->http_header->user_agent);
	if(r->http->http_header->content_len > 0 )	printf("body, %s\n",r->http->http_body->pos);
	//r->http_line =  sk_create_temp_buf(r->pool, strlen(http_line));
//	sk_memcpy(r->http_line,http_line);
#endif

	return SK_OK;
}

sk_ptr_t
sk_parse_header(sk_request_t *r) {
	struct in_addr		sin;

	if (!r->header) return SK_ERR;
	if (HEAD_ALLOC_M(r->header->pos) != 0x49 ) return SK_ERR;

	r->len = HEAD_ALLOC_L(r->header->pos);
	if(r->len < 0 ) return SK_ERR;

	sin.s_addr = r->netip=(HEAD_ALLOC_IP(r->header->pos));
	r->ip = sk_palloc(r->pool,IPADDR_LEN);

	memcpy(r->ip,inet_ntoa(sin),strlen(inet_ntoa(sin)));
	return SK_OK;

}

void sk_empty_handler(sk_event_t *ev) {
	_exit();
}
void sk_init_request(sk_event_t *rev) {

	sk_connection_t 		*c;
	sk_pool_t 				*pool;
	sk_event_t 			*wev;
	sk_request_t 			*r;
	int 					flags = 0;
	c = rev->data;
	c->request++;
	wev = c->write;

	pool = sk_create_pool(4096, sk_cycle->log);
	r = sk_palloc(pool, sizeof(sk_request_t));
	r->pool = pool;

	c->data = r;
	r->connection = c;
	c->send = 0;
	r->ready = 0;

	r->header = sk_create_temp_buf(r->pool, HEAD_LEN);
	if(r->header == NULL){
		save_err_log("create temp buf header error") ;
	}
	int rc, n;

	rc = SK_AGAIN;
	for (;;) {
		if (rc == SK_AGAIN) {

			n = sk_recv_event(rev, 1);

			if (n == SK_AGAIN || n == SK_ERR) {
				return;
			}

		}

		n = sk_parse_header(r);
		if (n == SK_ERR) return ;
		r->data = sk_create_temp_buf(r->pool,r->len);
		n = sk_recv_event(rev,2);
		if(n == r->len){
			//检测ip是否在链表内部

			if(sk_blacklist_event_find(r->netip) == SK_OK){
				flags = SK_ERR;

				break;
				//save_msg_log("find ip address %s in blacklist;")
			}
		}

		if (n != r->len) {
			return ;
		}


		n = sk_parse_data(r);
		if( n == SK_ERR )goto failed;

		flags = sk_detect_header(r);


		break;
	}

	r->response = sk_create_temp_buf(r->pool,8);

	if (flags == SK_ERR){
		SK_RT_MSG_ERR(r->response->pos) = r->netip;

	}else{
		SK_RT_MSG_OK(r->response->pos) = r->netip;
	}



	r->response->last += 8;

	wev->handler = sk_response_handler;
	/*
	printf("i am here ...>>> %d,%d,%d\n",(((u16_t*)(r->response->pos))[0]),
			(((u16_t*)(r->response->pos))[1]),
			(((u32_t*)(r->response->pos))[1]));
	*/

	sk_event_add_timer(wev,500);

	if(sk_epoll_add_event(wev, SK_WRITE_EVENT, SK_CLEAR_EVENT)!= SK_OK){
		sk_close_request(r,50);
		return ;
	}

	return ;
failed:
	sk_close_request(r,50);
}
void sk_init_connection(sk_connection_t *c) {
	sk_event_t *rev;

	rev = c->read;
	rev->log = c->log;

	if (rev->timeout) {
		c->timeout = 1;
		sk_close_connection(c);
		return;
	}

	rev->handler = sk_init_request;
	c->write->handler = sk_empty_handler;

	sk_event_add_timer(rev, 500);

	if (sk_handle_read_event(rev, 0) != SK_OK) {
		sk_close_connection(c);
		if (rev->timer_set) {
			sk_event_del_timer(rev);
		}
	}
	//sk_close_connection(c);
}
void
sk_response_handler(sk_event_t *wev)
{

	sk_connection_t 			*c;
	sk_request_t				*r;
	sk_event_t					*rev;
	sk_buf_t					*b;
	ssize_t					n,size,fgs;

	c = wev->data;
	r = c->data;
	if(r == NULL){
		sk_close_request(r,50);
		return ;
	}
	if(wev->timeout){
		sk_close_request(r,50);
		return ;
	}

	b = r->response;
	if(r->response == NULL) {

		sk_close_request(r,50);
		return ;
	}
	size = b->last - b->pos ;

	for(;;){

		n = sk_nc_send(c,b->pos,size);
		if(n == SK_AGAIN){
			wev ->handler = sk_response_handler;
			if(sk_handle_write_event(wev,0) != SK_OK){

				sk_close_request(r,50);
				return ;
			}
		}

		if (n == SK_ERR || n == 0){

			//sk_close_connection(c);
			sk_close_request(r,50);
			return ;
		}

		if( n > 0 ){
				b->pos += n;
				if(n == size){

					wev->handler = sk_empty_handler;


						sk_close_request(r,50);
						return;
					}
				}
	}

	return ;
}
