/*************************************************************************
	> File Name: math_filter.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2022/9/8 17:34:21
 ************************************************************************/

#include <stdint.h>
#include "math_filter.h"

void math_filter_min_value(uint8_t *value_aggre,
                           uint32_t aggre_len,
                           uint8_t min_value)
{

}

void math_get_max_value(process_handle_t *handle,
                        uint8_t select_value,
                        SELECT_FLAG_T select_flag,
                        value_pos_t *max_pos) 
{
    uint32_t count = 0;
    memset(max_pos, 0, sizeof(value_pos_t));

    switch (select_flag)
    {
        case Y_SELECT:
            for (count = 0; count < handle->x_max; count ++)
            {
                if (handle->adc_raw_value[count][select_value] > max_pos->value)
                {
                    max_pos->value = handle->adc_raw_value[count][select_value];
                    max_pos->pos_x = count;
                    max_pos->pos_y = select_value;
                }
            }
            break;

        case X_SELECT:
            for (count = 0; count < handle->y_max; count ++)
            {
                if (handle->adc_raw_value[select_value][count] > max_pos->value)
                {
                    max_pos->value = handle->adc_raw_value[select_value][count];
                    max_pos->pos_x = select_value;
                    max_pos->pos_y = count;
                }
            }
            break;
        
        case ALL_SELECT:
            break;
    }

}

void math_clean_aggre_surplus(process_handle_t *handle,
                              SELECT_FLAG_T select_flag,
                              value_pos_t *max_pos) 

{
    uint32_t count = 0;

    switch (select_flag)
    {
        case Y_SELECT:
            for (count = 0; count < handle->x_max; count ++)
            {
                if (count != max_pos->pos_x)
                {
                    handle->adc_raw_value[count][max_pos->pos_y] = 0;
                }
            }
            break;

        case X_SELECT:
            for (count = 0; count < handle->y_max; count ++)
            {
                if (count != max_pos->pos_y)
                {
                    handle->adc_raw_value[max_pos->pos_x][count] = 0;
                }
            }
            break;
        
        case ALL_SELECT:
            break;
    }

}

void math_display_key_point(process_handle_t *handle,
                            uint8_t select_value,
                            SELECT_FLAG_T select_flag,
                            value_pos_aggregate_t *key_point_aggregate,
                            float display_level)
{
    int32_t count = 0, point_num = 0;
    uint8_t key_value = 0;

    for (point_num = 0; point_num < key_point_aggregate->num_pos; point_num ++)
    {
        switch (select_flag)
        {
            case Y_SELECT:
                key_value = key_point_aggregate->key_point[point_num].value * display_level;
                for (count = key_point_aggregate->key_point[point_num].pos_x;
                     count < handle->x_max;
                     count ++)
                {
                    if (handle->adc_raw_value[count][select_value] < key_value)
                        handle->adc_raw_value[count][select_value] = 0;
                }

                for (count = key_point_aggregate->key_point[point_num].pos_x;
                     count >= 0;
                     count --)
                {
                    if (handle->adc_raw_value[count][select_value] < key_value)
                        handle->adc_raw_value[count][select_value] = 0;
                }

                break;

            case X_SELECT:
                break;

            case ALL_SELECT:
                break;
        }
    }
   
}

