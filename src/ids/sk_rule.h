#ifndef __RULE_H__
#define __RULE_H__

/*规则结构体*/
typedef struct rule_node_t
{
	char 	msg[256];			//message
	char 	type[128];			//type
	int 	level;					//级别

	char 	content[8192];		//rule content
	int  	content_len;			//rule content length

	int	 	nocase;				//区分大小写
	int		id;					//rule ID

	char 	file_name[512];		//rule所在配置文件名
	int 	file_line;			//rule所在配置文件行号
}rule_node_t;

extern int parse_rule_content(char* rule);

#endif /* #ifndef __RULE_H__ */
