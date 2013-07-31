// Copyright (c) 2013 The Silk Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// Written by qingfeng.wang,  some source code form snort,nginx.
// http://www.selinuxplus.com frank.wqf@gmail.com
// Thanks to China Standard software Co.,Ltd ;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "sk_ids.h"
#include "sk_tools.h"
#include "sk_config.h"
#include "sk_core.h"


#define STD_BUF 1024

int sk_strpos(const char *haystack, const char *needle)
{
	int ignorecase = 0;
	register unsigned char c, needc;
	unsigned char const *from, *end;
	int len = strlen(haystack);
	int needlen = strlen(needle);
	from = (unsigned char *)haystack;
	end = (unsigned char *)haystack + len;
	const char *findreset = needle;
	int i = 0;
	for (i=0; from < end; ++i) {
		c = *from++;
		needc = *needle;
		if (ignorecase) {
			if (c >= 65 && c < 97)
				c += 32;
			if (needc >= 65 && needc < 97)
				needc += 32;
		}
		if(c == needc) {
			++needle;
			if(*needle == '\0') {
				if (len == needlen)
					return 0;
				else
					return i - needlen+1;
			}
		}
		else {
			if(*needle == '\0' && needlen > 0)
				return i - needlen +1;
			needle = findreset;
		}
	}
	return  -1;
}


/*
 ** Function: FatalError(const char *, ...)
 **
 ** Purpose: When a fatal error occurs, this function prints the error message
 **          and cleanly shuts down the program
 **
 ** Arguments: format => the formatted error string to print out
 **            ... => format commands/fillers
 **
 ** Returns: void function
 **/
void FatalError(const char *format,...)
{
	char buf[STD_BUF+1];
	va_list ap;

	va_start(ap, format);

	vsnprintf(buf, STD_BUF, format, ap);

#if 0
	if(pv.daemon_flag)
	{
		syslog(LOG_CONS | LOG_DAEMON | LOG_ERR, "FATAL ERROR: %s", buf);
	}
	else
#endif
	{
		fprintf(stderr, "ERROR: %s", buf);
		fprintf(stderr,"Fatal Error, Quitting..\n");
	}

	exit(1);
}

int strip(char *data)
{
	int size;
	char *end;
	char *idx;

	idx = data;
	end = data + strlen(data);
	size = end - idx;

	while(idx != end)
	{
		if((*idx == '\n') ||
			(*idx == '\r'))
		{
			*idx = 0;
			size--;
		}
		if(*idx == '\t')
		{
			*idx = ' ';
		}
		idx++;
	}

	return size;
}


/****************************************************************
 *
 *  Function: mSplit()
 *
 *  Purpose: Splits a string into tokens non-destructively.
 *
 *  Parameters:
 *      char *str => the string to be split
 *      char *sep => a string of token separators
 *      int max_strs => how many tokens should be returned
 *      int *toks => place to store the number of tokens found in str
 *      char meta => the "escape metacharacter", treat the character
 *                   after this character as a literal and "escape" a
 *                   seperator
 *
 *  Returns:
 *      2D char array with one token per "row" of the returned
 *      array.
 *
 ****************************************************************/
char **m_split(char *str, char *sep, int max_strs, int *toks, char meta)
{
    char 				**retstr;      /* 2D array which is returned to caller */
    char 				*idx;          /* index pointer into str */
    char 				*end;          /* ptr to end of str */
    char 				*sep_end;      /* ptr to end of seperator string */
    char 				*sep_idx;      /* index ptr into seperator string */
    int 				len = 0;       /* length of current token string */
    int 				curr_str = 0;  /* current index into the 2D return array */
    char 				last_char = (char) 0xFF;

    if(!toks) return NULL;

    *toks = 0;

    if (!str) return NULL;

    /*
     * find the ends of the respective passed strings so our while() loops
     * know where to stop
     */
    sep_end = sep + strlen(sep);
    end = str + strlen(str);

    /* remove trailing whitespace */
#if 0 //暂去掉
    while(isspace((int) *(end - 1)) && ((end - 1) >= str))
        *(--end) = '\0';    /* -1 because of NULL */
#endif
    /* set our indexing pointers */
    sep_idx = sep;
    idx = str;

    /*
     * alloc space for the return string, this is where the pointers to the
     * tokens will be stored
     */
    if((retstr = (char **) malloc((sizeof(char **) * max_strs))) == NULL)
	{
		printf("fatal error\n");
		return NULL;
	}

    max_strs--;

    /* loop thru each letter in the string being tokenized */
    while(idx < end)
    {
        /* loop thru each seperator string char */
        while(sep_idx < sep_end)
        {
            /*
             * if the current string-indexed char matches the current
             * seperator char...
             */
            if((*idx == *sep_idx) && (last_char != meta))
            {
                /* if there's something to store... */
                if(len > 0)
                {
                    if(curr_str <= max_strs)
                    {
                        /* allocate space for the new token */
                        if((retstr[curr_str] = (char *)
                                    malloc((sizeof(char) * len) + 1)) == NULL)
                        {
                            printf("malloc");
							return NULL;
                        }

                        /* copy the token into the return string array */
                        memcpy(retstr[curr_str], (idx - len), len);
                        retstr[curr_str][len] = 0;

                        /* twiddle the necessary pointers and vars */
                        len = 0;
                        curr_str++;

                        last_char = *idx;
                        idx++;
                    }

                    /*
                     * if we've gotten all the tokens requested, return the
                     * list
                     */
                    if(curr_str >= max_strs)
                    {
#if 0 //暂时去掉
                        while(isspace((int) *idx))
                            idx++;
#endif
                        len = end - idx;
                        fflush(stdout);

                        if((retstr[curr_str] = (char *)
                                    malloc((sizeof(char) * len) + 1)) == NULL)
						{
							printf("malloc");
							return NULL;
						}

                        memcpy(retstr[curr_str], idx, len);
                        retstr[curr_str][len] = 0;

                        *toks = curr_str + 1;
                        return retstr;
                    }
                }
                else
                    /*
                     * otherwise, the previous char was a seperator as well,
                     * and we should just continue
                     */
                {
                    last_char = *idx;
                    idx++;
                    /* make sure to reset this so we test all the sep. chars */
                    sep_idx = sep;
                    len = 0;
                }
            }
            else
            {
                /* go to the next seperator */
                sep_idx++;
            }
        }

        sep_idx = sep;
        len++;
        last_char = *idx;
        idx++;
    }

    /* put the last string into the list */

    if(len > 0)
    {
        if((retstr[curr_str] = (char *)
                    malloc((sizeof(char) * len) + 1)) == NULL)
		{
         //   FatalPrintError("malloc");
			printf("malloc\n");
			return NULL;
		}

        memcpy(retstr[curr_str], (idx - len), len);
        retstr[curr_str][len] = 0;

        *toks = curr_str + 1;
    }

    /* return the token list */
    return retstr;
}





/****************************************************************
 *
 * Free the buffer allocated by mSplit().
 *
 * char** toks = NULL;
 * int num_toks = 0;
 * toks = (str, " ", 2, &num_toks, 0);
 * mSplitFree(&toks, num_toks);
 *
 * At this point, toks is again NULL.
 *
 ****************************************************************/
void m_split_free(char ***pbuf, int num_toks)
{
    int i;
    char** buf;  /* array of string pointers */

    if( pbuf==NULL || *pbuf==NULL )
    {
        return;
    }

    buf = *pbuf;

    for( i=0; i<num_toks; i++ )
    {
        if( buf[i] != NULL )
        {
            free( buf[i] );
            buf[i] = NULL;
        }
    }

    free(buf);
    *pbuf = NULL;
}

/*
 *判断文件夹是否存在
 */
int dir_exist(char* path)
{
	if(path == NULL)
		return -1;

	struct stat stat;
	if(lstat(path, &stat) < 0)
		return -1;

	if(S_ISDIR(stat.st_mode))
	{
		return 0;
	}
	else //文件存在，却不是目录文件，删除
	{
		remove(path);
		return -1;
	}

	return -1;
}

/*
 * 确认文件夹存在，若不存在，创建
 */
int confirm_dir_exist(char* path)
{
	if(path == NULL)
		return -1;
	if(dir_exist(path) == 0)
		return 0;

	char buf[DEFAULT_BUF_LEN*2] = {0};
	char** toks;
	int num_toks;
	int i = 0;

	toks = m_split(path, "/", 128, &num_toks, 0);
	while(toks[i] != NULL)
	{
		sprintf(buf+strlen(buf), "/%s", toks[i++]);
		if(dir_exist(buf) < 0)
			if(mkdir(buf, 0755)<0)
				return -1;
	}

	return 0;
}



u_char *
sk_cpystrn(u_char *dst, u_char *src, size_t n)
{
  if (n == 0) {
         return dst;
    }

     while (--n) {
         *dst = *src;

         if (*dst == '\0') {
             return dst;
         }

         dst++;
         src++;
     }

     *dst = '\0';

     return dst;
}

#define SK_SETPROCTITLE_PAD       '\0'
extern char **environ;

static char *sk_os_argv_last;

sk_ptr_t
sk_init_setproctitle(sk_log_t *log)
{
    u_char      *p;
    size_t       size;
    sk_ptr_t   i;

    size = 0;

    for (i = 0; environ[i]; i++) {
        size += sk_strlen(environ[i]) + 1;
    }

    p = sk_alloc(size);
    if (p == NULL) {
        return SK_ERR;
    }

    sk_os_argv_last = sk_os_argv[0];

    for (i = 0; sk_os_argv[i]; i++) {
        if (sk_os_argv_last == sk_os_argv[i]) {
            sk_os_argv_last = sk_os_argv[i] + sk_strlen(sk_os_argv[i]) + 1;
        }
    }

    for (i = 0; environ[i]; i++) {
        if (sk_os_argv_last == environ[i]) {

            size = sk_strlen(environ[i]) + 1;
            sk_os_argv_last = environ[i] + size;

            sk_cpystrn(p, (u_char *) environ[i], size);
            environ[i] = (char *) p;
            p += size;
        }
    }

    sk_os_argv_last--;

    return SK_OK;
}


void
sk_setproctitle(char *title)
{
    u_char     *p;

    sk_os_argv[1] = NULL;

    p = sk_cpystrn((u_char *) sk_os_argv[0], (u_char *) "silk: ",
                    sk_os_argv_last - sk_os_argv[0]);

    p = sk_cpystrn(p, (u_char *) title, sk_os_argv_last - (char *) p);

    if (sk_os_argv_last - (char *) p) {
        sk_memset(p, SK_SETPROCTITLE_PAD, sk_os_argv_last - (char *) p);
    }


}
