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
#include "cJSON.h"

typedef struct json_context {
    char service[32];
    char body[256];
} json_context_t;

#endif

