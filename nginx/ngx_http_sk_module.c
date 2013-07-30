#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include "cJSON.h"

#define MAX_TRY					10
#define DEFAULT_SERVER 			"127.0.0.1"
#define SK_HEAD_LEN 				8
#define DEFAULT_PORT 			5555
#define DEFAULT_TIMEOUT 			1000
#define MAX_TIMEOUT 				5000
#define HEAD_ALLOC_M(_ptr)		(((uint8_t*)(_ptr))[0])
#define HEAD_ALLOC_L(_ptr)		(((uint16_t*)(_ptr))[1])
#define HEAD_ALLOC_IP(_ptr)		(((uint32_t*)(_ptr))[1])

typedef struct {
	ngx_flag_t   skids;					/* on: turn on skids; off: turn off skids. */
	ngx_str_t    skids_server_ip;  		/* server ip address */
	ngx_uint_t   skids_server_port; 	/* server TCP port */
	ngx_uint_t   skids_timeout;			/* timeout skonds for IDS check */
}ngx_http_sk_main_conf_t;

typedef struct {
	ngx_http_request_t	*r;
	ngx_connection_t		*connection;
	ngx_pool_t				*pool;
	ngx_buf_t				*sndbuf;
	ngx_buf_t				*rcvbuf;
	ngx_uint_t				len;
}ngx_sk_data_t;

static void 		ngx_sk_request_event(ngx_event_t *);
static void 		ngx_sk_get_result(ngx_event_t *);
static int 		ngx_sk_connect_server(ngx_http_request_t *, const char *, int);
static ngx_int_t 	ngx_sk_get_request_data(ngx_http_request_t *r,ngx_sk_data_t *skbuf);
static ngx_int_t 	ngx_http_sk_init(ngx_conf_t *cf);
static void 		*ngx_http_sk_create_main_conf(ngx_conf_t *cf);
static char 		*ngx_http_skids(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char 		*ngx_http_skids_timeout(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char 		*ngx_http_skids_server(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);




int skids_timeout;

static ngx_command_t  ngx_http_sk_commands[] = {
    {
      ngx_string("skids"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_FLAG,
      ngx_http_skids,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_sk_main_conf_t, skids),
      NULL },

    { ngx_string("skids_server"),
      NGX_HTTP_MAIN_CONF | NGX_CONF_TAKE12,
      ngx_http_skids_server,
      NGX_HTTP_MAIN_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("skids_timeout"),
      NGX_HTTP_MAIN_CONF | NGX_CONF_TAKE1,
      ngx_http_skids_timeout,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_sk_main_conf_t, skids_timeout),
      NULL },

      ngx_null_command
};

static ngx_http_module_t  ngx_http_sk_module_ctx = {
	NULL,                                 	/* preconfiguration */
	ngx_http_sk_init,                     	/* postconfiguration */
	ngx_http_sk_create_main_conf,        	/* create main configuration */
	NULL,                                 	/* init main configuration */
	NULL,                                 	/* create server configuration */
	NULL,                                 	/* merge server configuration */
	NULL, 							        	/* create location configuration */
	NULL            					  		/* merge location configuration */
};

ngx_module_t  ngx_http_sk_module = {
	NGX_MODULE_V1,
	&ngx_http_sk_module_ctx,   				/* module context */
	ngx_http_sk_commands,      				/* module directives */
	NGX_HTTP_MODULE,                       /* module type */
	NULL,                                  /* init master */
	NULL,                                  /* init module */
	NULL,                                  /* init process */
	NULL,                                  /* init thread */
	NULL,                                  /* exit thread */
	NULL,                                  /* exit process */
	NULL,                                  /* exit master */
	NGX_MODULE_V1_PADDING
};

void
ngx_sk_close_request(ngx_sk_data_t *skbuf){
	ngx_connection_t		*c;
	c = skbuf->connection;
	if(c->fd < 0) return ;
	//sk_free_request(r,rc);
	ngx_destroy_pool(skbuf->pool);
	ngx_close_connection(c);
	return ;
}

void ngx_sk_empty_handler(ngx_event_t *ev)
{
	return ;
}

/*
 * Create Socket and Connect to skids server.
 *
 * Return Socket fd when sucess.
 * Return -1 when failed.
 *
 */
static int
ngx_sk_connect_server(ngx_http_request_t *r, const char *ip, int port)
{
	uint32_t 					s_addr;
	struct sockaddr_in 		sockaddr;
	ngx_connection_t 			*c;
	ngx_event_t 				*wev,*rev;
	ngx_fd_t					sk_fd;
	ngx_pool_t					*pool;
	ngx_sk_data_t				*skbuf;
	ngx_int_t					reuseaddr;
	reuseaddr = 1 ;

	ngx_log_t *log = r->connection->log;


    ngx_memzero(&sockaddr,sizeof(struct sockaddr_in ));
    inet_pton(AF_INET, ip, &s_addr);

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = s_addr;
    sockaddr.sin_port = htons(port);

    if ((sk_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ngx_log_error(NGX_LOG_EMERG, log, 0,
        		"sk_handler(): cannot get socket fd");
        return -1;
    }
	if(setsockopt(sk_fd,SOL_SOCKET,SO_REUSEADDR,
			(const void *)&reuseaddr,sizeof(int))< 0){
		   ngx_log_error(NGX_LOG_EMERG, log, 0,
				   "setsockopt,SO_REUSEADDR,warning,");
	}

    if(connect(sk_fd, (struct sockaddr*)&sockaddr,
    		sizeof(struct sockaddr_in ))) {
        ngx_log_error(NGX_LOG_EMERG, log, 0,
        		"sk_handler(): cannot connect server");
        return -2;
    }

    ngx_nonblocking(sk_fd);    /* Non-blocking mode */

    c = ngx_get_connection(sk_fd, log);

    if (c == NULL) {
        ngx_log_error(NGX_LOG_EMERG, log, 0,
        		"sk_handler(): skids no connection");
        return -3;
    }

	ngx_log_error(NGX_LOG_EMERG, log, 0, "sk_handler(): get_connection() successfully");
    c->log = log;


    pool = ngx_create_pool(4096,log);

    if( pool == NULL){
    	ngx_log_error(NGX_LOG_EMERG, log, 0,
    	    					"ngx_create_pool() error");

    	ngx_close_connection(c);
    }

    skbuf = ngx_palloc(pool,sizeof(ngx_sk_data_t));
    skbuf->pool = pool;
    /* get request data_string and bind it to connection struct */
    skbuf->r = r;   /* ngx_http_request_t struct pointer */
   // ngx_str_set(&(sd.data), ngx_sk_get_request_data(r));
    c->data = skbuf;
    skbuf->connection = c;

    if(ngx_sk_get_request_data(r,skbuf) == NGX_ERROR){
    	ngx_log_error(NGX_LOG_EMERG, log, 0,
    					"sngx_sk_get_request_data");
    }


	/* Add write event to send query data */

	rev = c->read;
	wev = c->write;
    wev->log = log;
    wev->write = 1;
    wev->handler = ngx_sk_request_event;

	rev->log = c->log;
	rev->handler = ngx_sk_get_result;

    /* Add read event to get query result from skids server */
    if (ngx_add_event(rev, NGX_READ_EVENT, 0) == NGX_ERROR) {
		ngx_log_error(NGX_LOG_EMERG, c->log, 0, "sk_handler(): ERROR add_event read");
	}

    ngx_log_error(NGX_LOG_EMERG, log, 0,
				"sk_handler(): try to add write event");
    if (ngx_add_event(wev, NGX_WRITE_EVENT, 0) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_EMERG, log, 0, "sk_handler(): ERROR add_event");
        return -4;
    }

	/* Add a timer for this write event */
    ngx_add_timer(rev, skids_timeout);
    ngx_add_timer(wev, skids_timeout);    /* set timeout 10 skonds */


   // ngx_sk_request_event(wev);

    return sk_fd;
}


/*
 * Send Client Http Request to skids server for skurity check.
 */
static void
ngx_sk_request_event(ngx_event_t *wev)
{
	ngx_connection_t  		*c;
	ngx_http_request_t 		*r;
	ngx_sk_data_t 			*skbuf;
	ngx_str_t 					uri, args;
	ngx_int_t					n,size;
	ngx_buf_t					*b;

	/* Get ngx_http_request from event data element */
	c = wev->data;

	skbuf = (ngx_sk_data_t *)(c->data);

	r = (ngx_http_request_t *) (skbuf->r);

    ngx_log_error(NGX_LOG_EMERG, c->log, 0, "ngx_sk_send_query() called.");

    if (wev->timedout) { /* Check if this event is timeout */
#if 1
    	ngx_log_error(NGX_LOG_EMERG, c->log, 0,
					"ngx_sk_send_query(): timeout.");
#endif
		ngx_del_event(c->write, NGX_WRITE_EVENT, 0);
		ngx_str_set(&uri, "/timeout.html");
		ngx_str_null(&args);
		ngx_http_internal_redirect(r, &uri, &args);
		ngx_sk_close_request(skbuf);

		return;
    }

	/* Try to send data to skids server */


	b = skbuf->sndbuf;

	size = b->last - b->pos ;

	for(;;){
		n = ngx_unix_send(c,b->pos,size);

		if(n == NGX_AGAIN){
			wev ->handler = ngx_sk_request_event;
			if(ngx_handle_write_event(wev,0) != NGX_OK){

				ngx_sk_close_request(skbuf);
				return ;
			}
		}

		if (n == NGX_ERROR || n == 0){
			ngx_sk_close_request(skbuf);
			return ;
		}

		if( n > 0 ){
				b->pos += n;
				if(n == size){
					wev->handler = ngx_sk_empty_handler;
					if(wev->timer_set){
						ngx_event_del_timer(wev);
					}
					//ngx_sk_close_request(skbuf);
					return;
				}
		}
	}

}

ngx_int_t
ngx_recv_sk_event(ngx_event_t *rev)
{
	ngx_int_t				n,size;
	ngx_buf_t				*b;
	ngx_connection_t 		*c;
	ngx_sk_data_t			*skbuf;
	c = rev->data;
	skbuf = c->data;
	b = skbuf->rcvbuf;
	size = b->end - b->last;
	if (size < 0)
		return  NGX_ERROR;

	n = b->last - b->pos;

	if (rev->ready) {
		n = ngx_unix_recv(c, b->last, size);

	} else {
		n = NGX_AGAIN;
	}

	if (n == NGX_AGAIN) {
		if (!rev->timer_set) {
			ngx_event_add_timer(rev, 500);
		}
		if (ngx_handle_read_event(rev, 0) != NGX_OK) {
			ngx_close_connection(c);
			return NGX_ERROR;
		}

		return NGX_AGAIN;
	}
	if (n == 0 || n == NGX_ERROR) {
		ngx_close_connection(c);
		//free
		return NGX_ERROR;
	}

	b->last += n;

	return n;
}

/*
 * Receive query result from skids server
 */
static void
ngx_sk_get_result(ngx_event_t *rev)
{
	ngx_str_t 				uri, args;
	ngx_connection_t 		*c;
	ngx_http_request_t 	*r;
	ngx_sk_data_t			*skbuf;

	c = rev->data;
	skbuf = (ngx_sk_data_t *) (c->data);
	r = (ngx_http_request_t *) (skbuf->r);

    if (rev->timedout) { /* It's already timeout */
		ngx_log_error(NGX_LOG_EMERG, c->log, 0,
				"ngx_sk_get_result event time out");
		ngx_sk_close_request(skbuf);
		ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
		return;
	}

    int rc,n;
    rc = NGX_AGAIN;

    for(;;){
    		if (rc == NGX_AGAIN){

    			n = ngx_recv_sk_event(rev);

    			if (n == NGX_AGAIN || n == NGX_ERROR) {
    				return;
    			}
    		}

    		if(HEAD_ALLOC_M(skbuf->rcvbuf->pos) != 0x49){
    			   ngx_log_error(NGX_LOG_EMERG, c->log, 0, "recv_data: buf[0] != 0x49");
    		}
    		rc = NGX_OK;
    		if (HEAD_ALLOC_L(skbuf->rcvbuf->pos) & 0x01){
    			ngx_log_error(NGX_LOG_EMERG, c->log, 0, "get_result(): pass");
    		    ngx_http_core_run_phases(r);

    		    break;

			} else {
				ngx_log_error(NGX_LOG_EMERG, c->log, 0, "get_result(): fail");
				ngx_str_set(&uri, "/skids_fail.html");
				ngx_str_null(&args);
				ngx_http_internal_redirect(r, &uri, &args);
				ngx_http_finalize_request(r, NGX_HTTP_FORBIDDEN);
				break;
    			}

    }

    ngx_sk_close_request(skbuf);

    return ;


}



/*
 * Get Http request header and Http request body from ngx_http_request struct.
 * Return as JSON format string
 */
static ngx_int_t
ngx_sk_get_request_data(ngx_http_request_t *r,ngx_sk_data_t *skbuf)
{
    char 					*req_json_str;
    char 					buf[INET_ADDRSTRLEN];
    const 					char *addr_str;
    ngx_int_t				sk_len;

    cJSON *http_request = cJSON_CreateObject();
    cJSON *request_header, *request_body;

    ngx_uint_t      j,k;
    ngx_table_elt_t **cookie=NULL;

    ngx_list_part_t *part = &r->headers_in.headers.part;
    ngx_table_elt_t *header = part->elts;

    /* source_id */
    addr_str = inet_ntop(AF_INET, &(((struct sockaddr_in*)
    		(r->connection->sockaddr))->sin_addr), buf, INET_ADDRSTRLEN);
    cJSON_AddStringToObject(http_request, "source_ip", addr_str);

    /* http_request_line */
    cJSON_AddStringToObject(http_request, "http_line",
    		(const char*)((r->request_line).data));
    /* http_request_header */
    cJSON_AddItemToObject(http_request, "http_header",
    		request_header=cJSON_CreateObject());
    cJSON_AddStringToObject(request_header, "user-agent",
    		(const char *)(((r->headers_in.user_agent)->value).data));
    cJSON_AddStringToObject(request_header, "host",
    		(const char *)(((r->headers_in.host)->value).data));

    if ((r->headers_in.referer) == NULL) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                "sk_handler(): referer is NULL");
        cJSON_AddStringToObject(request_header, "referer", "null");
    } else {
        cJSON_AddStringToObject(request_header, "referer",
        		(const char *)(((r->headers_in.referer)->value).data));
    }


    if ((r->headers_in.content_length_n) < 0) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                "sk_handler(): content-length is -1");
        cJSON_AddNumberToObject(request_header, "content-length", -1);
    } else {
		 cJSON_AddNumberToObject(request_header, "content-length",
				 r->headers_in.content_length_n);
    }

    if ((r->headers_in.content_type) == NULL) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                "sk_handler(): content-type is NULL");
        cJSON_AddStringToObject(request_header, "content-type", "null");
    } else {
		 cJSON_AddStringToObject(request_header, "content-type",
				 (const char *)(((r->headers_in.content_type)->value).data));
    }

	cookie = r->headers_in.cookies.elts;
    if (cookie == NULL){
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                "sk_handler():cookies is NULL");
        cJSON_AddStringToObject(request_header, "cookie", "null");
    } else {
        if (r->headers_in.cookies.nelts == 0 ){
            cJSON_AddStringToObject(request_header, "cookie", "null");
        } else {
            for(j=0; j < r->headers_in.cookies.nelts; j++){
                cJSON_AddStringToObject(request_header, "cookie",
                		(const char*)(cookie[j]->value.data));
            }
        }
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "This is test04");
    }
    char *connection = NULL;
    char *accept = NULL;
    char *accept_language = NULL;
    char *cache_control = NULL;

    for (k=0; /* void */; k++){
        if (k >= part->nelts) {
            if (part->next == NULL) {
                break;
            }
            part = part->next;
            header = part->elts;
            k = 0;
        }
        if (header[k].hash == 0) {
            continue;
        }
        if (ngx_strncasecmp(header[k].key.data, (u_char*)"Connection",
        		header[k].key.len) == 0) {
            connection = (char*)(header[k].value.data);
        }
        if (ngx_strcasecmp(header[k].key.data, (u_char*)"Accept") == 0) {
            accept = (char*)(header[k].value.data);
        }
        if (ngx_strcasecmp(header[k].key.data, (u_char*)"Accept-Language") == 0) {
            accept_language = (char*)(header[k].value.data);
        }
        if (ngx_strncasecmp(header[k].key.data, (u_char*)"Cache-Control",
        		header[k].key.len) == 0) {
            cache_control = (char*)(header[k].value.data);
        }
    }
    if ( connection == NULL ){
        cJSON_AddStringToObject(request_header, "connection", "null");;
    } else {
        cJSON_AddStringToObject(request_header, "connection", connection);
    }
    if (accept == NULL){
        cJSON_AddStringToObject(request_header, "accept", "null");;
    } else {
        cJSON_AddStringToObject(request_header, "accept", accept);
    }
    if ( accept_language == NULL){
        cJSON_AddStringToObject(request_header, "accept-language", "null");
    } else {
        cJSON_AddStringToObject(request_header, "accept-language", accept_language);
    }
    if ( cache_control == NULL){
        cJSON_AddStringToObject(request_header, "cache-control", "null");
    }else{
        cJSON_AddStringToObject(request_header, "cache-control", cache_control);
    }
	// http_body
    cJSON_AddItemToObject(http_request, "http_body",
    		request_body=cJSON_CreateString((const char *)r->header_in->pos));

    req_json_str = cJSON_Print(http_request);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
            "ngx_sk_get_request_data(): http_request, %s.", req_json_str);


    sk_len = strlen(req_json_str);
    if (sk_len < 0) return NGX_ERROR;
    skbuf->sndbuf = ngx_create_temp_buf(r->pool,sk_len+SK_HEAD_LEN);
    skbuf->rcvbuf = ngx_create_temp_buf(r->pool,SK_HEAD_LEN);
    if(skbuf->sndbuf == NULL)return NGX_ERROR;
    if(skbuf->rcvbuf == NULL)return NGX_ERROR;

    HEAD_ALLOC_M(skbuf->sndbuf->pos) = 0x49;
    HEAD_ALLOC_L(skbuf->sndbuf->pos) = sk_len;

    HEAD_ALLOC_IP(skbuf->sndbuf->pos) = ((struct sockaddr_in*)
    		(r->connection->sockaddr))->sin_addr.s_addr;
    skbuf->sndbuf->last +=SK_HEAD_LEN;
    memcpy(skbuf->sndbuf->pos+SK_HEAD_LEN,req_json_str,sk_len);
    skbuf->sndbuf->last += sk_len;
    return NGX_OK;

    //return req_json_str;
}


/*
 * Handler method for skurity check
 */
static ngx_int_t
ngx_http_sk_handler(ngx_http_request_t *r)
{
	ngx_http_sk_main_conf_t *sk_conf;
	ngx_fd_t					sock_fd, i;

    sk_conf = ngx_http_get_module_main_conf(r, ngx_http_sk_module);

    if (!sk_conf->skids) { 			/* Check if this module is off */
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
            "ngx_http_sk_handler(): skids is off.");
		return NGX_DECLINED;
    }

	/* internal redirect */

	if (r->internal) {
		ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
			"ngx_http_sk_handler(): internal request.");
		return NGX_DECLINED;
	}


    /* connect skids server */
    i = 0;
    sock_fd = -1;
    while(sock_fd < 0) {
		ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                      "ngx_http_sk_handler(): connect server: %s[%d]",
                      sk_conf->skids_server_ip.data, sk_conf->skids_server_port);

        sock_fd = ngx_sk_connect_server(r, (char *)(sk_conf->skids_server_ip.data),
        		sk_conf->skids_server_port);

        if (i >= MAX_TRY) { /* Fail to connect server after max times */
		    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                      "ngx_http_sk_handler(): fail to connect server");
            return NGX_HTTP_SERVICE_UNAVAILABLE;   /* 503 */
        }
        i++;

    }
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0,
                  "server_fd: %d", sock_fd);

	r->phase_handler++;   /* Move on phase handler to next */

    return NGX_DONE;
}


/*
 * Initialize this module:
 *   1. Check the commands.
 *   2. Set the phase handler.
 */
static ngx_int_t
ngx_http_sk_init(ngx_conf_t *cf)
{
	ngx_http_handler_pt        	*h;
	ngx_http_core_main_conf_t  	*cmcf;
	ngx_http_sk_main_conf_t 		*sk_conf =
			ngx_http_conf_get_module_main_conf(cf, ngx_http_sk_module);

	if (sk_conf->skids == NGX_CONF_UNSET || !(sk_conf->skids)) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "ngx_http_sk_init(): skids off.");
		return NGX_OK;
	}

    if (!((sk_conf->skids_server_ip).data)) {
        ngx_str_set(&(sk_conf->skids_server_ip), DEFAULT_SERVER);
		sk_conf->skids_server_port = DEFAULT_PORT;
	}
	if (sk_conf->skids_server_port == NGX_CONF_UNSET_SIZE) {
		sk_conf->skids_server_port = DEFAULT_PORT;
	}
    if (sk_conf->skids_timeout == NGX_CONF_UNSET_SIZE) {
		skids_timeout = DEFAULT_TIMEOUT;
    } else {
		skids_timeout = MAX_TIMEOUT > sk_conf->skids_timeout ?
				sk_conf->skids_timeout : MAX_TIMEOUT;
	}
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
			"ngx_sk_init(): skids_timeout = %d", skids_timeout);

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

	/* Put this phase hander at NGX_HTTP_SERVER_REWRITE_PHASE */
    h = ngx_array_push(&cmcf->phases[NGX_HTTP_SERVER_REWRITE_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_sk_handler;    /* Set the handler */

	return NGX_OK;
}


/*
 * Called to create self-defined sk_conf struct when nginx reading conf file.
 */
static void *
ngx_http_sk_create_main_conf(ngx_conf_t *cf)
{
    ngx_http_sk_main_conf_t *sk_conf = NULL;
    sk_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_sk_main_conf_t));
    if (sk_conf == NULL)
    {
         return NULL;
    }
	ngx_str_null(&sk_conf->skids_server_ip);
	sk_conf->skids_server_port = NGX_CONF_UNSET_SIZE;
	sk_conf->skids = NGX_CONF_UNSET;
	sk_conf->skids_timeout = NGX_CONF_UNSET_SIZE;
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "ngx_http_sk_create_main_conf() called.");
	return sk_conf;
}


/*
 * Parse method for 'skids' command in nginx conf file.
 */
static char *
ngx_http_skids(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	char* rv = NULL;

	rv = ngx_conf_set_flag_slot(cf, cmd, conf);
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
		   "ngx_http_skids(): skids=%d", ((ngx_http_sk_main_conf_t *)conf)->skids);
	return rv;
	}


/*
 * Parse method for 'skids_timeout' command in nginx conf file.
 */
static char *
ngx_http_skids_timeout(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	char *rv = NULL;

	rv = ngx_conf_set_size_slot(cf, cmd, conf);
	ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
			"ngx_http_skids_timeout: %d", ((ngx_http_sk_main_conf_t *)conf)->skids);
	return rv;
}


/*
 * Parse method for 'skids_server' command in nginx conf file.
 */
static char *
ngx_http_skids_server(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_str_t						*value;
    ngx_http_sk_main_conf_t		*sk_conf = conf;

    if ((sk_conf->skids_server_ip).data) {
		return "is duplicate";
	}

    value = cf->args->elts;

    sk_conf->skids_server_ip = value[1];

    if (sk_conf->skids_server_port != NGX_CONF_UNSET_SIZE) {
        return "is duplicate";
    }

    if (cf->args->nelts < 3) {
		sk_conf->skids_server_port = DEFAULT_PORT;
	} else {
	    sk_conf->skids_server_port = ngx_parse_size(&value[2]);
    	if (sk_conf->skids_server_port == (size_t) NGX_ERROR) {
        	return "invalid value";
	    }
	}

    return NGX_CONF_OK;
}
