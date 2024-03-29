/* Finding Partitions Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <assert.h>
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"

#define SSID_MAX_LEN 32

static const char *TAG = "example";

typedef struct{
    char ssid[SSID_MAX_LEN];
    char pass[SSID_MAX_LEN];

} wifi_settings_t;

void clean_string(char *str) {
    char *cleaned = malloc(strlen(str) + 1); // allocate a buffer to hold the cleaned string
    if (!cleaned) {
        // error handling if malloc fails
        return;
    }
    
    int i = 0, j = 0;
    while (str[i] != '\0') {
        if (str[i] < 128) { // check if the character is an ASCII character
            cleaned[j++] = str[i];
        }
        i++;
    }
    cleaned[j] = '\0'; // terminate the cleaned string
    
    strcpy(str, cleaned); // copy the cleaned string back to the original string
    free(cleaned); // free the temporary buffer
}


esp_err_t Write_Data_Flash(wifi_settings_t data, char* label){
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    size_t data_len = sizeof(wifi_settings_t);

    // Prepare data to be read later using the mapped address
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, &data, data_len));
    
    return ESP_OK;
}

esp_err_t Read_Data_Flash(wifi_settings_t* data, char* label){
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    const void *map_ptr;
    spi_flash_mmap_handle_t map_handle;

    // Map the partition to data memory
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle));
    ESP_LOGI(TAG, "Mapped partition to data memory address %p", map_ptr);

    // Read back the written verification data using the mapped memory pointer
    memcpy(data, map_ptr, sizeof(wifi_settings_t));
    clean_string(data->ssid);
    clean_string(data->pass);
    
    spi_flash_munmap(map_handle);
    ESP_LOGI(TAG, "Unmapped partition from data memory");

    return ESP_OK;
}


void app_main(void)
{
    /*
    * This example uses the partition table from ../partitions_example.csv. For reference, its contents are as follows:
    *
    *  nvs,        data, nvs,      0x9000,  0x6000,
    *  phy_init,   data, phy,      0xf000,  0x1000,
    *  factory,    app,  factory,  0x10000, 1M,
    *  storage,    data, ,             , 0x40000,
    */

    // Find the partition map in the partition table
    
    char label[] = "wifi_settings";

    //static const char SSID[SSID_MAX_LEN] = "MY_SSID_EXAMPLE";

    const wifi_settings_t wifi_settings = {
        .ssid = "MY_SSID_EXAMPLE",
        .pass = "MY_PASS_EXAMPLE"
    };
    wifi_settings_t mem_wifi_settings = {
        .ssid = "",
        .pass = ""
    };
    
    ESP_ERROR_CHECK(Write_Data_Flash(wifi_settings, label));
    ESP_LOGI(TAG, "Written sample data to partition: %s", wifi_settings.ssid);
    ESP_LOGI(TAG, "Written sample data to partition: %s", wifi_settings.pass);

    //char memSSID[SSID_MAX_LEN];
    
    Read_Data_Flash(&mem_wifi_settings, label);
    ESP_LOGI(TAG, "Read sample data from partition using mapped memory: %s", (char*) mem_wifi_settings.ssid);
    ESP_LOGI(TAG, "Read sample data from partition using mapped memory: %s", (char*) mem_wifi_settings.pass);
    
    if(strcmp(wifi_settings.ssid, mem_wifi_settings.ssid) == 0)
        ESP_LOGI(TAG, "Data matches");
    else
        ESP_LOGI(TAG, "Data dont't matches");
    // Unmap mapped memory
    //spi_flash_munmap(map_handle);
    

    ESP_LOGI(TAG, "Example end");
}