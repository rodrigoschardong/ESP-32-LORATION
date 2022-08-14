/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//==================================================================
//ADC
#include "driver/adc.h"
#include "adc.h"
#define ADC_WIDTH       ADC_WIDTH_BIT_11
#define ADC_CHANNEL     ADC_CHANNEL_6       //gpio 34
#define ADC_NUM_SAMPLES 10
uint16_t pot_value;

//PWM
#include "driver/gpio.h"
#include "pwm.h"
#define MotorDelayTimer 250
uint16_t maxDuty;
uint16_t minDuty;
bool motorStatus = false;


void app_main(void)
{
    esp_err_t err;
    //ADC Setup

    err = ADC_Setup(ADC_WIDTH, ADC_CHANNEL);

    //Motor Setup
    PWMConfig();

    uint16_t lastPotValue = 0;
    while(1){
        err = ADC_Read(&pot_value, ADC_CHANNEL, ADC_NUM_SAMPLES);
        if(pot_value != lastPotValue){
            PWMSetDutyWithFade(pot_value, MotorDelayTimer);
            lastPotValue = pot_value;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }


}
