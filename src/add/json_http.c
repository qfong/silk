/**
 * Copyright (c) 2012, China Standard Software CO., LTD.
 * All rights reserved
 * @file json_http.c
 * @the project of json_http module  
 *  
 * @author ying.wang <yingying.wang@cs2c.com.cn>
 * @version 1.0
 * @date 2013-04-22
 * 
 * Last Version: 1.0
 * Author: ying.wang <yingying.wang@cs2c.com.cn>
 * Completion Date: 2013-04-22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "json_http.h"

#include "sk_tools.h"
#include "sk_config.h"
#include "sk_core.h"

/**
 * @desc 将json数据格式字符串转换为http结构体数据
 * @param char * str_http_data 需要转换的json数据格式字符串
 * @param nids_http nidshttp nids_http结构体指针
*/
sk_ptr_t
get_http_data_from_json(cJSON *cjson_http, sk_request_t *r)
{
	sk_http_t			*http;
	http = r->http;
	if(NULL == http || NULL == cjson_http)
	{
		return SK_ERR;
	}
	cJSON *cjson_http_header;
	char * str_http_line, * str_http_line_method, * str_http_line_url, * str_http_line_version;

	//解析json格式数据
	//cjson_http = cJSON_Parse(str_http_data);

	cjson_http_header = cJSON_GetObjectItem(cjson_http, HTTP_HEADER);

	//源IP地址
	http->source_ip = cJSON_GetObjectItem(cjson_http, SOURCE_IP)->valuestring;
	//请求体


	//请求行
	str_http_line = cJSON_GetObjectItem(cjson_http, HTTP_LINE)->valuestring;
	//解析请求行
	int pos = 0;
	pos = sk_strpos(str_http_line, "\r\n");
	char tmp[10000];
	if(pos)
	{	
		str_http_line = strncpy(tmp, str_http_line, pos);
	}
	//获取请求行的各项值	
	split_header_line(str_http_line, &str_http_line_method, &str_http_line_url, &str_http_line_version);
	http->http_line->method = str_http_line_method;
	http->http_line->url = str_http_line_url;
	http->http_line->version = str_http_line_version;

	//请求头
	http->http_header->accept = cJSON_GetObjectItem(cjson_http_header, ACCEPT)->valuestring;
	http->http_header->referer = cJSON_GetObjectItem(cjson_http_header, REFERER)->valuestring;
	http->http_header->accept_language = cJSON_GetObjectItem(cjson_http_header, ACCEPT_LANGUAGE)->valuestring;
	http->http_header->user_agent = cJSON_GetObjectItem(cjson_http_header, USER_AGENT)->valuestring;
	http->http_header->content_type = cJSON_GetObjectItem(cjson_http_header, CONTENT_TYPE)->valuestring;
	http->http_header->host = cJSON_GetObjectItem(cjson_http_header, HOST)->valuestring;
	http->http_header->content_len = cJSON_GetObjectItem(cjson_http_header, CONTENT_LENGTH)->valueint;
	http->http_header->conn_status = cJSON_GetObjectItem(cjson_http_header, CONNECTION)->valuestring;
	http->http_header->cache_control = cJSON_GetObjectItem(cjson_http_header, CACHE_CONTROL)->valuestring;
	http->http_header->cookie = cJSON_GetObjectItem(cjson_http_header, COOKIE)->valuestring;

	if( http->http_header->content_len == -1){

		return SK_OK;
	}
	http->http_body = sk_create_temp_buf(r->pool,http->http_header->content_len);
	size_t	size;
	size = (http->http_header->content_len == strlen(cJSON_GetObjectItem(cjson_http, HTTP_BODY)->valuestring) )
			? http->http_header->content_len:strlen(cJSON_GetObjectItem(cjson_http, HTTP_BODY)->valuestring);
	memcpy(http->http_body->last ,
			cJSON_GetObjectItem(cjson_http, HTTP_BODY)->valuestring,
			size);
	http->http_body->last += http->http_header->content_len;

	if(size != http->http_header->content_len) {
		http->http_header->content_len = size;
		//printf("warning \n");
	}
	return SK_OK;

}

/**
 * @desc 分割请求头部数据，转为method url version
 * @param char *str_http_line 请求行头部数据
 * @param char **method 请求行头部method
 * @param char **url 请求行头部url
 * @param char **version 请求行头部version
 * @return bool 成功返回true，失败返回false
*/
bool split_header_line(char *str_http_line, char **method, char **url, char **version)
{
	if(NULL == str_http_line)
	{
		return FALSE;
	}
	char s[10000];
	char *tmp[10]; int i=0;
	strcpy(s, str_http_line);
	char *delim = " ";
	char *p;
	*method = strtok(s, delim);
	while((p = strtok(NULL, delim)) && i<10)
	{
		tmp[i] = p;
		i++;
	}
	*url = tmp[0];
	*version = tmp[1];	
	return TRUE;
}

#if 0
/**
 * @desc 获取发送给body检测模块的数据，将要发送给body的数据整合成JSON的格式
 * @param data_t data 唯一标志key
 * @param char *body 发送的body数据
 * @return char *返回json格式字符串
 */
char * get_body_data(data_t data, char *body)
{
	if(0 == data || NULL == body)
	{
		return NULL;
	}
	char *json_body; char sid[13];
	cJSON *cjson_body = cJSON_CreateObject();
	sprintf(sid , "%ld", data);
	cJSON_AddItemToObject(cjson_body, "sid", cJSON_CreateString(sid));
	cJSON_AddItemToObject(cjson_body, "body", cJSON_CreateString(body));
	json_body = cJSON_Print(cjson_body);
	cJSON_Delete(cjson_body);
	return json_body;
}

/**
 * @desc 分析body模块接收到的数据
 * @param char* str_body 接收到的数据
 * @param char *sid 获取的sid
 * @param char *body 获取的body
 * @return void
 */
bool analy_body_data(char * str_body, char *sid, char *body)
{
	if(NULL == str_body)
	{
		return FALSE;
	}
	cJSON *cjson_body;char *tmp1, *tmp2;
	//解析json格式数据
	cjson_body = cJSON_Parse(str_body);
	tmp1 = cJSON_GetObjectItem(cjson_body, "sid")->valuestring;
	tmp2 = cJSON_GetObjectItem(cjson_body, "body")->valuestring;
	strcpy(sid, tmp1);
	strcpy(body, tmp2);
	cJSON_Delete(cjson_body);
	return TRUE;
}
#endif
