/**
 * Copyright (c) 2012, China Standard Software CO., LTD.
 * All rights reserved
 * @file json_http.h
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
#include "cJSON.h"
#include "sk_config.h"
#include "sk_core.h"
//#include "nids_rbtree.h"

#ifndef BOOL_TYPE_
#define BOOL_TYPE_
#define BOOL int
#define bool int
#define TRUE 1
#define FALSE 0
#endif

#define method_t char *
#define url_t char *
#define version_t char *
#define accept_t char *
#define referer_t char *
#define accept_language_t char *
#define user_agent_t char *
#define content_type_t char *
#define host_t char *
#define content_length_t int
#define connection_t char *
#define cache_control_t char *
#define cookie_t char *
#define nids_http_body_t char *
#define nide_source_ip_t char *

#define HTTP_LINE "http_line"
#define HTTP_HEADER "http_header"
#define HTTP_BODY "http_body"
#define SOURCE_IP "source_ip"

#define METHOD "method"
#define URL "url"
#define VERSION "version"

#define ACCEPT "accept"
#define REFERER "referer"
#define ACCEPT_LANGUAGE "accept-language"
#define USER_AGENT "user-agent"
#define CONTENT_TYPE "content-type"
#define HOST "host"
#define CONTENT_LENGTH "content-length"
#define CONNECTION "connection"
#define CACHE_CONTROL "cache-control"
#define COOKIE "cookie"

//定义请求行结构
typedef struct nids_http_line_t {
	method_t method;
	url_t url;
	version_t version;	
}nids_http_line_t;

//定义请求头结构
typedef struct nids_http_header_t {
	accept_t accept;
	referer_t referer;
	accept_language_t accept_language;
	user_agent_t user_agent;
	content_type_t content_type;
	host_t host;
	content_length_t content_length;
	connection_t connection;
	cache_control_t cache_control;
	cookie_t cookie;
}nids_http_header_t;

//定义http数据报文结构
typedef struct nids_http_t {
	nide_source_ip_t nids_source_ip;	
	nids_http_line_t *nids_http_line;
	nids_http_header_t *nids_http_header;
	nids_http_body_t nids_http_body;
}nids_http_t, *nids_http;

/**
 * @desc 将json数据格式字符串转换为http结构体数据
 * @param char * str_http_data 需要转换的json数据格式字符串
 * @return nids_http *nidshttp nids_http结构体指针的指针
*/
sk_ptr_t get_http_data_from_json(cJSON *cjson_http, sk_request_t *r);

/**
 * @desc 分割请求头部数据，转为method url version
 * @param char *str_http_line 请求行头部数据
 * @param char **method 请求行头部method
 * @param char **url 请求行头部url
 * @param char **version 请求行头部version
 * @return bool 成功返回true，失败返回false
*/
bool split_header_line(char *str_http_line, char **method, char **url, char **version);

/**
 * @desc 获取发送给body检测模块的数据，将要发送给body的数据整合成JSON的格式
 * @param data_t data 唯一标志key
 * @param char *body 发送的body数据
 * @return char *返回json格式字符串
 */

//char * get_body_data(data_t data, char *body);

/**
 * @desc 分析body模块接收到的数据
 * @param char* str_body 接收到的数据
 * @param char *sid 获取的sid
 * @param char *body 获取的body
 * @return void
 */
bool analy_body_data(char * str_body, char *sid, char *body);

