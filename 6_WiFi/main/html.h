#include "esp_system.h"
#include "esp_http_server.h"

typedef struct {
	char str_value[4];
	long long_value;
} URL_t;

esp_err_t html_root_get_handler(httpd_req_t *req);