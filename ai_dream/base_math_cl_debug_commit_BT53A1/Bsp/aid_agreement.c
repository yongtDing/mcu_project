/*************************************************************************
	> File Name: aid_agreement.c
	> Author: ma6174
	> Mail: ma6174@163.com
	> Created Time: 2023/1/28 14:43:25
 ************************************************************************/

#include "aid_agreement.h"
#include "uart.h"
#include "cJSON.h"


#define ENABLE_DEBUG_USART_LOG 

#define SEND_TEST_TOTAL 200
#define MAX_SEND_RAW_SIZE_TOTAL 160
#define MAX_SEND_NEXT 153
#define AID_HEADER_SIZE 7
#define AID_LENGTH_SIZE 4

aid_message_t aid_message = {0};
aid_message_body_match_t body_match = {0};

#define AID_CIR_BUF 1024
uint8_t aid_cir_buffer[AID_CIR_BUF] = {0};

void raw_buffer_send_split(uint8_t *buffer, uint16_t size, uint16_t once_max);
void aid_init_raw_sensor_data(void);
json_context_t aijson_context[] =
{
    {"AiCfgVer","{\"ver\":100}"},

    {"deviceInfo",
//     "{\"sn\":\"D0BAE48F0893\",\"vendor\":{\"devName\":\"\",\"deviceInfo\":{\"productId\":\"D001\",\"manu\":\"DREAM1\",\"blemac\":\"D0BAE48F0893\",\"wifimac\":\"\",\"fwv\":\"1.0.1\",\"hwv\":\"C65.1.0\"}}}"
     "{\"sn\":\"E9C3500298F3\",\"vendor\":{\"devName\":\"\",\"deviceInfo\":{\"productId\":\"D101\",\"manu\":\"DREAM1\",\"blemac\":\"E9C3500298F3\"}}}"
    },

    {"authSetup",
     "{\"errcode\":0}"
    },

    {"createSession",
     "{\"seq\":1,\"sn2\":\"6233373430326635\",\"uuid\":\"123454\"}"
    },

    {"customData",
     "{\"errcode\":0}"
    },

    {"customSecData",
     "{\"errcode\":0}"
    },

    {"autosend",
     "{\"seq\":123,\"vendor\":[{\"sid\":\"Press:upload\",\"data\":{\"column\":32,\"row\":64,\"frame\":123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678}}]}"
    },

    {"default_deviceInfo",
     "{\"sn\":\"D0BAE48F0893\",\"vendor\":{\"devName\":\"\",\"deviceInfo\":{\"productId\":\"D001\",\"manu\":\"DREAM1\",\"blemac\":\"D0BAE48F0893\",\"wifimac\":\"\",\"fwv\":\"1.0.1\",\"hwv\":\"C65.1.0\"}}}"
    }
};

char json_raw_value_start[88] =
{
    "{\"seq\":123,\"vendor\":[{\"sid\":\"Press:upload\",\"data\":{\"column\":32,\"row\":64,\"frame\":"
};

char json_raw_value_end[8] =
{
    "}}]}"
};

aid_custom_value_data_t aid_custom_value = {0};

aid_agreement_context_t agreement_context_common = {0};


uint16_t CalChecksum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum += data[i];
    }
    return sum;
}


void aid_cmd_circular_init(aid_agreement_context_t **agreement_context)
{
    *agreement_context = &agreement_context_common;
    CircBuf_Init(&(*agreement_context)->circular_handle, aid_cir_buffer, AID_CIR_BUF);
    aid_init_raw_sensor_data();
}

int aid_frame_check(aid_agreement_context_t *agreement_context,
                    aid_message_t *aid_message)
{
    return 0;
}

int aid_message_match_char(void *buffer, uint16_t buffer_size)
{
    uint8_t service_length = 0, body_length = 0;
    aid_message_t *aid_message = NULL;
    aid_message = (aid_message_t *)buffer;
    aid_message_service_match_t pri_service_match = {0};
    aid_message_body_match_t pri_body_match = {0};
    if (aid_message->message_header.single_frame == 0)
    {
        service_length = aid_message->message_payload.service_length;
        if (aid_message->message_header.payload_size != 0)
        {
            body_length = aid_message->message_header.payload_size - service_length - 4;
        } else {
            body_length = buffer_size - service_length - 4;
        }
        memcpy((void *)&pri_service_match,
                (void *)&aid_message->message_payload,
                service_length + 2);
        memcpy((void *)&pri_body_match,
               (void *)(aid_message->message_payload.body + service_length),
               body_length);
        printf("service :%s \n body %d:%s\n", pri_service_match.service, body_length, pri_body_match.body);
    } else {
        body_length = aid_message->message_header.payload_size;
        memcpy((void *)&pri_body_match,
               (void *)aid_message->message_payload.body,
               body_length);
        printf("service :%s \n body %d:%s\n", pri_service_match.service, body_length, pri_body_match.body);
    }

    return 0;
}

int aid_recive_message_first_frame(aid_agreement_context_t *aid_agreement_context,
                                   aid_message_t *aid_message,
                                   uint16_t buffer_size)
{
    uint16_t body_size = 0;
    aid_session_context_t *recv_session_context = &aid_agreement_context->recv_session_context;
    aid_session_context_t *ack_session_context = &aid_agreement_context->ack_session_context;

    if (aid_message->message_header.total_frame - aid_message->message_header.single_frame == 1)
    {
        recv_session_context->cmd_finish = true;
        memcpy(&recv_session_context->header, &aid_message->message_header, AID_HEADER_SIZE);
        ack_session_context->cmd_finish = false;
        memcpy(&ack_session_context->header, &aid_message->message_header, AID_HEADER_SIZE);

        recv_session_context->service_length = aid_message->message_payload.service_length;
        ack_session_context->service_length = aid_message->message_payload.service_length;
        memcpy(recv_session_context->service,
               aid_message->message_payload.body,
               aid_message->message_payload.service_length);
        memcpy(ack_session_context->service,
               aid_message->message_payload.body,
               aid_message->message_payload.service_length);

        body_size = aid_message->message_payload.body[recv_session_context->service_length]
                    | aid_message->message_payload.body[recv_session_context->service_length + 1] << 8;
        recv_session_context->total_body_size = body_size;
        recv_session_context->rec_body_size = body_size;
        if (body_size > 0)
        {
            CircBuf_Push(&aid_agreement_context->circular_handle,
                         aid_message->message_payload.body + recv_session_context->service_length + 2,
                         recv_session_context->rec_body_size);
        }
    } else if (aid_message->message_header.total_frame - aid_message->message_header.single_frame > 1) {
        recv_session_context->cmd_finish = false;
        memcpy(&recv_session_context->header, &aid_message->message_header, AID_HEADER_SIZE);
        ack_session_context->cmd_finish = false;
        ack_session_context->header.ver_cmd |= 0x01;

        memcpy(&ack_session_context->header, &aid_message->message_header, AID_HEADER_SIZE);

        recv_session_context->service_length = aid_message->message_payload.service_length;
        ack_session_context->service_length = aid_message->message_payload.service_length;
        memcpy(recv_session_context->service,
               aid_message->message_payload.body,
               aid_message->message_payload.service_length);
        memcpy(ack_session_context->service,
               aid_message->message_payload.body,
               aid_message->message_payload.service_length);

        body_size = aid_message->message_payload.body[recv_session_context->service_length]
                    | aid_message->message_payload.body[recv_session_context->service_length + 1] << 8;

        recv_session_context->total_body_size = body_size;
        recv_session_context->rec_body_size = MAX_SEND_RAW_SIZE_TOTAL
                                             - AID_HEADER_SIZE
                                             - AID_LENGTH_SIZE
                                             - recv_session_context->service_length;
        CircBuf_Push(&aid_agreement_context->circular_handle,
                     aid_message->message_payload.body + recv_session_context->service_length + 2,
                     recv_session_context->rec_body_size);
    }
    return 0;
}

int aid_create_session()
{

    return 0;
}

int aid_clean_message_session(aid_agreement_context_t *aid_agreement_context)
{
    memset(&aid_agreement_context->recv_session_context, 0x00, sizeof(aid_session_context_t));
    memset(&aid_agreement_context->ack_session_context, 0x00, sizeof(aid_session_context_t));
    return 0;
}

int aid_make_up_message_next(aid_agreement_context_t *agreement_context,
                                aid_message_t *aid_message,
                                uint16_t buffer_size)
{
    uint8_t body_size = 0;
    aid_session_context_t *recv_session_context = &agreement_context->recv_session_context;

    if (buffer_size == 0)
        goto err;
    if (aid_message->message_header.total_frame - aid_message->message_header.single_frame == 1)
    {
        recv_session_context->cmd_finish = true;
    }
    body_size = buffer_size - AID_HEADER_SIZE;
    if (body_size != aid_message->message_header.payload_size)
    {
        //printf("err! body_size != header_body_size %d != %d", body_size, aid_message->message_header.payload_size);
    }
    CircBuf_Push(&agreement_context->circular_handle,
                (uint8_t *)&aid_message->message_payload,
                body_size);
    return 0;
err:
    return 1;
}

int aid_printf_message_session(aid_agreement_context_t *agreement_context)
{
    return 0;
}

int aid_ack_message(aid_agreement_context_t *agreement_context)
{
    uint16_t  body_length = 0x00;
    uint8_t cmd_count = 0;
    uint16_t json_count = 0;
    aid_session_context_t *recv_session_context = &agreement_context->recv_session_context;
    aid_session_context_t *ack_session_context = &agreement_context->ack_session_context;

    json_count = sizeof(aijson_context) / sizeof(json_context_t);
    if (recv_session_context->cmd_finish == true)
    {
        //usart_dma_send_data(USART_2_TR, (uint8_t *)recv_session_context->service, recv_session_context->service_length);
        //delay_1ms(20);

        body_length = CircBuf_GetUsedSize(&agreement_context->circular_handle);
        if (body_length > 0)
        {
            CircBuf_Pop(&agreement_context->circular_handle, recv_session_context->body_cache, body_length);
            //usart_dma_send_data(USART_2_TR, (uint8_t *)recv_session_context->body_cache, body_length);
            //delay_1ms(20);
        }

        for (cmd_count = 0; cmd_count < json_count; cmd_count ++)
        {
            if (!strcmp((const char *)aijson_context[cmd_count].service,
                        (const char *)ack_session_context->service))
            {
                memcpy(ack_session_context->body_cache,
                        aijson_context[cmd_count].body,
                        strlen(aijson_context[cmd_count].body));

                ack_session_context->total_body_size
                    = strlen(aijson_context[cmd_count].body);
            }

            if (!strcmp((const char*)ack_session_context->service, "customData"))
            {
                agreement_context->enable_raw_value_ack = true;
                agreement_context->send_count = 0;
            }
        }

#if 1
        if (cmd_count <= json_count)
        {
            aid_message_raw_buffer_send(ack_session_context->header.message_id,
                                        ack_session_context->service,
                                        ack_session_context->service_length,
                                        ack_session_context->body_cache,
                                        ack_session_context->total_body_size
                                        );
            delay_1ms(50);
        }
#endif
        aid_clean_message_session(agreement_context);
    }

    return 0;
}

int aid_message_match(aid_agreement_context_t *agreement_context,
                      uint8_t *cache_buffer,
                      uint8_t cache_size)
{
    memset((uint8_t *)&aid_message, 0x0, sizeof(aid_message_t));
    memcpy((uint8_t *)&aid_message, (uint8_t *)cache_buffer, cache_size);

    if (aid_message.message_header.single_frame == 0)
    {
        aid_recive_message_first_frame(agreement_context, &aid_message, cache_size);
    } else {
        aid_make_up_message_next(agreement_context, &aid_message, cache_size);
    }

    if (agreement_context->recv_session_context.cmd_finish)
    {
        aid_ack_message(agreement_context);
    } else {
#ifdef ENABLE_DEBUG_USART_LOG
        printf("recv need too more!\n");
#endif
    }

    return 0;
}

int aid_message_raw_buffer_auto_ack(uint8_t message_id,
                                uint8_t *service_name,
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
    uint16_t sum_check = 0;

    first_frame_body_size = MAX_SEND_RAW_SIZE_TOTAL - AID_HEADER_SIZE - service_length - 4;

    if (body_size > first_frame_body_size)
    {
        resend_cache_count = (body_size - first_frame_body_size) / MAX_SEND_NEXT;
        resend_cache_count ++;
        body_done_frame_size = first_frame_body_size;
    } else {
        body_done_frame_size = body_size;
    }

    aid_header.ver_cmd = 0x02;
    aid_header.message_id = message_id;
    aid_header.total_frame = ++ resend_cache_count;
    aid_header.single_frame = send_frame_count;
    aid_header.encry_type = 0x01;
    aid_header.payload_size = body_done_frame_size + service_length + 4;

    service_match.rev_res = 0x11;
    service_match.service_length = service_length;
    memcpy(service_match.service, service_name, service_length);
    sum_check = CalChecksum((uint8_t *)&service_match, service_length + 2);

    ((aid_custom_value_data_t *)body_buffer)->sum_check += sum_check;

    body_match.body_length = body_size;
    memcpy(body_match.body, body_buffer, body_done_frame_size);

    sum_check = CalChecksum((uint8_t *)&body_match, 2);
    ((aid_custom_value_data_t *)body_buffer)->sum_check += sum_check;


    memcpy((uint8_t *)&aid_message.message_header, (uint8_t *)&aid_header, AID_HEADER_SIZE);
    memcpy((uint8_t *)&aid_message.message_payload, (uint8_t *)&service_match, service_match.service_length + 2);

    if (1)
    {
        memcpy((uint8_t *)&aid_message.message_payload.body + service_match.service_length,
               (uint8_t *)&body_match,
               body_done_frame_size + 2);
    }

    usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + service_length + body_done_frame_size + 4);

#ifdef ENABLE_DEBUG_USART_LOG
    usart_dma_send_data(USART_2_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + service_length + body_done_frame_size + 4);
#endif
    delay_1ms(50);

    for (; body_done_frame_size < body_size; body_done_frame_size += MAX_SEND_NEXT)
    {
        aid_message.message_header.single_frame = ++ send_frame_count;
        if (body_size - body_done_frame_size > MAX_SEND_NEXT)
        {
            memcpy((uint8_t *)&aid_message.message_payload, body_buffer + body_done_frame_size, MAX_SEND_NEXT);
            aid_message.message_header.payload_size = MAX_SEND_NEXT;
            usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + MAX_SEND_NEXT);
#ifdef ENABLE_DEBUG_USART_LOG
            usart_dma_send_data(USART_2_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + MAX_SEND_NEXT);
#endif
        }
        else {
            memcpy((uint8_t *)&aid_message.message_payload, body_buffer + body_done_frame_size, body_size - body_done_frame_size);
            aid_message.message_header.payload_size = body_size - body_done_frame_size;
            usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + body_size - body_done_frame_size);
#ifdef ENABLE_DEBUG_USART_LOG
            usart_dma_send_data(USART_2_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + body_size - body_done_frame_size);
#endif
        }
        delay_1ms(50);
    }

    return 0;
}

void aid_mx_set_auto_ack(aid_agreement_context_t *agreement_context,
                         bool enable)
{
    if (enable)
        agreement_context->enable_raw_value_ble_ack = true;
    else
    {
        agreement_context->enable_raw_value_ble_ack = false;
        agreement_context->enable_raw_value_ack = false;
        //agreement_context->send_count = SEND_TEST_TOTAL;
    }
}

void aid_mx_value_send(aid_agreement_context_t *agreement_context,
                       uint8_t *raw_value,
                       uint16_t raw_size)
{

#if 1
    if (!agreement_context->enable_raw_value_ble_ack)
        goto out;
#endif
    aid_custom_value.value_type = 0x01;
    aid_custom_value.column_context.value_type = 0x01;
    aid_custom_value.column_context.value_length = 0x01;
    aid_custom_value.column_value = 0x40;

    aid_custom_value.row_context.value_type = 0x02;
    aid_custom_value.row_context.value_length = 0x01;
    aid_custom_value.row_value = 0x20;

    aid_custom_value.value_context.value_type = 0x03;
    aid_custom_value.value_context.value_length = raw_size;
    memcpy(aid_custom_value.raw_value,
           raw_value,
           raw_size);

    aid_custom_value.sum_check = CalChecksum((uint8_t *)&aid_custom_value, sizeof(aid_custom_value_data_t) - 2);

    if (agreement_context->enable_raw_value_ack)
    {
        aid_message_raw_buffer_auto_ack(0x12,
                                        "customSumData",
                                        13,
                                        (uint8_t *)&aid_custom_value,
                                        sizeof(aid_custom_value_data_t));
        //agreement_context->enable_raw_value_ack = false;
    }

out:
    ;
}


aid_sensor_value_data_t sensor_data = {0};
uint8_t data_static[] =
        {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
         1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

void aid_init_raw_sensor_data()
{
    uint8_t count = 0 ; 
    for (; count < 32; count ++)
    {
        memcpy((uint8_t *)sensor_data.raw_value[count], data_static, 64);
    }
}

#define ENABLE_USART2_DEBUG_RAW_DATA
void aid_mx_value_send_raw(aid_agreement_context_t *agreement_context,
                           uint8_t *raw_value,
                           uint16_t raw_size)
{

    if (agreement_context->enable_raw_value_ack
        && agreement_context->enable_raw_value_ble_ack
        && agreement_context->send_count < SEND_TEST_TOTAL)
    {
        agreement_context->send_count ++;
        memcpy(sensor_data.header, "pressupload", 11);
        sensor_data.total_struct_size = sizeof(aid_sensor_value_data_t);
        sensor_data.column_context.value_type = 0x01;
        sensor_data.column_context.value_length = 0x01;
        sensor_data.column_value = 0x40;

        sensor_data.row_context.value_type = 0x02;
        sensor_data.row_context.value_length = 0x01;
        sensor_data.row_value = 0x20;

        sensor_data.value_context.value_type = 0x03;
        sensor_data.value_context.value_length = raw_size;


        if (1)
        {
            memcpy(sensor_data.raw_value, raw_value, raw_size);
            //memset(sensor_data.raw_value, (uint8_t)agreement_context->send_count, raw_size);
        }

        sensor_data.sum_check = CalChecksum((uint8_t *)&sensor_data,
                sizeof(aid_sensor_value_data_t) - 2);

#if 1
#ifdef ENABLE_USART2_DEBUG_RAW_DATA
        printf("send once %d\n", agreement_context->send_count);
#endif
        usart_dma_send_data(USART_3_TR,
                            (uint8_t *)&sensor_data,
                            sizeof(aid_sensor_value_data_t));
#else
        printf("send once %d\n", agreement_context->send_count);
        raw_buffer_send_split((uint8_t *)&sensor_data,
                              sizeof(aid_sensor_value_data_t),
                              243);
#endif
    }
}

void raw_buffer_send_split(uint8_t *buffer, uint16_t size, uint16_t once_max)
{
    uint16_t total_buffer_size = size;
    uint16_t send_buffer_done = 0;
    for (send_buffer_done = 0;
         send_buffer_done < total_buffer_size;
         send_buffer_done += once_max)
    {
        if (total_buffer_size - send_buffer_done < once_max)
        {
            usart_dma_send_data(USART_3_TR,
                                buffer + send_buffer_done,
                                total_buffer_size - send_buffer_done);
#ifdef ENABLE_USART2_DEBUG_RAW_DATA
            usart_dma_send_data(USART_2_TR,
                                buffer + send_buffer_done,
                                total_buffer_size - send_buffer_done);
#endif
        } else {
            usart_dma_send_data(USART_3_TR,
                                buffer + send_buffer_done,
                                once_max);
#ifdef ENABLE_USART2_DEBUG_RAW_DATA
            usart_dma_send_data(USART_2_TR,
                                buffer + send_buffer_done,
                                once_max);
#endif
        }
        delay_1ms(3);
    }
}

int aid_message_raw_buffer_send(uint8_t message_id,
                                uint8_t *service_name,
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
    aid_header.message_id = message_id;
    aid_header.total_frame = ++ resend_cache_count;
    aid_header.single_frame = send_frame_count;
    aid_header.payload_size = body_done_frame_size + service_length + 4;

    service_match.rev_res = 0x11;
    service_match.service_length = service_length;
    memcpy(service_match.service, service_name, service_length);

    body_match.body_length = body_size;
    memcpy(body_match.body, body_buffer, body_done_frame_size);

    memcpy((uint8_t *)&aid_message.message_header, (uint8_t *)&aid_header, AID_HEADER_SIZE);
    memcpy((uint8_t *)&aid_message.message_payload, (uint8_t *)&service_match, service_match.service_length + 2);
    memcpy((uint8_t *)&aid_message.message_payload.body + service_match.service_length,
           (uint8_t *)&body_match,
           body_match.body_length + 2);

    usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + service_length + body_done_frame_size + 4);

#ifdef ENABLE_DEBUG_USART_LOG
    usart_dma_send_data(USART_2_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + service_length + body_done_frame_size + 4);
#endif
    delay_1ms(50);

    for (; body_done_frame_size < body_size; body_done_frame_size += MAX_SEND_NEXT)
    {
        aid_message.message_header.single_frame = ++ send_frame_count;
        if (body_size - body_done_frame_size > MAX_SEND_NEXT)
        {
            memcpy((uint8_t *)&aid_message.message_payload, body_buffer + body_done_frame_size, MAX_SEND_NEXT);
            aid_message.message_header.payload_size = MAX_SEND_NEXT;
            usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + MAX_SEND_NEXT);
#ifdef ENABLE_DEBUG_USART_LOG
            usart_dma_send_data(USART_2_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + MAX_SEND_NEXT);
#endif
        }
        else {
            memcpy((uint8_t *)&aid_message.message_payload, body_buffer + body_done_frame_size, body_size - body_done_frame_size);
            aid_message.message_header.payload_size = body_size - body_done_frame_size;
            usart_dma_send_data(USART_3_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + body_size - body_done_frame_size);
#ifdef ENABLE_DEBUG_USART_LOG
            usart_dma_send_data(USART_2_TR, (uint8_t *)&aid_message, AID_HEADER_SIZE + body_size - body_done_frame_size);
#endif
        }
        delay_1ms(50);
    }

    return 0;
}

void aid_paser_json(uint8_t *json_buffer,
                    uint16_t json_size)
{
    cJSON *cjson = NULL, *single_json = NULL;
    char *buffer = NULL;
    json_buffer = (uint8_t *)aijson_context[3].body;
    json_size = strlen(aijson_context[3].body);
    cjson = cJSON_Parse((const char*)json_buffer);
    single_json = cJSON_GetObjectItem(cjson, "vendor");
    single_json = cJSON_GetObjectItem(single_json, "deviceInfo");
    single_json = cJSON_GetObjectItem(single_json, "manu");
    printf("%s:json size %d  %s\n", __FUNCTION__, json_size, single_json->valuestring);
    cJSON_Delete(cjson);


    cjson = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson, "wzh", "like");
    buffer = cJSON_PrintUnformatted(cjson);
    printf("%s  %d", buffer, strlen(buffer));
    cJSON_Delete(cjson);
}

#if 1
void aid_create_json(uint8_t *json_buffer, uint16_t json_size)
{
    cJSON *main_json = NULL, *seq_json = NULL, *vendor_json = NULL, *value_json = NULL;
    char *buffer = NULL;

    main_json = cJSON_CreateObject();
    seq_json = cJSON_CreateObject();
    vendor_json = cJSON_CreateArray();
    value_json = cJSON_CreateObject();

    cJSON_AddStringToObject(main_json, "seq", "1");

    cJSON_AddItemToObject(main_json, "seq_1", seq_json);
    cJSON_AddStringToObject(seq_json, "seq_single", "2");

    cJSON_AddItemToObject(main_json, "vendor_2", vendor_json);

    cJSON_AddItemToObject(vendor_json, "sensor", value_json);

    cJSON_AddStringToObject(value_json, "value_0", "1-1");
    cJSON_AddStringToObject(value_json, "frame", "1234567890");
    cJSON_AddStringToObject(value_json, "frame", "1234567890");

    buffer = cJSON_PrintUnformatted(main_json);
    //usart_dma_send_data(USART_3_TR, buffer, strlen(buffer));
    printf("%d", strlen(buffer));
    cJSON_Delete(main_json);

}
#endif
