/*************************************************************************
  > File Name: ai_json.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: 2023/1/31 21:40:13
 ************************************************************************/

#include "ai_json.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"
#include "malloc.h"
#include "aid_agreement.h"

void aid_paser_authSetup_json(uint8_t *json_buffer, uint16_t json_size, void *process_handle);
void aid_paser_customSecData_json(uint8_t *json_buffer, uint16_t json_size, void *process_handle);
void aid_paser_customData_json(uint8_t *json_buffer, uint16_t json_size, void *process_handle);

json_context_t aijson_context[] =
{
    {   "AiCfgVer",
        "{\"ver\":100}"},

    {   "deviceInfo",
        "{\"sn\":\"E9C3500298F3\",\"vendor\":{\"devName\":\"\",\"deviceInfo\":{\"productId\":\"D101\",\"manu\":\"DREAM1\",\"blemac\":\"E9C3500298F3\"}}}"
    },

    {   "authSetup",
        "{\"errcode\":0}",
        aid_paser_authSetup_json
    },

    {   "createSession",
        "{\"seq\":1,\"sn2\":\"6233373430326635\",\"uuid\":\"123454\"}"
    },

    {   "customData",
        "{\"errcode\":0}",
        aid_paser_customData_json
    },

    {   "customSecData",
        "{\"errcode\":0}",
        aid_paser_customSecData_json
    },

    {   "autosend",
        "{\"seq\":123,\"vendor\":[{\"sid\":\"Press:upload\",\"data\":{\"column\":32,\"row\":64,\"\
            frame\":123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678}}]}"
    },

    {   "default_deviceInfo",
        "{\"sn\":\"D0BAE48F0893\",\"vendor\":{\"devName\":\"\",\"deviceInfo\":{\"productId\":\"D001\",\"manu\":\"DREAM1\",\"blemac\":\"D0BAE48F0893\",\"wifimac\":\"\",\"fwv\":\"1.0.1\",\"hwv\":\"C65.1.0\"}}}"
    }
};

json_context_t *aid_paser_json_context(char *service_name)
{

    uint8_t cmd_count = 0;
    json_context_t *json_context = NULL;
    uint16_t json_count = sizeof(aijson_context) / sizeof(json_context_t);

    for (cmd_count = 0; cmd_count < json_count; cmd_count ++)
    {
        if (!strcmp((const char *)aijson_context[cmd_count].service,
                    (const char *)service_name))
        {
            json_context = aijson_context + cmd_count;
        }
    }

    return json_context;
}

void *aid_extract_config(json_context_t *json_context, uint8_t *cmd, uint16_t cmd_size, void *process_handle)
{
    if (json_context->match_extract)
    {
        json_context->match_extract(cmd, cmd_size, process_handle);
    }
    return NULL;
}


void aid_paser_authSetup_json(uint8_t *json_buffer, uint16_t json_size, void *process_handle)
{
    cJSON *total_cjson = NULL, *single_json = NULL;
    total_cjson = cJSON_Parse((const char*)json_buffer);

    single_json = cJSON_GetObjectItem(total_cjson, "authCode");
    if (single_json)
        printf("%s:  %s/%s\n", __FUNCTION__, single_json->string, single_json->valuestring);
    else {
        printf("%s can not find authCode name\n", __FUNCTION__);
    }

    single_json = cJSON_GetObjectItem(total_cjson, "devSecret");
    printf("%s: %s/%s\n", __FUNCTION__, single_json->string, single_json->valuestring);

    single_json = cJSON_GetObjectItem(total_cjson, "productkey");
    printf("%s: %s/%s\n", __FUNCTION__, single_json->string, single_json->valuestring);

    single_json = cJSON_GetObjectItem(total_cjson, "devName");
    printf("%s: %s/%s\n", __FUNCTION__, single_json->string, single_json->valuestring);

    cJSON_Delete(total_cjson);
}

void aid_paser_customSecData_json(uint8_t *json_buffer, uint16_t json_size, void *process_handle)
{
    cJSON *total_cjson = NULL, *single_json = NULL, *single_json_2 = NULL;
    aid_agreement_context_t *agreement_context = (aid_agreement_context_t *)process_handle;
    total_cjson = cJSON_Parse((const char*)json_buffer);


    single_json = cJSON_GetObjectItem(total_cjson, "vendor");
    if (single_json) {
        single_json = cJSON_GetArrayItem(single_json, 0);
        single_json_2 = cJSON_GetObjectItem(single_json, "data");
        if (single_json_2)
        {
            single_json = cJSON_GetObjectItem(single_json_2, "key");
            printf("%s: key/%s", __FUNCTION__, single_json->valuestring);
            single_json = cJSON_GetObjectItem(single_json_2, "ssid");
            printf("%s: ssid/%s", __FUNCTION__, single_json->valuestring);
            if (single_json)
                agreement_context->wifi_set_success = true;
        }
        //single_json_2 = cJSON_GetObjectItem(single_json, "seq");
        //printf("%s: seq/%s", __FUNCTION__, single_json_2->valuestring);
#if 0
        agreement_context->enable_raw_value_ack = true;
        agreement_context->send_count = 0;
#endif
    } else {

    }
    if (total_cjson)
    {
        cJSON_Delete(total_cjson);
    }
}

void aid_paser_customData_json(uint8_t *json_buffer, uint16_t json_size, void *process_handle)
{
    cJSON *total_cjson = NULL, *single_json = NULL, *single_json_2 = NULL, *vendor_json = NULL;
    aid_agreement_context_t *agreement_context = (aid_agreement_context_t *)process_handle;
    total_cjson = cJSON_Parse((const char*)json_buffer);


    vendor_json = cJSON_GetObjectItem(total_cjson, "vendor");
    if (vendor_json) {
        single_json = cJSON_GetArrayItem(vendor_json, 0);
        single_json_2 = cJSON_GetObjectItem(single_json, "data");
        if (single_json_2)
        {
            single_json = cJSON_GetObjectItem(single_json_2, "key");
            if (single_json)
            {
                printf("%s: key/%s", __FUNCTION__, single_json->valuestring);
                single_json = cJSON_GetObjectItem(single_json_2, "ssid");
                printf("%s: ssid/%s", __FUNCTION__, single_json->valuestring);
                if (single_json)
                    agreement_context->wifi_set_success = true;
            }
        }

        single_json = cJSON_GetArrayItem(vendor_json, 0);
        single_json_2 = cJSON_GetObjectItem(single_json, "sid");
        if (single_json_2)
        {
            printf("%s: sid %s", __FUNCTION__, single_json_2->valuestring);
            if (strcmp(single_json_2->valuestring, "Press:start") == 0)
            {
                agreement_context->enable_raw_value_ack = true;
                agreement_context->send_count = 0;
            }

        } else {
                printf("do not find sid!!\n");
        }

        //single_json_2 = cJSON_GetObjectItem(single_json, "seq");
        //printf("%s: seq/%s", __FUNCTION__, single_json_2->valuestring);
#if 0
        agreement_context->enable_raw_value_ack = true;
        agreement_context->send_count = 0;
#endif
    } else {
    }
    if (total_cjson)
    {
        cJSON_Delete(total_cjson);
    }
}

uint8_t json_buffer_test[] = "{\"seq\":2,\"vendor\":[{\"sid\":\"WifiMod:WifiMod:WifiSet\",\"data\":{\"key\":\"88888888\",\"ssid\":\"HS\"}}]}";

//#define SJON_DEMO_ENABLE
#ifdef SJON_DEMO_ENABLE
void aid_paser_json_demo(uint8_t *json_buffer,
        uint16_t json_size)
{
    cJSON *cjson = NULL, *single_json = NULL;
    json_buffer = (uint8_t *)aijson_context[1].body;
    json_size = strlen(aijson_context[1].body);

    json_buffer = json_buffer_test;
    json_size = strlen(json_buffer);
    cjson = cJSON_Parse((const char*)json_buffer);

    single_json = cJSON_GetObjectItem(cjson, "vendor");
    single_json = cJSON_GetArrayItem(single_json, 0);
    single_json = cJSON_GetObjectItem(single_json, "sid");
    printf("%s:json size %d  %s\n", __FUNCTION__, json_size, single_json->valuestring);

    cJSON_Delete(cjson);
}

void aid_create_json_demo(uint8_t *json_buffer, uint16_t json_size)
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
    cJSON_AddStringToObject(value_json, "value_2", "1123124321412312321312312-1");
    cJSON_AddStringToObject(value_json, "frame", "1234567890");
    cJSON_AddStringToObject(value_json, "frame", "1234567890");

    buffer = cJSON_PrintUnformatted(main_json);
    usart_dma_send_data(USART_2_TR, (uint8_t *)buffer, strlen(buffer));
    delay_1ms(50);
    printf("%d\n", strlen(buffer));

    printf("line %d  %d\n", __LINE__, my_mem_perused(SRAMIN));
    cJSON_Delete(main_json);
    board_free((uint8_t *)buffer);
    printf("line %d  %d\n", __LINE__, my_mem_perused(SRAMIN));
}

#endif
