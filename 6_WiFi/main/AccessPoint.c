#include "esp_wifi.h"
#include <string.h>
#include "sdkconfig.h"  //Constantes e configurações da SDK para a placa específica.

#include "esp_log.h"
#include "lwip/err.h"

#include "freertos/task.h"  //Lib para criação de tasks.
#include "freertos/event_groups.h" //Lib para manipulação de grupos de evento do FreeRTOS.
#include "esp_event.h"  //Lib para criação de loops de eventos.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_mac.h"

// Define a tag for the logs
#define TAG "Access Point"

// Define constants for the access point (AP) configuration
#define WIFI_AP_MODE_SSID           "ESP32_AP"    // Name of the AP
#define WIFI_AP_MODE_PASS           "password"    // Password for the AP
#define WIFI_AP_MODE_MAX_STA_CONN   10            // Maximum number of stations that can connect to the AP
#define WIFI_AP_MODE_CHANNEL        1             // Wi-Fi channel to use for the AP

// Event handler for Wi-Fi AP events
void wifi_ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// Function to initialize the Wi-Fi AP
void wifi_ap_init(void) {
    ESP_LOGI(TAG, "Starting Access Point...");

    // Initialize the TCP/IP network interface
    ESP_ERROR_CHECK(esp_netif_init());

    // Create the default event loop and network interface for the Wi-Fi AP
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    // Initialize the Wi-Fi driver with default configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register the event handler for Wi-Fi events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_ap_event_handler,
                                                        NULL,
                                                        NULL));

    // Configure the Wi-Fi AP with the specified settings
    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = WIFI_AP_MODE_SSID,
            .ssid_len = strlen(WIFI_AP_MODE_SSID),
            .channel = WIFI_AP_MODE_CHANNEL,
            .password = WIFI_AP_MODE_PASS,
            .max_connection = WIFI_AP_MODE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    // If no password is set, use an open Wi-Fi network
    if(strlen(WIFI_AP_MODE_PASS) == 0) {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Set the Wi-Fi mode to AP and configure the AP with the specified settings
    ESP_LOGI(TAG, "Step 1...");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_LOGI(TAG, "Step 2...");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_LOGI(TAG, "Step 3...");
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Access Point Created\nSSID: %s\nPASSWORD: %s\nCANAL: %d\n",
        WIFI_AP_MODE_SSID, WIFI_AP_MODE_PASS, WIFI_AP_MODE_CHANNEL);
}


void wifi_ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    // Handle the event based on its ID
    if(event_id == WIFI_EVENT_AP_START){
        ESP_LOGI(TAG, "Access Point Started");
    } else if(event_id == WIFI_EVENT_AP_STACONNECTED){
        // A station connected to the AP
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" join, AID = %d, connected.", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED){
        // A station disconnected from the AP
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" join, AID = %d, disconnected.", MAC2STR(event->mac), event->aid);
    } else if( event_id == WIFI_EVENT_AP_STOP){
        ESP_LOGI(TAG, "Access Point Stop");
    } else {
        ESP_LOGI(TAG, "Unknow Event");
    }
}