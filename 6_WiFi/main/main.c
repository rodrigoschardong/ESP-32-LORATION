/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_err.h"
#include "driver/gpio.h"

#include "nvs_flash.h"
#include "esp_log.h"

#include "AccessPoint.h"
#include "Server.h"

#define TAG "Main"

#define WEB_MOUNT_POINT "www"
void NVS(void){
    //NVS é a sigla para armazenamento de armazenamento não volátil.
  ESP_LOGI(TAG, "Init NVS");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  ESP_LOGI(TAG, "NVS done\n");
}

void app_main(void)
{
    NVS();
    //Start AP
    wifi_ap_init();
    ESP_ERROR_CHECK(start_rest_server(WEB_MOUNT_POINT));
}

