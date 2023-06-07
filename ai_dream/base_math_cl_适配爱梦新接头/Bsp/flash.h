#ifndef __FLASH_h
#define __FLASH_h
#include "gd32e10x.h"
#include "string.h"
#include "systick.h"
#include "stdbool.h"


#define FMC_PAGE_SIZE           ((uint16_t)0x400U)
#define FMC_WRITE_START_ADDR    ((uint32_t)(0x0801FC00U - 0x400))
#define FMC_WRITE_END_ADDR      ((uint32_t)(0x0801FFFFU))

typedef struct
{
    float max[5];
}Config;

extern int load_config_params(void);
 
extern Config get_config_params(void);
 
extern int save_config_params(Config *params);

#endif
