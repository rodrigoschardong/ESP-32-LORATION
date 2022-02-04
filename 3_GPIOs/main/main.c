/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
//#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/queue.h"
//#include "esp_system.h"
//#include "esp_spi_flash.h"

#include "driver/gpio.h" 
#include <esp_timer.h>

#include "esp_log.h"
#include "esp_err.h"

#define TAG "GPIO"
#define BLUE_LED    GPIO_NUM_2
#define BUTTON      GPIO_NUM_15

//uint8_t state = 0;  // 0 Led Desligado // 1 Led Ligado // 2 Led Piscando

void Pin_As_Output(uint8_t pinNum){
   gpio_set_direction(pinNum, GPIO_MODE_OUTPUT);
}

void Set_Pin_Level(uint8_t pinNum, bool level){
    gpio_set_level(pinNum, level);
}

void IRAM_ATTR Pin_ISR_Handler(void* *interrupt_flag){
    *interrupt_flag = *interrupt_flag + 1;
}

void Pin_As_IRAM_Input(uint8_t pinNum, uint8_t *interrupt_flag){
    gpio_config_t io_config = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<< pinNum),
        .intr_type = GPIO_INTR_POSEDGE,
        .pull_up_en = 1,
        .pull_down_en = 0
    };
    gpio_config(&io_config);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(pinNum, Pin_ISR_Handler, (void*) interrupt_flag);
}

bool Timeout_Expired(uint64_t start, uint64_t time_us){
    uint64_t time_now = esp_timer_get_time() - start;
    if(time_now >= time_us){
        return true;
    }
    else{
        return false;
    }
}

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
                ESP_LOGI(TAG, "Turning Led Off");
                level = false;
                Set_Pin_Level(BLUE_LED, level);
            }
            break;
        case 1:
            if(level != true){
                ESP_LOGI(TAG, "Turning Led On");
                level = true;
                Set_Pin_Level(BLUE_LED, level);
            }
            break;
        case 2:
            if(!blinking){
                ESP_LOGI(TAG, "Blinking Led");
                start = esp_timer_get_time();
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
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    uint8_t interrupt_flag = 0;
    ESP_LOGI(TAG, "Starting");
    Pin_As_Output(BLUE_LED);
    Pin_As_IRAM_Input(BUTTON, &interrupt_flag);

    Run_States(&interrupt_flag);
}
