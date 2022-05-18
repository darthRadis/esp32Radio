#include <string.h>
#include <sys/param.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include "udp_server.h"
#include "showtransf.h"
#include "radfalhasim.h"
#include "uart.h"

#define PORTA_BASE 1818
#define IP_BASE "192.168.162.151"
#define PORTA_ATACK 1881
#define IP_ATACK "192.168.162.175"

static const char *TAG = "UDP Server";

int udpBase;
struct sockaddr_in *dest_addr_send;
int udpAtack;
struct sockaddr_in *dest_addr_atack;

void udpSendConecta(){
  udpBase = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
  if(udpBase)
    ESP_LOGI(TAG, "Send udp OK");
  else
    ESP_LOGI(TAG, "Send udp FAILURE %d",udpBase);
  dest_addr_send=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
  dest_addr_send->sin_addr.s_addr = inet_addr(IP_BASE);
  dest_addr_send->sin_family = AF_INET;
  dest_addr_send->sin_port = htons(PORTA_BASE);

  udpAtack = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
  if(udpAtack)
    ESP_LOGI(TAG, "Atack udp OK");
  else
    ESP_LOGI(TAG, "Atack udp FAILURE %d",udpAtack);
  dest_addr_atack=(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
  dest_addr_atack->sin_addr.s_addr = inet_addr(IP_ATACK);
  dest_addr_atack->sin_family = AF_INET;
  dest_addr_atack->sin_port = htons(PORTA_ATACK);
}

void udpSendAtack(char* buffer,int tamanho){
    sendto(udpAtack,(const char*)buffer,tamanho,0,
        (struct sockaddr *)dest_addr_send, sizeof(struct sockaddr_in));
    showtransfInclui(3,buffer,tamanho);
    ESP_LOGI(TAG, "Send Sat to atack");
}

void udpSendBase(char* buffer,int tamanho){
  if(getRadfalhasim(40,tamanho)){
    sendto(udpBase,(const char*)buffer,tamanho,0,
        (struct sockaddr *)dest_addr_send, sizeof(struct sockaddr_in));
    showtransfInclui(1,buffer,tamanho);
    ESP_LOGI(TAG, "Send Sat to base");
  }
  else
    showtransfInclui(0,buffer,tamanho);
  udpSendAtack(buffer,tamanho);
}

void udp_server_task(void *pvParameters)
{
  if ((int)pvParameters == AF_INET) {
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(PORTA_BASE);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock > 0) {
      ESP_LOGI(TAG, "Socket created");

      int err = bind(sock, (struct sockaddr *)dest_addr_ip4, sizeof(struct sockaddr_in6));
      if (err < 0) {
        ESP_LOGI(TAG, "Socket unable to bind: errno %d", errno);
      }
      else{
        ESP_LOGI(TAG, "Socket bound, port %d", PORTA_BASE);
      }

      int len;
      char rx_buffer[265];
      struct sockaddr_storage source_addr; 
      socklen_t socklen = sizeof(source_addr);
      while (1) {

        len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
        if (len > 0) {
          ESP_LOGI(TAG, "Leu %d", len);
          rx_buffer[len] = 0;
          if(getRadfalhasim(40,len)){
            showtransfInclui(2,rx_buffer,len);
            uartSend((unsigned char*)rx_buffer,len);
          }
          else
            showtransfInclui(0,rx_buffer,len);
          udpSendAtack(rx_buffer,len);
        }
      }

      if (sock != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
      }
                ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    }
  }
  vTaskDelete(NULL);
}

void init_udp_server(){
  udpBase=0;
  while(udpBase==0)
    udpSendConecta();
  uartInit();
  ESP_LOGI(TAG, "Send udp operando");
  xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
}
