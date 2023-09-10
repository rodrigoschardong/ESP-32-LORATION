#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_http_server.h>

#include "esp_spiffs.h" //Storage
#include "nvs_flash.h"
#include <dirent.h> //DIR lib

#define TAG "MAIN"

static esp_err_t Text2Html(httpd_req_t *req, char * filename) {
	ESP_LOGI(TAG, "Reading %s", filename);
	FILE* fhtml = fopen(filename, "r");
	if (fhtml == NULL) {
		ESP_LOGE(TAG, "fopen fail. [%s]", filename);
		return ESP_FAIL;
	} else {
		char line[128];
		while (fgets(line, sizeof(line), fhtml) != NULL) {
			size_t linelen = strlen(line);
			//remove EOL (CR or LF)
			for (int i=linelen;i>0;i--) {
				if (line[i-1] == 0x0a) {
					line[i-1] = 0;
				} else if (line[i-1] == 0x0d) {
					line[i-1] = 0;
				} else {
					break;
				}
			}
			ESP_LOGD(TAG, "line=[%s]", line);
			if (strlen(line) == 0) continue;
			esp_err_t ret = httpd_resp_sendstr_chunk(req, line);
			if (ret != ESP_OK) {
				ESP_LOGE(TAG, "httpd_resp_sendstr_chunk fail %d", ret);
			}
		}
		fclose(fhtml);
	}
	return ESP_OK;
}

/* Handler for the form submission */
esp_err_t form_submit_handler(httpd_req_t *req)
{
	#define BUFFER_LEN 2048
	ESP_LOGI(TAG, "Receiving Form Data");

    char buf[BUFFER_LEN];
    int ret;
	int remaining = req->content_len;

    /* Read form data */
    while (remaining > 0) {
		ESP_LOGI(TAG, "Reading Form Data");
        if ((ret = httpd_req_recv(req, buf, BUFFER_LEN)) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
			ESP_LOGE(TAG, "Reading NOT Form Data");
            return ESP_FAIL;
        }
		ESP_LOGI(TAG, "NEXT");
        remaining -= ret;
    }

	ESP_LOGI(TAG, "Form Data Received");

    /* Parse form data */
    /* Assuming the form data is in the format "ssid=XXX&wifi-password=XXX&username=XXX&account-password=XXX" */
    char ssid[32];
    char wifi_password[64];

    ret = httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
	ESP_LOGI(TAG, "ssid: %s", ssid);

    ret = httpd_query_key_value(buf, "wifi-password", wifi_password, sizeof(wifi_password));
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }
	ESP_LOGI(TAG, "password: %s", wifi_password);

    /* Turn off AP mode */
    esp_wifi_stop();

    /* Connect to Wi-Fi network */
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, wifi_password, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Wait for connection */
    // Add your own code here to handle the connection status

    /* Respond with a redirection to a new page */
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

/* Handler for serving the HTML page */
esp_err_t index_html_handler(httpd_req_t *req)
{
    //httpd_resp_set_type(req, "text/html");
    //httpd_resp_send(req, html_page, strlen(html_page));

	ESP_LOGI(TAG, "Starting");
	ESP_LOGI(TAG, "root_get_handler req->uri=[%s]", req->uri);

	/* Send index.html */
	Text2Html(req, "/html/form.html");

	/* Send empty chunk to signal HTTP response completion */
	httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

/* HTTP server configuration */
httpd_uri_t index_uri = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = index_html_handler,
    .user_ctx = NULL
};

httpd_uri_t form_submit_uri = {
    .uri      = "/form",
    .method   = HTTP_POST,
    .handler  = form_submit_handler,
    .user_ctx = "form_submit_uri"
};

/* Start the AP server and HTTP server */
void start_ap_server()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32_AP",
            .ssid_len = 0,
            .max_connection = 4,
            .password = "password",
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.stack_size = 20480;

	config.max_uri_handlers = 8; // Increase the maximum number of URI handlers

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &form_submit_uri);
    }
}

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


static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent*pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(TAG,"d_name=%s/%s d_ino=%d d_type=%x", path, pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}

esp_err_t SPIFFS_Mount(char * path, char * label, int max_files) {
	esp_vfs_spiffs_conf_t conf = {
		.base_path = path,
		.partition_label = label,
		.max_files = max_files,
		.format_if_mount_failed =true
	};

	// Use settings defined above toinitialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is anall-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		} else if (ret== ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
		}
		return ret;
	}

	size_t total = 0, used = 0;
	ret = esp_spiffs_info(conf.partition_label, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
	} else {
		ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
	}

	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "Mount %s to %s success", path, label);
		SPIFFS_Directory(path);
	}
	return ret;
}


void app_main()
{
	NVS();
    start_ap_server();
	// Initialize SPIFFS
    #define MAX_FILES 6
    ESP_LOGI(TAG, "Initializing SPIFFS");
    if (SPIFFS_Mount("/html", "storage", MAX_FILES) != ESP_OK){
      ESP_LOGE(TAG, "SPIFFS mount failed");
      while(1) { vTaskDelay(1); }
    }
}
