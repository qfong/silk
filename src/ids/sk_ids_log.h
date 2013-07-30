

#ifndef __LOG_H__
#define __LOG_H__

#include "sk_rule.h"

#define MAX_LOG_LEN 1024
#define ERR_LOG_PATH "/var/log/silk/error.log"
#define DETECT_LOG_PATH	"/var/log/silk/detection.log"

extern int save_err_log(char* format, ...);
extern int save_msg_log(char* format, ...);
extern int save_msg(rule_node_t* rule);

#endif /*#ifndef __LOG_H__*/
