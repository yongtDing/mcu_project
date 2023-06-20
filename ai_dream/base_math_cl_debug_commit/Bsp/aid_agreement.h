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

#pragma pack (1)

typedef struct aid_message_header {
    uint8_t ver_cmd;  // 4:4
    uint8_t message_id;
    uint8_t total_frame;
    uint8_t single_frame;
    uint8_t payload_size;
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

typedef struct aid_custom_context {
    uint8_t value_type;
    uint16_t value_length;
} aid_custom_context_t;

typedef struct aid_custom_value_data {
    uint8_t value_type;
    aid_custom_context_t column_context;
    uint8_t column_value;
    aid_custom_context_t row_context;
    uint8_t row_value;
    aid_custom_context_t value_context;
    uint8_t raw_value[32][64];
    uint16_t sum_check;
} aid_custom_value_data_t;

typedef struct aid_sensor_value_data {
    char header[11];
    uint16_t total_struct_size;
    aid_custom_context_t column_context;
    uint8_t column_value;
    aid_custom_context_t row_context;
    uint8_t row_value;
    aid_custom_context_t value_context;
    uint8_t raw_value[32][64];
    uint16_t sum_check;
}aid_sensor_value_data_t;

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
    bool enable_raw_value_ack;
    bool enable_raw_value_ble_ack;
    bool wifi_set_success;
    uint32_t send_count;
} aid_agreement_context_t;

int aid_message_raw_buffer_send(uint8_t message_id,
                                uint8_t *service_name,
                                uint8_t service_length,
                                uint8_t *body_buffer,
                                uint16_t body_size);
void aid_cmd_circular_init(aid_agreement_context_t **agreement_context);
int aid_message_match(aid_agreement_context_t *agreement_context,
                      uint8_t *cache_buffer,
                      uint8_t cache_size);

int aid_message_match_char(void *buffer,
                           uint16_t buffer_size);
void aid_mx_value_send(aid_agreement_context_t *agreement_context,
                       uint8_t *raw_value,
                       uint16_t raw_size);

uint16_t CalChecksum(uint8_t *data, uint16_t len);

void aid_mx_set_auto_ack(aid_agreement_context_t *agreement_context,
                         bool enable);

void aid_mx_value_send_raw(aid_agreement_context_t *agreement_context,
                           uint8_t *raw_value,
                           uint16_t raw_size);
#endif
