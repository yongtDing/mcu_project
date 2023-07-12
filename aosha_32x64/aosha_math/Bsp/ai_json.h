/*************************************************************************
	> File Name: ai_json.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/1/31 21:39:56
 ************************************************************************/

#ifndef __AI_JSON_H__
#define __AI_JSON_H__

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"

typedef struct json_context {
    char service[32];
    char body[256];
	void (*match_extract)(uint8_t*, uint16_t, void *);
} json_context_t;

json_context_t *aid_paser_json_context(char *service_name);
void *aid_extract_config(json_context_t *json_context, uint8_t *cmd, uint16_t cmd_size, void *process_handle);
void aid_paser_json_demo(uint8_t *json_buffer, uint16_t json_size);

#endif

#if 0
{"seq":2,"vendor":[{"sid":"WifiMod:WifiMod:WifiSet","data":{"key":"88888888","ssid":"HS"}}]}
#endif
