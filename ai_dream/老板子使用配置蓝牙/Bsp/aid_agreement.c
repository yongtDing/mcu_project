/*************************************************************************
	> File Name: aid_agreement.c
	> Author: ma6174
	> Mail: ma6174@163.com
	> Created Time: 2023/1/28 14:43:25
 ************************************************************************/

#include "aid_agreement.h"
#include "uart.h"
#include "cJSON.h"

#define MAX_SEND_RAW_SIZE_TOTAL 160
#define MAX_SEND_NEXT 153
#define AID_HEADER_SIZE 7

aid_message_t aid_message = {0};
aid_message_body_match_t body_match = {0};
aid_message_service_match_t service_match = {0};

#define AID_CIR_BUF 2048
uint8_t aid_cir_buffer[AID_CIR_BUF] = {0};

uint8_t cache_char[512] = {0};

void aid_cmd_circular_init(aid_agreement_context_t **agreement_context)
{
    *agreement_context = calloc(1, sizeof(aid_agreement_context_t));
    CircBuf_Init(&(*agreement_context)->circular_handle, aid_cir_buffer, AID_CIR_BUF);
}

int aid_frame_check(aid_agreement_context_t *agreement_context,
                    aid_message_t *aid_message)
{
    return 0;
}

int aid_create_message_session(aid_session_context_t *aid_session_context,
                               aid_message_t *aid_message)
{
    uint16_t body_size = 0;
    if (aid_message->message_header.total_frame - aid_message->message_header.single_frame == 1)
    {
        aid_session_context->cmd_finish = true;
        memcpy(&aid_session_context->header, &aid_message->message_header, AID_HEADER_SIZE);
        aid_session_context->service_length = aid_message->message_payload.service_length;
        memcpy(aid_session_context->service,
               aid_message->message_payload.body,
               aid_message->message_payload.service_length);
        body_size = aid_message->message_payload.body[aid_session_context->service_length]
                    | aid_message->message_payload.body[aid_session_context->service_length + 1] << 8;
        aid_session_context->total_body_size = body_size;
        aid_session_context->rec_body_size = body_size;
        memcpy(aid_session_context->body_cache,
               aid_message->message_payload.body + aid_session_context->service_length + 2,
               body_size);
    }
    return 0;
}

int aid_clean_message_session()
{
    return 0;
}

int aid_message_match(aid_agreement_context_t *agreement_context,
                      uint8_t *cache_buffer,
                      uint8_t cache_size)
{
    uint8_t service_length = 0;
    uint16_t body_length = 0;
    uint8_t first_frame_max_body_size = 0;
    uint16_t body_total_count = 0;

    memset((uint8_t *)&aid_message, 0x0, sizeof(aid_message_t));
    memcpy((uint8_t *)&aid_message, (uint8_t *)cache_buffer, cache_size);

    if (aid_message.message_header.single_frame == 0)
    {
        aid_create_message_session(&agreement_context->recv_session_context, &aid_message);
        service_length = aid_message.message_payload.service_length;

        memcpy((uint8_t *)&service_match,
              (uint8_t *)&aid_message.message_payload,
               service_length + 2);

        memcpy((uint8_t *)&body_match,
               (uint8_t *)&aid_message.message_payload.body + service_length,
                2);

        first_frame_max_body_size = MAX_SEND_RAW_SIZE_TOTAL - AID_HEADER_SIZE - 4 - service_length;

        body_length = body_match.body_length;
        memcpy((uint8_t *)&body_match,
               (uint8_t *)&aid_message.message_payload.body + service_length,
                body_match.body_length + 2);
        if (first_frame_max_body_size < body_match.body_length)
        {
            body_match.body_length = first_frame_max_body_size;
        }

    } else {
        body_length = cache_size - sizeof(aid_message_header_t);
        body_match.body_length = body_length;
        memcpy((uint8_t *)&body_match.body,
               (uint8_t *)&aid_message.message_payload,
                body_length);
    }

    if (aid_message.message_header.total_frame > 1)
    {
        CircBuf_Push(&agreement_context->circular_handle, body_match.body, body_match.body_length);
        if (aid_message.message_header.total_frame - aid_message.message_header.single_frame == 1)
        {
            body_total_count = CircBuf_GetUsedSize(&agreement_context->circular_handle);
            body_total_count = CircBuf_Pop(&agreement_context->circular_handle, cache_char, body_total_count);
            usart_dma_send_data(USART_3_TR, cache_char, body_total_count);
            delay_1ms(100);
            memset(cache_char, 0x00, 512);
        }
    } else {
        CircBuf_Push(&agreement_context->circular_handle, body_match.body, body_match.body_length);
        body_total_count = CircBuf_GetUsedSize(&agreement_context->circular_handle);
        if (body_total_count > 0)
        {
            CircBuf_Pop(&agreement_context->circular_handle, cache_char, body_total_count);
            usart_dma_send_data(USART_3_TR, cache_char, body_total_count);
            delay_1ms(100);
            memset(cache_char, 0x00, 512);
        }
    }

    memset(&service_match, 0x00, sizeof(aid_message_service_match_t));
    memset(&body_match, 0x00, sizeof(aid_message_body_match_t));

    return 0;
}

int aid_message_raw_buffer_send(uint8_t *service_name,
                                uint8_t service_length,
                                uint8_t *body_buffer,
                                uint16_t body_size)
{
    aid_message_t aid_message = {0};
    aid_message_header_t aid_header = {0};
    aid_message_service_match_t service_match = {0};
    aid_message_body_match_t body_match = {0};

    uint8_t resend_cache_count = 0, send_frame_count = 0;
    uint8_t first_frame_body_size = 0;
    uint16_t body_done_frame_size = 0;

    first_frame_body_size = MAX_SEND_RAW_SIZE_TOTAL - AID_HEADER_SIZE - service_length - 4;

    if (body_size > first_frame_body_size)
    {
        resend_cache_count = (body_size - first_frame_body_size) / MAX_SEND_NEXT;
        resend_cache_count ++;
        body_done_frame_size = first_frame_body_size;
    } else {
        body_done_frame_size = body_size;
    }


    aid_header.ver_cmd = 0x01;
    aid_header.message_id = 0x01;
    aid_header.total_frame = ++ resend_cache_count;
    aid_header.single_frame = send_frame_count;

    service_match.rev_res = 0x11;
    service_match.service_length = service_length;
    memcpy(service_match.service, service_name, service_length);

    body_match.body_length = body_done_frame_size;
    memcpy(body_match.body, body_buffer, body_done_frame_size);

    memcpy((uint8_t *)&aid_message.message_header, (uint8_t *)&aid_header, AID_HEADER_SIZE);
    memcpy((uint8_t *)&aid_message.message_payload, (uint8_t *)&service_match, service_match.service_length + 2);
    memcpy((uint8_t *)&aid_message.message_payload.body + service_match.service_length,
           (uint8_t *)&body_match,
           body_match.body_length + 2);

    usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + service_length + body_done_frame_size + 4);

    delay_1ms(100);

    for (; body_done_frame_size < body_size; body_done_frame_size += MAX_SEND_NEXT)
    {
        aid_message.message_header.single_frame = ++ send_frame_count;
        if (body_size - body_done_frame_size > MAX_SEND_NEXT)
        {
            memcpy((uint8_t *)&aid_message.message_payload, body_buffer + body_done_frame_size, MAX_SEND_NEXT);
            usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + MAX_SEND_NEXT);
        }
        else {
            memcpy((uint8_t *)&aid_message.message_payload, body_buffer + body_done_frame_size, body_size - body_done_frame_size);
            usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + body_size - body_done_frame_size);
        }

        delay_1ms(100);
    }

    return 0;
}

void aid_paser_json(uint8_t *json_buffer,
                    uint16_t json_size)
{
    cJSON *cjson = NULL, *single_json = NULL;
    char *buffer = NULL;
    cjson = cJSON_Parse((const char*)json_buffer);
    single_json = cJSON_GetObjectItem(cjson, "vendor");
    single_json = cJSON_GetObjectItem(single_json, "deviceInfo");
    single_json = cJSON_GetObjectItem(single_json, "manu");
    printf("%s: %s", __FUNCTION__, single_json->valuestring);
    cJSON_Delete(cjson);


    cjson = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson, "wzh", "like");
    buffer = cJSON_PrintUnformatted(cjson);
    printf("%s  %d", buffer, strlen(buffer));
    cJSON_Delete(cjson);
}
