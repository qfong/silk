
// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include "sk_config.h"
#include "sk_core.h"
#include <dirent.h>




char			*file_name = NULL;	/*当前读取的文件名*/
int  			file_line;	/*当前读取的文件行*/


static sk_ptr_t rule_type(char* type);
static sk_ptr_t var_define(char* name, char* value);



/*
 * return 0:succ -1:fail
 */
void disp_conf()
{
	printf("detect_port:\t%d\n", sk_ctx->port);
	printf("max_listen_num:\t%d\n", sk_ctx->max_listen_num);
	printf("mempool_num:\t%d\n", sk_ctx->mempool_num);
	printf("mempool_size:\t%d\n", sk_ctx->mempool_size);
	printf("rules_file_dir:\t%s\n", sk_ctx->rules_file_dir);
	printf("log_dir:\t%s\n", sk_ctx->log_dir);

	return;
}

/*
 * return 0:succ -1:fail
 */
sk_ptr_t
get_conf(sk_ctx_t* p_conf)
{
	if(p_conf==NULL)
		return SK_ERR;

	memcpy(p_conf, &sk_ctx, sizeof(sk_ctx_t));

	return SK_OK;
}


/*
 *Function:		set_default_conf
 *Description:	为参数结构体分配空间，同时设定程序默认参数
 *Input:
 *Output:
 *Return:		0:succ -1:fail
 *Others:
 */
sk_ptr_t
set_default_conf(sk_ctx_t *sk_ctx)
{
	sk_ctx = (sk_ctx_t*)calloc(1, sizeof(sk_ctx_t));
	if(sk_ctx==NULL)
		FATAL_ERR("calloc conf error");

	//server
	sk_ctx->port = 5555;
	sk_ctx->process = 2;
	sk_ctx->max_listen_num = 1024;
	//memory pool
	sk_ctx->mempool_num = 1024;
	sk_ctx->mempool_size = 512;
	//nids
	//log
	strcpy(sk_ctx->log_dir, "/var/log/silk");
	strcpy(sk_ctx->err_log, "/var/log/silk/error.log");
	strcpy(sk_ctx->detect_log, "/var/log/silk/detect.log");
	//rules
	strcpy(sk_ctx->rules_file_dir, RULES_FILE_DIR);

	if(confirm_dir_exist(sk_ctx->log_dir) < 0){
		save_err_log("init log dir error");
		return SK_ERR;
	}


	return SK_OK;
}


/*
 *Function:		parse_rule_file
 *Description:	读取配置文件或规则文件
 *Input:		file	:	文件路径
 *				inclevel:	文件标志，MAIN_CONF表示主配置文件, RULE_CONF表示规则文件
 *Output:
 *Return:		0:succ -1:fail
 *Others:
 */
 sk_ptr_t
sk_parse_rule_file(char* file, sk_t inclevel)
{
	FILE					*thefp;	/*file pointer for the rule file or config file*/
	char 					buf[2048] = {0};
	char					*index;
	char					*stored_file_name = file_name;
	int 					stored_file_line = file_line;
//	char* new_line;
	struct 	stat 		file_stat;	/*for inclue path testing*/

	file_line = 0;
	file_name = strdup(file);
	if(file_name == NULL)
		FATAL_ERR("strdup file_name error");

	if(stat(file_name, &file_stat)<0){
		free(file_name);
		save_err_log("profile %s doesn't exist", file_name);
		if(inclevel == 0)
			exit(1);
		else
			return SK_ERR;
	}

	if((thefp = fopen(file_name, "r")) == NULL){

		save_err_log("open profile:%s error", file_name);
		FREE(file_name);
		file_name = stored_file_name;
		file_line = stored_file_line;
		return SK_ERR;
	}

	sk_memzero((char*)buf, FILE_LINE_LEN);

	/*loop each file line and send ift to rule parser*/
	while(fgets(buf, FILE_LINE_LEN, thefp) != NULL){

		file_line++;
		index = buf;
		while(*index==' ' || *index=='\t')
			index++;


		if((*index != '#') && (*index != 0x0a) && (index != NULL)){

			sk_parse_rule(thefp, index, inclevel);
		}
		bzero((char*)buf, FILE_LINE_LEN);

	}

	fclose(thefp);

	FREE(file_name);
	file_name = stored_file_name;
	file_line = stored_file_line;

	return SK_OK;
}


/*
 *Function:		parse_rule
 *Description:	分析配置文件中一行内容
 *Input:		thefp	:	文件流
 *				prule	:	一行配置文件内容
 *				inclevel:	文件标志
 *Output:
 *Return:		0:succ -1:fail
 *Others:
 */
sk_ptr_t
sk_parse_rule(FILE* thefp, char* prule, sk_t inclevel)
{
	char				** toks;
	int 				num_toks;
	int 				ruletype;
	char 				rule[PARSERULE_SIZE] = {0};
	char				* tmp;
	int					offset;

	bzero((void*)rule, sizeof(rule));

	strcpy(rule, prule);

	toks = m_split(rule, " ", 10, &num_toks, 0);

	ruletype = rule_type(toks[0]);
	switch(ruletype){

		case SK_RULE_VAR:				//设置参数
			var_define(toks[1], toks[2]);

			m_split_free(&toks, num_toks);
			return 0;
			break;
		case SK_RULE_INCLUDE:			//读取规则文件
			if(toks[1] != NULL){
				if((tmp=strchr(toks[1], '\n')) != NULL)
					*tmp = 0x00;
				if(!memcmp(toks[1], "all", strlen("all"))){	//加载所有的规则文件
					save_msg_log("load all rule profile");
					int total;

					DIR* dir = opendir(sk_ctx->rules_file_dir);
					if(dir == NULL){
						save_err_log("rule profile dir %s is not exist");
						exit(1);
					}
					struct dirent* pdirent;
					while((pdirent = readdir(dir)) != NULL){

						if(pdirent->d_name[0] == '.')
							continue;
						if( strstr(pdirent->d_name, "rules") != NULL){

							char file_path[1024] = {0};

							sprintf(file_path, "%s/%s", sk_ctx->rules_file_dir, pdirent->d_name);

							save_msg_log("load rule profile %s", file_path);

							if(sk_parse_rule_file(file_path, RULE_CONF) < 0){
								save_msg_log("load rule profile %s error, please check your profile");
								return SK_ERR;
							}
							total++;
						}
					}
					save_msg_log("the total number of the profile is %d", total);
				}
				else{ 	//加载指定的规则文件
					char		* idx;

					idx = strchr(toks[1], '*');
					if(idx){//包含通配符

						char* name = (char*)calloc(strlen(toks[1]), sizeof(char));
						if(name == NULL)
							return -1;
						memcpy(name, toks[1], idx-toks[1]);

						DIR* dir = opendir(sk_ctx->rules_file_dir);
						if(dir == NULL){
							save_err_log("rule profile dir %s is not exist");
							exit(1);
						}
						struct dirent* pdirent;
						while((pdirent = readdir(dir)) != NULL){

							if(pdirent->d_name[0] == '.')
								continue;
							if( strstr(pdirent->d_name, "rules") != NULL){

								if(strncmp(pdirent->d_name, name, strlen(name))){
									continue;
								}
								char file_path[FILE_PATH_LEN] = {0};
								sprintf(file_path, "%s/%s", sk_ctx->rules_file_dir, pdirent->d_name);
								save_msg_log("load rule profile %s", file_path);
								if(sk_parse_rule_file(file_path, RULE_CONF) < 0){
									save_msg_log("load rule profile %s error, please check your profile");
								}
							}
						}

					}
					else
					{
						char file_path[FILE_PATH_LEN] = {0};
						sprintf(file_path, "%s/%s", sk_ctx->rules_file_dir, toks[1]);

						save_msg_log("load rule profile %s", file_path);
						if(sk_parse_rule_file(file_path, RULE_CONF) < 0){
							save_msg_log("load rule profile %s error, please check your profile");
						}
					}
				}

				m_split_free(&toks, num_toks);
				return SK_OK;
			}
			else{
				m_split_free(&toks, num_toks);
				return SK_ERR;
			}
			break;
		case SK_RULE_CONTENT:			//分析规则数据
			offset = strlen(toks[0]);
			m_split_free(&toks, num_toks);

			parse_rule_content(rule+offset);
			return SK_OK;
			break;
		default:
			return SK_ERR;
			break;
	}

	return SK_OK;
}

/*
 * return >0:rule type -1:fail
 */
static sk_ptr_t
rule_type(char* type)
{
	if(type == NULL)
		return -1;

	if(!strcmp(type, "$VAR"))
		return SK_RULE_VAR;
	else if(!strcmp(type, "include"))
		return SK_RULE_INCLUDE;
	else if(!strcmp(type, "rule"))
		return SK_RULE_CONTENT;
	else
		return SK_ERR;
}

/*
 * return 0:succ -1:fail
 */
static sk_ptr_t
var_define(char* name, char* value)
{
	if(name==NULL || value==NULL)
		return -1;

	char 					buf[DEFAULT_BUF_LEN] = {0};
	char					*p=NULL;

	if((p=strchr(value,'\n')) != NULL)
		*p = 0x00;

	//server
	if(!strcmp(name, "PORT")){
		sk_ctx->port = atoi(value);
	}
	if(!strcmp(name, "PROCESS")){
			sk_ctx->process = atoi(value);
		}
	else if(!strcmp(name, "MAX_CONNECT_NUM")){
		sk_ctx->max_listen_num = atoi(value);
	}
	//mempool
	else if(!strcmp(name, "MEMPOOL_NUM")){
		sk_ctx->mempool_num = atoi(value);
	}
	else if(!strcmp(name, "MEMPOOL_SIZE")){
		sk_ctx->mempool_size = atoi(value);
	}
	/*
	//nids
	else if(!strcmp(name, "IDS_ADDR")){
		bzero(sk_ctx->ids_addr, 20);
		strcpy(sk_ctx->ids_addr, value);
	}
	else if(!strcmp(name, "IDS_PORT")){
		sk_ctx->ids_port = atoi(value);
	}
	*/
	//log
	else if(!strcmp(name, "LOG_DIR")){
		bzero(sk_ctx->log_dir, DEFAULT_BUF_LEN);
		strcpy(sk_ctx->log_dir, value);

		NOR_ERR_CHECK(confirm_dir_exist(sk_ctx->log_dir), "init log dir error");
	}
	else if(!strcmp(name, "ERR_LOG")){
		bzero(sk_ctx->err_log, DEFAULT_BUF_LEN);
		sprintf(sk_ctx->err_log, "%s/%s", sk_ctx->log_dir, value);
	}
	else if(!strcmp(name, "DETECT_LOG")){
		bzero(sk_ctx->detect_log, DEFAULT_BUF_LEN);
		sprintf(sk_ctx->detect_log, "%s/%s", sk_ctx->log_dir, value);
	}
	//rules
	else if(!strcmp(name, "RULES_FILE_DIR")){
		bzero(sk_ctx->rules_file_dir, DEFAULT_BUF_LEN);
		strcpy(sk_ctx->rules_file_dir, value);

	}
	else{
		return SK_ERR;
	}

	return SK_OK;
}


