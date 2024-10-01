/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "sw3526.h"
#include <adc_read.h>
#include "esp_http_client_example.h"
#include "menu.h"
#include "mqtt.h"

#define PORT 8000
#define CONFIG_EXAMPLE_IPV4
static const char *TAG = "udp_server";
int flag = 0;
int sock;
 struct sockaddr_storage source_addr;
 char city[50];
 extern char appid[20];
 extern char appsecret[20];
 extern int16_t rgbProportion[3];
 extern int16_t light;
 extern myWeather mWeather;
 extern uint8_t Humi,Humi_small,Temp,Temp_small;

/************************************************
* 把字符串进行URL编码。
* 输入：
* 	str: 要编码的字符串
* 	strSize: 字符串的长度。这样str中可以是二进制数据
* 	result: 结果缓冲区的地址
* 	resultSize:结果地址的缓冲区大小(如果str所有字符都编码，该值为strSize*3)
* 返回值：
* 	>0: result中实际有效的字符长度，
* 	0: 编码失败，原因是结果缓冲区result的长度太小
************************************************/
int URLEncode(const char* str, const int strSize, char* result, const int resultSize) 
{
	int i;
	int j = 0; /* for result index */
	char ch;

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0)) 
	{
		return 0;
	}

	for (i=0; (i<strSize) && (j<resultSize); i++) 
	{
		ch = str[i];
		if ((ch >= 'A') && (ch <= 'Z')) {
			result[j++] = ch;
		} else if ((ch >= 'a') && (ch <= 'z')) {
			result[j++] = ch;
		} else if ((ch >= '0') && (ch <= '9')) {
			result[j++] = ch;
		} else if(ch == ' '){
			result[j++] = '+';
		} else {
			if (j + 3 < resultSize) {
				sprintf(result+j, "%%%02X", (unsigned char)ch);
				j += 3;
			} else {
				return 0;
			}
		}
	}
    if(j >= 0)
	    result[j] = '\0';
	return j;
}

void udp_txvi(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(300);
        if(flag == 1)
        {
            char tx_buffer[40];
            sprintf(tx_buffer, "%.3f#%.3f#%.3f#%.3f#%.3f#%.3f", ((double)sw35xx_c1.OutVol * 6) / 1000, ((double)sw35xx_c1.OutCur * 25 / 10) / 1000, \
                                                        ((double)sw35xx_c2.OutVol * 6) / 1000, ((double)sw35xx_c2.OutCur * 25 / 10) / 1000, \
                                                        ((double)ADC[0]) / 1000, ((double)ADC[1]) / 1000);
            int txLen = strlen(tx_buffer);
            int err = sendto(sock, tx_buffer, txLen, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            }
            else{
                ESP_LOGI(TAG, "SEND SUCCESS");
            }
        }
    }
}

void udp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1) {

        if (addr_family == AF_INET) {
            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(PORT);
            ip_protocol = IPPROTO_IP;
        } else if (addr_family == AF_INET6) {
            bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
            dest_addr.sin6_family = AF_INET6;
            dest_addr.sin6_port = htons(PORT);
            ip_protocol = IPPROTO_IPV6;
        }

        sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");
        flag = 1;

#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
        if (addr_family == AF_INET6) {
            // Note that by default IPV6 binds to both protocols, it is must be disabled
            // if both protocols used at the same time (used in CI)
            int opt = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
        }
#endif

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        while (1) {

            ESP_LOGI(TAG, "Waiting for data");
            // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
                char urlCity[20];
                if(cutString("city", urlCity, rx_buffer) == 0)
                {
                    int re = URLEncode(urlCity, strlen(urlCity), city, sizeof(city));
                    
                    ESP_LOGI("CITY", "CITY = %s", city);
                    
                    nvsWriteCity(city);
                }
                
                if(cutString("appid", appid, rx_buffer) == 0)
                {
                    
                    ESP_LOGI("appid", "appid = %s", appid);
                    
                    nvsWriteAppid(appid);
                }

                if(cutString("appsecret", appsecret, rx_buffer) == 0)
                {
                    printf("appsecret de length = %d", strlen(appsecret));
                    ESP_LOGI("appsecret", "appsecret = %s", appsecret);
                    appsecret[strlen(appsecret)] = '\0';
                    
                    nvsWriteAppsecret(appsecret);
                }


                char rgb[5];
                if(cutString("rgb[0]", rgb, rx_buffer) == 0)
                {
                    rgbProportion[0] = (rgb[0] - '0') * 100 +  (rgb[1] - '0') * 10 + rgb[2] - '0';
                    
                }
                if(cutString("rgb[1]", rgb, rx_buffer) == 0)
                {
                    rgbProportion[1] = (rgb[0] - '0') * 100 +  (rgb[1] - '0') * 10 + rgb[2] - '0';
            
                }
                if(cutString("rgb[2]", rgb, rx_buffer) == 0)
                {
                    rgbProportion[2] = (rgb[0] - '0') * 100 +  (rgb[1] - '0') * 10 + rgb[2] - '0';
        
                }
                if(cutString("light", rgb, rx_buffer) == 0)
                {
                    light = (rgb[0] - '0') * 100 +  (rgb[1] - '0') * 10 + rgb[2] - '0';
                    
                }
                nvsWrite();

                char tx_buffer[40];
                sprintf(tx_buffer, "\nC1 %6.3fW %6.3fV %.3fA \nC2 %6.3fW %6.3fV %.3fA\nA1 %6.3fV  A2 %6.3fV\nVIN %6.3fV\n%3d %3d %3d %3d %2d %2d %2d %2d %d %d %d %4.1f", ((double)sw35xx_c1.OutVol * 6) / 1000 * ((double)sw35xx_c1.OutCur * 25 / 10) / 1000,((double)sw35xx_c1.OutVol * 6) / 1000, ((double)sw35xx_c1.OutCur * 25 / 10) / 1000, \
                                                            ((double)sw35xx_c2.OutVol * 6) / 1000 * ((double)sw35xx_c2.OutCur * 25 / 10) / 1000, ((double)sw35xx_c2.OutVol * 6) / 1000, ((double)sw35xx_c2.OutCur * 25 / 10) / 1000, \
                                                            ((double)ADC[0]) / 1000, ((double)ADC[1]) / 1000, ((double)sw35xx_c1.InVol) / 100, light, rgbProportion[0], rgbProportion[1], rgbProportion[2], sw35xx_c1.protocol, sw35xx_c1.pdversion, sw35xx_c2.protocol, sw35xx_c2.pdversion, sw35xx_c1.state.tem_alarm_upmax, sw35xx_c2.state.tem_alarm_upmax, \
                                                            Humi,Temp + 0.1 * Temp_small);
                printf("c1 tem: %6f\n", sw35xx_c1.tem);
                 printf("c2 tem: %6f\n", sw35xx_c2.tem);
                int txLen = strlen(tx_buffer);
                int err = sendto(sock, tx_buffer, txLen, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));

                //sendto(sock, tiaoshi, strlen(sendudp), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                }
                else{
                    ESP_LOGI(TAG, "SEND SUCCESS");
                }
                
            }

            
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            flag = 0;
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

