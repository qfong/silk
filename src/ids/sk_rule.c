#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "sk_detect.h"
#include "sk_tools.h"
#include "sk_ids.h"

extern char* file_name;		//当前读取配置文件名
extern int file_line;		//当前读取配置文件行

static int rule_id = 1;

static int parse_key_value_pair(char* pair, char* key, char* value);
static int rule_level(char* level);
static int parse_pattern(char* rule, char* pattern, int* len);

int parse_rule_content(char* rule)
{
	if(rule == NULL)
		return -1;

	char* index = rule;
	char** toks;
	int i;	// j;
	int num_toks;
//	int ruletype;
	int level;

	rule_node_t rule_node[10];	//每一行内容可能包含多个content
	int rule_num = 0;
	memset(&rule_node, 0, sizeof(rule_node_t)*10);

	while(*index == ' ' || *index =='\t')
		index++;
	for(i=0; i<strlen(index); i++)
	{
		if(*(index+i) == '\n')
		{
			*(index+i) = 0x00;
			break;
		}
	}

	toks = m_split(index, ";", PARSERULE_SIZE, &num_toks, 0);
	for(i=0; i<num_toks; i++)
	{
		char key[PARSERULE_SIZE]={0};
		char value[PARSERULE_SIZE]={0};
		char* idx;

		idx = toks[i];
		while(*idx == ' ' || *idx == '\t')
			idx++;
		if(*idx == '\n' || *idx == 0x00)	//是否是最后一个元素，即换行符
			break;

		if(parse_key_value_pair(idx, key, value) < 0)
		{
			save_err_log("profile %s, line:%d, this line is not in the expected format", file_name, file_line);
			exit(1);
		}

		if(!strcmp(key, "classtype"))
		{
			strcpy(rule_node[0].type, value);
		}
		else if(!strcmp(key, "level"))
		{
			level = rule_level(value);
			if(level < 0)
			{
				m_split_free(&toks, num_toks);
				return -1;
			}
			rule_node[0].level = level;
		}
		else if(!strcmp(key, "msg"))
		{
			strcpy(rule_node[0].msg, value);
		}
		else if(!strcmp(key, "content") || !strcmp(key, "uricontent"))
		{
			if(parse_pattern(value, rule_node[rule_num].content, &rule_node[rule_num].content_len)<0)
			{
				save_err_log("profile %s, line:%d, this line is not in the expected format", file_name, file_line);
				m_split_free(&toks, num_toks);
				return -1;
			}
			rule_num++;
		}
	}

	for(i=0; i<rule_num; i++)
	{
		rule_node[i].nocase = NOCASE;
		rule_node[i].id = rule_id++;
		strcpy(rule_node[i].file_name, file_name);
		rule_node[i].file_line = file_line;

		if(i != 0)
		{
			strcpy(rule_node[i].type, rule_node[0].type);
			rule_node[i].level = rule_node[0].level;
			strcpy(rule_node[i].msg, rule_node[0].msg);
		}
	}

	m_split_free(&toks, num_toks);

	if(rule_node[0].content_len > 0 && rule_num >= 1)
	{
		rule_node_t* cur_rule_node = &rule_node[0];
		if(rule_num == 1)
		{
			fpdetect_add_rule(cur_rule_node);
		}
		else
		{
			int j=1;
			for(j=1; j<rule_num; j++)
			{
				cur_rule_node = &rule_node[j];

				if(cur_rule_node->content_len > 0)
					fpdetect_add_rule(cur_rule_node);
			}
		}
	}
	return 0;
}

static int parse_key_value_pair(char* pair, char* key, char* value)
{
	if(pair == NULL || key == NULL || value == NULL)
		return -1;

	char** toks;
	int num_toks;
	int i;

	toks = m_split(pair, ":", PARSERULE_SIZE, &num_toks, 0);
	if(toks[0] == NULL || toks[1] == NULL)
	{
		m_split_free(&toks, num_toks);
		return -1;
	}
	else
	{
		char* p_start, *p_end;

		char tmp[2][PARSERULE_SIZE]={{0}, {0}};

		for(i=0; i<2; i++)
		{
			p_start = toks[i];
			p_end = toks[i]+strlen(toks[i])-1;

			if(*p_start=='"')
				p_start++;
			if(*p_end=='"')
				*p_end = 0x00;

			strcpy(tmp[i], p_start);
		}

		strcpy(key, tmp[0]);
		strcpy(value, tmp[1]);

		m_split_free(&toks, num_toks);
	}

	return 0;
}

static int rule_level(char* level)
{
	if(level == NULL)
		return -1;

	if(!strcmp(level, "activation"))
		return ACTIVATION;
	else if(!strcmp(level, "dynamic"))
		return DYNAMIC;
	else if(!strcmp(level, "alert"))
		return ALERT;
	else if(!strcmp(level, "pass"))
		return PASS;
	else if(!strcmp(level, "log"))
		return LOG;
	else
		return -1;
}

static int parse_pattern(char* rule, char* pattern, int* len)
{
	if(rule == NULL || pattern == NULL)
		return -1;

	unsigned char tmp_buf[2048];
	char* start_ptr;
	char* end_ptr;
	char* idx;
	char* dummy_idx;
	char* dummy_end;
	char hex_buf[3];
	int dummy_size;
	int size;
	int hexmode = 0;
	int hexsize = 0;
	int pending = 0;
	int cnt = 0;
	int literal = 0;
	int exception_flag = 0;

//	while(isspace(rule))
//		rule++;

	if(*rule == '!')
		exception_flag = 1;

	start_ptr = rule;
	end_ptr = rule+strlen(rule);

	size = end_ptr - start_ptr;
	if(size <= 0)
		return -1;

	idx = start_ptr;

	dummy_idx = (char*)tmp_buf;
	dummy_end = (dummy_idx + size);

	bzero(hex_buf, 3);
	memset(hex_buf, '0', 2);

	while(idx < end_ptr)
	{
		switch(*idx)
		{
		case '|':
			if(!literal)
			{
				if(!hexmode)
				{
					hexmode = 1;
				}
				else
				{
					if(!hexsize || hexsize%2)
					{
						save_err_log("profile:%s, line:%d, error occurs between \'|\' and \'|\'", file_name, file_line);
						return -1;
					}

					hexmode = 0;
					pending = 0;
				}
				if(hexmode)
					hexsize = 0;
			}
			else
			{
				literal = 0;
				tmp_buf[dummy_size] = start_ptr[cnt];
				dummy_size++;
			}
			break;
		case '\\':
			if(!literal)
			{
				literal = 1;
			}
			else
			{
				tmp_buf[dummy_size] = start_ptr[cnt];
				literal = 0;
				dummy_size++;
			}
			break;
		case '"':
			save_err_log("profile:%s, line:%d, error occurs near \"%10.10s\"", file_name, file_line, idx-3);
			return -1;
			break;
		default:
			if(hexmode)
			{
				if(isxdigit((int)*idx))
				{
					hexsize++;
					if(!pending)
					{
						hex_buf[0] = *idx;
						pending++;
					}
					else
					{
						hex_buf[1] = *idx;
						pending--;

						if(dummy_idx < dummy_end)
						{
							tmp_buf[dummy_size] = (unsigned char)
								strtol(hex_buf, (char**)NULL, 16)&0xFF;

							dummy_size++;
							bzero(hex_buf, 3);
							memset(hex_buf, '0', 2);
						}
						else
						{
							save_err_log("dummy size is smaller then rule size");
						}
					}
				}
				else
				{
					if(*idx != ' ')
					{
						save_err_log("profile:%s, line:%d, error occurs near \"%10.10s\"", file_name, file_line, idx-3);
					}
				}
			}
			else
			{
				if(*idx >= 0x1F && *idx <= 0x7e)	//保证是正常的可视符号
				{
					if(dummy_idx < dummy_end)
					{
						tmp_buf[dummy_size] = start_ptr[cnt];
						dummy_size++;
					}
					else
					{
						printf("Fatal error\n");
					}

					if(literal)
					{
						literal = 0;
					}
				}
				else
				{
					if(literal)
					{
						tmp_buf[dummy_size] = start_ptr[cnt];
						dummy_size++;
						literal = 0;
					}
					else
					{
						printf("FATAL ERROR\n");
					}
				}
			}
			break;
		}

		dummy_idx++;
		idx++;
		cnt++;
	}

	if(literal)
	{
		printf("error.....\n");
	}
	if(hexmode)
	{
		printf("error.....\n");
	}

	memcpy(pattern, tmp_buf, dummy_size);
	*len = dummy_size;

	return 0;
}






















