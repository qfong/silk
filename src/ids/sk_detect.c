#include <stdlib.h>
#include <errno.h>
#include "sk_config.h"
#include "sk_core.h"

#include "mwm.h"
#include "sk_rule.h"
#include "sk_detect.h"
#include "sk_config.h"

static sk_detect_t skdetect;

static int engine_init(sk_detect_engine_t* engine, char* name);

int sk_detect_create()
{
	int 				i;
	sk_detect_t* 		p_engine;
	/*
	 * 设置参数
	 */
	skdetect.debug = 0;

	char 	engine_name[5][20] = {
			"activation",
			"dynamic",
			"alert",
			"pass",
			"log"
	};


	for(i=0; i<ENGINE_NUM; i++){
		p_engine = &skdetect.engine[i];
		if(engine_init(p_engine, engine_name[i])<0)
			return SK_ERR;
	}

	return 0;
}

int fpdetect_add_rule(rule_node_t* rule_node)
{
	if(rule_node == NULL)
		return -1;

	int level = rule_node->level;
	sk_detect_engine_t* p_engine = &skdetect.engine[level];

	rule_node_t* prule_node = (rule_node_t*)calloc(1, sizeof(rule_node_t));
	if(prule_node == NULL)
		return -1;
	memcpy(prule_node, rule_node, sizeof(rule_node_t));

	p_engine->add_rule(p_engine->engine, prule_node);

	return 0;
}

int sk_detect_pre_rule(char* text, int len)
{
	int i;
	sk_detect_engine_t* p_engine;

	for(i=0; i<ENGINE_NUM; i++)
	{
		p_engine = &skdetect.engine[i];
		p_engine->prep_rule(p_engine->engine);
	}

	return 0;
}

void fpdetect_search_rule(sk_list_t *list)
{
	char * text;
	int len;
	//char *sid;
	text = list->current;
	len = list->len;
	//sid = list->sid;
	//char sid[128] = {0};
	//memset(sid, 0, sizeof(sid));

	if(text == NULL || len < 0){
		return ;
	}
	int i;
	sk_detect_engine_t* p_engine;
	char data[PARSERULE_SIZE];
	int nfound;
	char* body = text;
	int body_len = len;

	//memcpy(sid, text, NIDS_SID_LEN);

	for(i=0; i<ENGINE_NUM; i++)
	{
		p_engine = &skdetect.engine[i];

		if((nfound = p_engine->search(p_engine->engine, (unsigned char*)body, body_len, p_engine->match, data)) > 0)
		{

			list->found = nfound;
			return ;
		}
	}
	list->found = 0;
	return ;
}

static int engine_init(sk_detect_engine_t* engine, char* name)
{
	if(engine == NULL || name == NULL)
		return -1;

	strcpy(engine->name, name);

	engine->create = mwmNew;
	engine->add_rule = mwmAddRule;
	engine->prep_rule = mwmPrepPatterns;
	engine->search = mwmSearch;
	engine->destroy = mwmFree;
	engine->match = mwmMatch;

	engine->engine = engine->create();
	if(engine->engine == NULL)
		return -1;

	return 0;
}
