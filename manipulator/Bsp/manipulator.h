/*************************************************************************
	> File Name: manipulator.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2023/8/4 11:09:20
 ************************************************************************/

#ifndef __MANIPULATOR_H__
#define __MANIPULATOR_H__

void manipulator_init(void);
void manipulator_pos_ctrl(uint16_t pos);
void manipulator_moment_ctrl(uint16_t moment);
void manipulator_pos_spd_ctrl(uint16_t pos_spd);
float manipulator_spd_ctrl(float speed, bool clean_flag);
int8_t manipulator_get_init_states(void *usb_handle);

#endif

