// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "sk_ids_log.h"
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#define	_U	01	/* Upper case */
#define	_L	02	/* Lower case */
#define	_N	04	/* Numeral (digit) */
#define	_S	010	/* Spacing character */
#define	_P	020	/* Punctuation */
#define	_C	040	/* Control character */
#define	_B	0100	/* Blank */
#define	_X	0200	/* heXadecimal digit */

#define SUCC	0
#define FAIL	-1

#define NOR_ERR(err_info)	{	\
	save_err_log(err_info); \
	return -1; \
}

#define NOR_ERR_CHECK(ret, err_info)	{	\
	if(ret < 0)	\
	{	\
		NOR_ERR(err_info);	\
	}	\
}

#define FATAL_ERR(err_info)	{	\
	save_err_log(err_info);	 \
	exit(1);	\
}

#define FATAL_ERR_CHECK(ret, err_info)	{	\
	if(ret == SK_ERR)	\
		FATAL_ERR(err_info);	\
}

#define FREE(ptr)	{	\
	if((ptr) != NULL)	\
	{	\
		free(ptr);	\
		(ptr) = NULL;	\
	}	\
}

#define DEL_CHIEF_SPACE(str)	{	\
	while(*str == ' ' || *str == '\t')	\
		str++;	\
}

#define ONE_CHECK(_onevar, xxx)	\
	(_onevar)++;				\
	if((_onevar)>1)				\
	{							\
		FatalError("%s(%d)=>Only one '%s' option per rule\n",\
				file_name, file_line, xxx);						\
	}

void FatalError(const char *format,...);
int sk_strpos(const char *haystack, const char *needle);
/*字符串处理相关函数*/
int strip(char* data);							/*去掉 \n \t \r */
/*字符串分隔*/
char** m_split(char* str, char* sep, int max_strs, int* toks, char meta);
void m_split_free(char ***pbuf, int num_toks);

/*目录处理相关参数*/
int dir_exist(char* path);
int confirm_dir_exist(char* path);

u_char * sk_cpystrn(u_char *dst, u_char *src, size_t n);
#endif /*#ifndef __TOOLS_H__*/
