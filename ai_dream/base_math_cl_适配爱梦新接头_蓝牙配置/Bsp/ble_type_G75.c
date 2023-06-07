#include "ble_type_G75.h"

ble_g75_command_context_t *G75_ble_cmd_context_cmp(ble_g75_cmd_t cmd_enum, const uint8_t *cmd_char)
{
    ble_g75_command_context_t *ble_cmd = NULL;
    uint8_t cmd_count = 0, count = 0;

    count = sizeof(ble_g75_cmd_context) / sizeof(ble_g75_command_context_t);

    if ((cmd_enum != BLE_G75_NULL && cmd_char != NULL)
        || (cmd_enum == BLE_G75_NULL && cmd_char == NULL))
    {
        goto param_err;
    }

    if (cmd_enum != BLE_G75_NULL)
    {
        for (count = 0; count < cmd_count; count ++)
        {
            if (cmd_enum != BLE_G75_NULL)
            {
                if (ble_g75_cmd_context[count].command == cmd_enum)
                {
                    ble_cmd = (ble_g75_command_context_t *)ble_g75_cmd_context + count;
                    break;
                }
            }

            if (cmd_char != NULL)
            {
                if (strcmp((char *)ble_g75_cmd_context[count].keyword_cmd_char, (char *)cmd_char) == 0)
                {
                    ble_cmd = (ble_g75_command_context_t *)ble_g75_cmd_context + count;
                    break;
                }
            }
        }
    }

param_err:
    return ble_cmd;
}

uint8_t send_cmd_buffer[128] = {0};
uint8_t recv_cmd_buffer[128] = {0};
int G75_ble_sys_cmd_sync(ble_g75_cmd_t cmd, ble_g75_cfg_t config)
{
    uint8_t count = 0;
	return 0;
}

