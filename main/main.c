#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include "wifi_sta.h"
#include "udp_server.h"
#include "webserver.h"

void app_main() {
    // initialize nvs
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Inicializa conexão wifi
    wifi_init_sta();
    //Inicializa servidor web
    httpd_handle_t server;
    server=start_webserver();
    //Inicializa servidor udp
    init_udp_server();
}
