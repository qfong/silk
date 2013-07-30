

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <stdarg.h>
#include "sk_rule.h"
#include "sk_config.h"
#include "sk_ids_log.h"


extern sk_ctx_t* conf;

/*
 *Function:		save_err_log
 *Description:	将出错信息存储到error.log文件
 *Input:		info:出错信息
 *Output:
 *Return:		0:succ -1:fail
 *Others:		错误信息不要超过512字节的长度
 */
int save_err_log(char* format, ...)
{
	if(format == NULL)
		return -1;

	char* log_info = (char*)calloc(MAX_LOG_LEN, sizeof(char));
	if(log_info == NULL)
		return -1;

	time_t		now = time(NULL);
	struct tm*	tm = localtime(&now);
	sprintf(log_info, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d [error] ",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	va_list	va;
	va_start(va, format);
	if(vsprintf(log_info+28, format, va)<0)
	{
		free(log_info);
		return -1;
	}
	va_end(va);

	int len = strlen(log_info)+1;
	strcat(log_info, "\n");

#ifdef _DEBUG
	printf("%s\n", log_info);
#endif
	int fd = open(sk_ctx->err_log, O_RDWR|O_CREAT|O_APPEND, 0644);
	if(fd < 0)
	{
		free(log_info);
		return -1;
	}
	write(fd, log_info, len);

	free(log_info);
	close(fd);
	return 0;
}


int save_msg_log(char* format, ...)
{
	if(format == NULL)
		return -1;

	char* log_info = (char*)calloc(MAX_LOG_LEN, sizeof(char));
	if(log_info == NULL)
		return -1;

	time_t		now = time(NULL);
	struct tm*	tm = localtime(&now);
	sprintf(log_info, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d [message] ",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	va_list	va;
	va_start(va, format);
	if(vsprintf(log_info+30, format, va)<0)
	{
		free(log_info);
		return -1;
	}
	va_end(va);

	int len = strlen(log_info)+1;
	strcat(log_info, "\n");

#ifdef _DEBUG
	printf("%s\n", log_info);
#endif
	int fd = open(sk_ctx->err_log, O_RDWR|O_CREAT|O_APPEND, 0644);
	if(fd < 0)
	{
		free(log_info);
		return -1;
	}
	write(fd, log_info, len);

	free(log_info);
	close(fd);
	return 0;
}

int save_detect_log(rule_node_t* rule)
{
	if(rule == NULL)
		return -1;

	char* log_info = (char*)calloc(MAX_LOG_LEN, sizeof(char));
	if(log_info == NULL)
		return -1;

	time_t		now = time(NULL);
	struct tm*	tm = localtime(&now);
	sprintf(log_info, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	sprintf(log_info+20, "\"msg:%s\" \"type:%s\" \"content:%s\"", rule->msg, rule->type, rule->content);

	int len = strlen(log_info)+1;
	strcat(log_info, "\n");

#ifdef _DEBUG
	printf("%s\n", log_info);
#endif
	int fd = open(sk_ctx->detect_log, O_RDWR|O_CREAT|O_APPEND, 0644);
	if(fd < 0)
	{
		free(log_info);
		return -1;
	}
	write(fd, log_info, len);

	free(log_info);
	close(fd);
	return 0;
}
