/*************************************************************************
	> File Name: math_filter.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2022/9/8 17:34:35
 ************************************************************************/

#ifndef __MATH_FILTER_H__
#define __MATH_FILTER_H__

#include "74HC4051BQ.h"

typedef struct value_pos {
    uint8_t value;
    uint8_t pos_x;
    uint8_t pos_y;
} value_pos_t;

typedef struct value_pos_aggregate{
    uint8_t num_pos;
    value_pos_t key_point[10];
} value_pos_aggregate_t;

typedef enum {
    X_SELECT,
    Y_SELECT,
    ALL_SELECT
} SELECT_FLAG_T;

void math_get_max_value(process_handle_t *handle,
                        uint8_t select_value,
                        SELECT_FLAG_T select_flag,
                        value_pos_t *max_pos);

void math_clean_aggre_surplus(process_handle_t *handle,
                              SELECT_FLAG_T select_flag,
                              value_pos_t *max_pos);

#endif

