#include <esp_event.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_http_client.h>
#include <esp_netif.h>

#include "webserver.h"
#include "showtransf.h"
#include "radfalhasim.h"

static const char* TAG = "http_server";

esp_err_t http_event_handle_(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

esp_err_t relatorio_handler(httpd_req_t* req) {
  esp_err_t res = ESP_OK;
  ESP_LOGI(TAG, "Relatorio IN transfere");// %d",xTaskGetTickCount());
  size_t tamanho;
  char* resposta=showtransfMostra(&tamanho);
  if(resposta)
    httpd_resp_send(req, resposta, tamanho);
  ESP_LOGI(TAG, "Relatorio OUT transfere");// %d",xTaskGetTickCount());
  return res;
}

static const httpd_uri_t relatorio_uri = {
    .uri       = "/relatorio",
    .method    = HTTP_GET,
    .handler   = relatorio_handler
};

static void connect_event_handler(void* arg, esp_event_base_t event_base, 
                                  int32_t event_id, void* event_data) {
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

static void disconnect_event_handler(void* arg, esp_event_base_t event_base, 
                                     int32_t event_id, void* event_data) {
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

httpd_handle_t start_webserver() {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "http socket %d e uri %d", config.max_open_sockets, config.max_uri_handlers);

    ESP_LOGI(TAG, "starting server on port %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_ERROR_CHECK(esp_event_handler_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP,
            &connect_event_handler, &server));
        ESP_ERROR_CHECK(esp_event_handler_register(
            WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED,
            &disconnect_event_handler, &server));
        httpd_register_uri_handler(server, &relatorio_uri);
        return server;
    }
    ESP_LOGI(TAG, "error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server) {
    httpd_stop(server);
}
