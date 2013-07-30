// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks by China Standard software Co.,Ltd ;

#ifndef _SK_DEFINE_H_INCLUDE_
#define _SK_DEFINE_H_INCLUDE_

#include <stdint.h>		//uint8_t
#include <stddef.h>		//intptr_t
#include <grp.h>			//	gid
#include <sys/types.h> 	//	uid,pid

#include <stddef.h>
#include <sys/socket.h>

/*
 * Re-define the Data types;
 */
typedef uint8_t 	u8_t;
typedef uint16_t	u16_t;
typedef uint32_t	u32_t;
typedef uint64_t	u64_t;

typedef int8_t 	s8_t;
typedef int16_t	s16_t;
typedef int32_t	s32_t;
typedef int64_t	s64_t;


/*
 * Re-definition of the extended data type for the conversion pointer
 * and integer;
 */
typedef uintptr_t		sk_uptr_t;
typedef intptr_t	sk_ptr_t;

typedef int		sk_t;
typedef int		sk_err_t;
typedef uid_t		sk_uid_t;
typedef gid_t		sk_gid_t;
typedef pid_t		sk_pid_t;

/*
 * Re-definition error code;
 */

#define SK_OK		0
#define SK_ERR	-1
#define SK_BUSY	2


typedef struct {
	size_t		len;
	u_char	   *data;
}sk_str_t;



#define HEAD_ALLOC_M(_ptr)		(((u8_t*)(_ptr))[0])
#define HEAD_ALLOC_S0(_ptr)		(((u8_t*)(_ptr))[1])
#define HEAD_ALLOC_S1(_ptr)		(((u8_t*)(_ptr))[2])
#define HEAD_ALLOC_S2(_ptr)		(((u8_t*)(_ptr))[3])
#define HEAD_ALLOC_L(_ptr)		(((u16_t*)(_ptr))[1])
#define HEAD_ALLOC_IP(_ptr)		(((u32_t*)(_ptr))[1])
#define IPADDR_LEN				sizeof("255.255.255.255")-1
#define BODY_ALLOC_L(_ptr)		(((sk_uptr_t*)(_ptr))[0])


#define sk_singal_helper(n)		SIG##n
#define sk_singal_value(n)		sk_singal_helper(n)

#define SK_PROCESS_RESPAMWN 	-3
#define sk_strlen(s)       		strlen((const char *) s)
#define sk_memcpy(dst,src,n)	(void)memcpy(dst,src,n)
#define sk_cpymem(dst,src,n)	(((u_char*)memcpy(dst,src,n))+n)
#define sk_null_string 			{ 0,NULL }
#define sk_string(str)			{ sizeof(str) - 1,(u_char *) str }
#define sk_str_set( str,text ) 	(str)->len = sizeof(text) -1;(str)->data = (u_char *) text
#define sk_str_null( str ) 		( str )->len = 0; (str)->data = NULL;

#define _exit(x)					do { } while (0)
#define sk_memzero(buf,n)		(void) memset(buf,0,n)

#define sk_align_ptr(p,a) \
	(u_char *)(((uintptr_t)(p) + ((uintptr_t)a-1 )) & ~((uintptr_t)a-1))



#define SK_AGAIN					-2

#define sk_signal_helper(n)		SIG##n
#define sk_signal_value(n) 		sk_signal_helper(n)
#define sk_value_helper(n)		#n
#define sk_values(n)				sk_value_helper(n)

#define SK_SHUTDOWN_SIGNAL		QUIT
#define SK_TERMINATE_SIGNAL		TERM
#define SK_NOACCEPT_SIGNAL		WINCH
#define SK_RECONFIGURE_SIGNAL	HUP

#define SK_REOPEN_SIGNAL			USR1
#define SK_CHANGEBIN_SIGNAL		USR2


#define SK_RT_MSG_OK(_ptr)		(((u16_t*)(_ptr))[0]) = 0x49;\
									(((u16_t*)(_ptr))[1]) = 0x01;\
									(((u32_t*)(_ptr))[1])
#define SK_RT_MSG_ERR(_ptr)		(((u16_t*)(_ptr))[0]) = 0x49;\
									(((u16_t*)(_ptr))[1]) = 0x10;\
									(((u32_t*)(_ptr))[1])

#endif
