#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> //DIR lib
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_err.h"
#include "driver/gpio.h"

#include "esp_spiffs.h" //Storage
#include "nvs_flash.h"
#include "esp_log.h"

#include "AccessPoint.h"
#include "Server.h"

#define TAG "Main"

#define WEB_MOUNT_POINT "www"

void NVS(void);
static void SPIFFS_Directory(char * path);
esp_err_t SPIFFS_Mount(char * path, char * label, int max_files)

void app_main(void)
{
    NVS();                                                        // Call the function to initialize non-volatile storage

    //Start AP
    wifi_ap_init();                                               // Call a function to initialize WiFi access point mode
    ESP_ERROR_CHECK(start_rest_server(WEB_MOUNT_POINT));          // Call a function to start a REST server on a specific mount point

    // Initialize SPIFFS
    #define MAX_FILES 6
    ESP_LOGI(TAG, "Initializing SPIFFS");                         // Log a message to indicate that SPIFFS is being initialized
    if (SPIFFS_Mount("/html", "storage", MAX_FILES) != ESP_OK){   // Call a function to mount a SPIFFS partition with a specified path, label and maximum number of files, and check if it was successful
      ESP_LOGE(TAG, "SPIFFS mount failed");                       // Log an error message if SPIFFS mount failed
      while(1) { vTaskDelay(1); }                                 // Stay in an infinite loop to avoid the program from proceeding further
    }
}

// This function initializes the Non-Volatile Storage (NVS) subsystem.
// Which is a type of memory that can store data even when the device is powered off.
void NVS(void){
    // Log an informational message to the console using the "ESP_LOGI" macro.
    ESP_LOGI(TAG, "Init NVS");

    // Initialize the NVS subsystem by calling the "nvs_flash_init" function.
    esp_err_t ret = nvs_flash_init();

    // If the initialization failed due to no free pages or a new version being found,
    // erase the NVS flash memory using the "nvs_flash_erase" function and attempt to initialize again.
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // Check the return value of the initialization function using the "ESP_ERROR_CHECK" macro.
    // If an error occurred during initialization, it will cause the program to abort and print an error message to the console.
    ESP_ERROR_CHECK(ret);

    // Log another informational message to the console indicating that NVS initialization is complete.
    ESP_LOGI(TAG, "NVS done\n");
}

// Logs information about the SPIFFS partition specified by the "path" argument to the console.
static void SPIFFS_Directory(char * path) {
    // Open the directory specified by the path argument
    DIR* dir = opendir(path);
    // Check that the directory was successfully opened
    assert(dir != NULL);
    // Loop through each file in the directory
    while (true) {
        // Read the next directory entry
        struct dirent*pe = readdir(dir);
        // If there are no more entries, break out of the loop
        if (!pe) break;
        // Log information about the file (name, inode number, type) using the ESP_LOGI function
        ESP_LOGI(TAG,"d_name=%s/%s d_ino=%d d_type=%x", path, pe->d_name,pe->d_ino, pe->d_type);
    }
    // Close the directory
    closedir(dir);
}

// Mounts the SPIFFS partition and initializes it for use in the program. 
esp_err_t SPIFFS_Mount(char * path, char * label, int max_files) {
    // Create a configuration structure for SPIFFS
	esp_vfs_spiffs_conf_t conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed =true
	};

	// Use the configuration to initialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	// Handle error cases if the mount operation fails
	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return ret;
	}

	// Get information about the SPIFFS partition
	size_t total = 0, used = 0;
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	// If the mount was successful, log the mount point and directory contents
	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "Mount %s to %s success", path, label);
		SPIFFS_Directory(path);
	}

	// Return the result of the mount operation
	return ret;
}
