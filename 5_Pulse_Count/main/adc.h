#include "esp_err.h"
#include "driver/adc.h"

esp_err_t ADC_Setup(adc_bits_width_t adc_width_bit, adc_channel_t adc_channel);
esp_err_t ADC_Read(uint8_t *data_address, adc_channel_t adc_channel, uint8_t num_of_samples);