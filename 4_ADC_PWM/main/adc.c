//$include "freertos/FreeRTOS.h"
//#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "esp_log.h"
#include "esp_err.h"

#define TAG "ADC"

esp_err_t ADC_Setup(adc_bits_width_t adc_width_bit, adc_channel_t adc_channel){
    esp_err_t err;
    err = adc1_config_width(adc_width_bit);
    if(err == ESP_OK){
        err =adc1_config_channel_atten(adc_channel, ADC_ATTEN_0db);
    }
    return err;
}

esp_err_t ADC_Read(int *data_address, adc_channel_t adc_channel, uint8_t num_of_samples){
    if(num_of_samples == 0){
        ESP_LOGE(TAG, "Num of samples = 0");
        return ESP_FAIL;
    }
    uint16_t adc_reading = 0;
    uint8_t i;
    
    for(i = 0 ; i < num_of_samples; i++){
        adc_reading += adc1_get_raw(adc_channel);
    }
    adc_reading = adc_reading / num_of_samples;
    *data_address = adc_reading;
    ESP_LOGI(TAG, "read = %u", adc_reading);
    return ESP_OK;
}
