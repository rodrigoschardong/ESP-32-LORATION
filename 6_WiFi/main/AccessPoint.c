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


#define TAG "Access Point"


#define WIFI_AP_MODE_SSID           "ESP32_AP"
#define WIFI_AP_MODE_PASS           "password"
#define WIFI_AP_MODE_MAX_STA_CONN   10
#define WIFI_AP_MODE_CHANNEL        1

void wifi_ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if(event_id == WIFI_EVENT_AP_START){
        ESP_LOGI(TAG, "PONTO DE ACESSO INICIADO.");
    } else if(event_id == WIFI_EVENT_AP_STACONNECTED){
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "ESTAÇÃO "MACSTR" ID DE ACESSO %d, SE CONECTOU.", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED){
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "ESTAÇÃO "MACSTR" ID DE ACESSO %d DESCONECTOU.", MAC2STR(event->mac), event->aid);
    } else if( event_id == WIFI_EVENT_AP_STOP){
        ESP_LOGI(TAG, "PONTO DE ACESSO DESLIGADO.");
    } else {
        ESP_LOGI(TAG, "Evento de WIFI desconhecido");
    }
}


void wifi_ap_init(void) {
    
    ESP_LOGI(TAG, "Starting Access Point...");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_ap_event_handler,
                                                        NULL,
                                                        NULL));
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
    if(strlen(WIFI_AP_MODE_PASS) == 0) {
        wifi_ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_LOGI(TAG, "Step 1...");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_LOGI(TAG, "Step 2...");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));
    ESP_LOGI(TAG, "Step 3...");
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Access Point Created\nSSID: %s\nPASSWORD: %s\nCANAL: %d\n",
        WIFI_AP_MODE_SSID, WIFI_AP_MODE_PASS, WIFI_AP_MODE_CHANNEL);
}