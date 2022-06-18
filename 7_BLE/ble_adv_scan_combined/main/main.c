#include <stdio.h>

#include "app_bt.h"
#include "gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

void app_main(void)
{
    ESP_LOGI("MAIN", "Hello World");
    ble_main();
    //gpio_main();
    //xTaskCreate(gpio_main, "gpio_main", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);
    xTaskCreate(gpio_main, "gpio_main_thread", configMINIMAL_STACK_SIZE *  3, NULL, 5, NULL);
}