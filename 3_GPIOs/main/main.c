/* 
* Rodrigo Schardong
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "idf.h"
#include "main.h"

#define BLUE_LED    02
#define BUTTON      15

#define TAG "MAIN"

bool Timeout_Expired(uint64_t start, uint64_t time_us){
    uint64_t time_now = get_time();
    if(time_now < start){
        uint64_t delta_time = (UINT64_MAX - start) + time_now;
        return delta_time >= time_us;
    }
    uint64_t delta_time = time_now - start;
    return delta_time >= time_us;
}

/**
 * @brief  
 * @note   
 * @param  *interrupt_flag: 
 * @retval None
 */
void Run_States(uint8_t *interrupt_flag){
    #define MAX_STATE   3
    #define DELAY_US    500000

    bool level = false;
    bool blinking = false;
    uint64_t start = 0;
    *interrupt_flag = 0;  // 0 Led Desligado // 1 Led Ligado // 2 Led Piscando
    Set_Pin_Level(BLUE_LED, level);
    while(1){
        if(*interrupt_flag > MAX_STATE){
            *interrupt_flag = 0;
            blinking = false;
        }
        switch (*interrupt_flag)
        {
        case 0:
            if(level != false){
                level = false;
                Set_Pin_Level(BLUE_LED, level);
            }
            break;
        case 1:
            if(level != true){
                level = true;
                Set_Pin_Level(BLUE_LED, level);
            }
            break;
        case 2:
            if(!blinking){
                start = get_time();
                blinking = true;
                level = !level;
                Set_Pin_Level(BLUE_LED, level);
            }
            if(Timeout_Expired(start, DELAY_US)){
                blinking = false;
            }            
            break;
        default:
            break;
        }
        delay(10);
    }
}

void app_main(void)
{
    uint8_t interrupt_flag = 0;
    Pin_As_Output(BLUE_LED);
    Pin_As_IRAM_Input(BUTTON, &interrupt_flag);

    Run_States(&interrupt_flag);
}
