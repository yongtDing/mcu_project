#include "flash.h"
#include <stdio.h>

__IO fmc_state_enum fmc_state = FMC_READY;

void fmc_program(uint32_t *data, uint32_t data_len)
{
     /* unlock the flash program/erase controller */
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    
    fmc_state = fmc_page_erase(FMC_WRITE_START_ADDR); 
    
    uint32_t Address = FMC_WRITE_START_ADDR;
    uint32_t len = 0;
    while((Address < FMC_WRITE_END_ADDR) && (fmc_state == FMC_READY) && len < data_len)
    {
        fmc_state = fmc_word_program(Address, *data);
        data++;
        Address += 4;
        len ++;
    }
     
    fmc_lock();
}

 
int load_config_params(uint8_t *params, uint16_t param_len)
{	
    uint32_t *ptr = (uint32_t*)FMC_WRITE_START_ADDR;													
    {
        memcpy((uint32_t *)&params, ptr, param_len);						 
        return 0;
    }
}
 
int save_config_params(uint8_t *params, uint16_t param_len)
{
    fmc_program((uint32_t *)params, param_len);
    return 0;
}
