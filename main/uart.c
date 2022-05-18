#include <esp_log.h>
#include <driver/uart.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#include <soc/rtc_wdt.h>

#include "udp_server.h"

static const char *TAG = "UART";

void uartLe(void* parametros){
  uint8_t data[264];
  size_t length=0;
  size_t lido;
  while(1){
    while(length==0){
      uart_get_buffered_data_len(UART_NUM_2, (size_t*)&length);
      esp_task_wdt_reset();
      rtc_wdt_feed();
    }
    lido=0;
    while(length){
      lido = uart_read_bytes(UART_NUM_2, data, length, 100);
      if(lido>0){
        udpSendBase((char*)data,lido);
        length-=lido;
      }
    }
  }
}

int uartInit(){
  uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };

  uart_set_pin(UART_NUM_2,17,16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
  uart_driver_install(UART_NUM_2, 2048, 0, 0, NULL, 0);
  xTaskCreate(uartLe, "leUART", 4096, NULL, tskIDLE_PRIORITY, NULL);
  return 0;
}

int uartSend(unsigned char* msg,size_t tamMsg){
  if(uart_write_bytes(UART_NUM_2,msg,tamMsg)>0)
    ESP_LOGI(TAG, "Send to UART");
  else
    ESP_LOGI(TAG, "Send No UART");
  return 0;
}
