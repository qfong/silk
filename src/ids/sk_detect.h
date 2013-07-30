#ifndef __DETECT_H__
#define __DETECT_H__
#include "sk_config.h"
#include "sk_core.h"
#include "sk_rule.h"
#include "mwm.h"

#define NOCASE 1

#define ENGINE_NUM		5

/*危险等级*/
#define ACTIVATION		0
#define DYNAMIC			1
#define ALERT				2
#define PASS				3
#define LOG				4

typedef struct sk_detect_engine_s	sk_detect_engine_t;
typedef struct sk_detect_s			sk_detect_t;

struct sk_detect_engine_s{
	MWM_STRUCT* engine;
	char name[100];
	void		*(*create)();
	int		 	(*add_rule)(void* engine, const rule_node_t* rule);
	int 		(*prep_rule)(void* engine);
	int 		(*search)(void* engine, unsigned char* text, int text_len,
	int			(*match)(void* id, int index, void* data), void* data);
	void 		(*destroy)(void* engine);
	int 		(*match)(void* id, int index, void*data);
};

struct sk_detect_s
{
	int 					debug;
	sk_detect_engine_t	engine[ENGINE_NUM];
};

extern int fpdetect_create();
extern int fpdetect_add_rule(rule_node_t* rule_node);
extern int sk_detect_pre_rule();
//extern int fpdetect_search_rule(char* text, int text_len, char* sid);
void fpdetect_search_rule(sk_list_t *list);
#endif /*#ifndef __DETECT_H__*/
