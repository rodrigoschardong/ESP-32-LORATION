#include "driver/ledc.h"
#include "driver/gpio.h" 

#include "esp_log.h"
#include "esp_err.h"

#define TAG "PWM"


ledc_channel_config_t ledc_channel0 = {
        .channel = LEDC_CHANNEL_1,
        .duty = 1056,
        .gpio_num = 18,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_1,
    };

void PWMConfig(){
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_11_BIT,
        .freq_hz = 1,
        .timer_num = LEDC_TIMER_1,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;

    ledc_timer_config(&ledc_timer);

    //ledc_channel0.intr_type  = LEDC_INTR_DISABLE;
    ledc_channel_config(&ledc_channel0); 

    ledc_fade_func_install(0);
}

void PWMReConfig(uint8_t freq){
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_11_BIT,
        .freq_hz = freq,
        .timer_num = LEDC_TIMER_1,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;

    ledc_timer_config(&ledc_timer);

    ledc_channel_config(&ledc_channel0); 

    ledc_fade_func_install(0);
}

void PWMSetDutyWithFade(uint32_t duty, uint16_t fadeTime){
    ledc_set_fade_with_time(ledc_channel0.speed_mode, ledc_channel0.channel, duty, fadeTime);
    ledc_fade_start(ledc_channel0.speed_mode, ledc_channel0.channel, LEDC_FADE_NO_WAIT);
}

void PWMSetDuty(uint16_t duty){    
    ledc_set_duty(ledc_channel0.speed_mode, ledc_channel0.channel, duty);
    ledc_update_duty(ledc_channel0.speed_mode, ledc_channel0.channel);
}

