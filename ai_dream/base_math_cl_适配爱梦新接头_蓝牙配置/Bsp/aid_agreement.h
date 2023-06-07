/*************************************************************************
	> File Name: aid_agreement.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/1/28 14:43:36
 ************************************************************************/

#ifndef __AID_AGREEMENT_H__
#define __AID_AGREEMENT_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "circular_buffer.h"
#include <stdbool.h>
#include <stdlib.h>
#include "ai_json.h"

#pragma pack (1)

typedef struct aid_message_header {
    uint8_t ver_cmd;  // 4:4
    uint8_t message_id;
    uint8_t total_frame;
    uint8_t single_frame;
    uint8_t rev_res;
    uint8_t encry_type;
    uint8_t return_type;
} aid_message_header_t;

typedef struct aid_message_payload {
    uint8_t rev_res;
    uint8_t service_length;
    uint8_t body[256];
} aid_message_payload_t;

typedef struct aid_message_service_match {
    uint8_t rev_res;
    uint8_t service_length;
    uint8_t service[168];
} aid_message_service_match_t;

typedef struct aid_message_body_match {
    uint16_t body_length;
    uint8_t body[168];
} aid_message_body_match_t;

typedef struct aid_message {
    aid_message_header_t message_header;
    aid_message_payload_t message_payload;
} aid_message_t;

#pragma pack ()

typedef struct aid_session_context {
    aid_message_header_t header;
    uint8_t service_length;
    uint8_t service[32];
    uint16_t total_body_size;
    uint16_t rec_body_size;
    uint8_t body_cache[512];
    bool  cmd_finish;
} aid_session_context_t;

typedef struct aid_agreement_context {
    aid_session_context_t recv_session_context;
    aid_session_context_t ack_session_context;
    CircBuf_t circular_handle;
} aid_agreement_context_t;

int aid_message_raw_buffer_send(uint8_t message_id, uint8_t *service_name, uint8_t service_length,  uint8_t *body_buffer, uint16_t body_size);
void aid_paser_json(uint8_t *json_buffer, uint16_t json_size);
void aid_cmd_circular_init(aid_agreement_context_t **agreement_context);
int aid_message_match(aid_agreement_context_t *agreement_context,
                      uint8_t *cache_buffer,
                      uint8_t cache_size);

#endif
