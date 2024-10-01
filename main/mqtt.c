/* MQTT (over TCP) Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
 
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
 
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
 
#include "esp_log.h"
#include "mqtt_client.h"
#include "os.h"
#include "sw3526.h"
#include "adc_read.h"
#include "mqtt.h"
#include "esp_http_client_example.h"

 
static const char *TAG = "MQTT_EXAMPLE";

extern uint8_t Humi,Humi_small,Temp,Temp_small;
int16_t boardMode;
 
/*！！！！！！如下信息请替换成自己在阿里云的信息！！！！！！*/
/*由阿里网平台可得如下信息*/
// ProductKey:"gu1apipmzjV"; 
// DeviceName:"dev-esp32c3"; 
// DeviceSecret:"4147b310a3798d203acf81b5855bb38a";
// Region:"cn-shanghai";    
 
 
/*下面一参数为自定义*/
// ClientID:="112233";       
 
/*Broker Address��${YourProductKey}.iot-as-mqtt.${YourRegionId}.aliyuncs.com*/
#define   Aliyun_host       "a1fTxgHg0Z0.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define   Aliyun_port       1883
/*Client ID��     ${ClientID}|securemode=${Mode},signmethod=${SignMethod}|*/
#define   Aliyun_client_id  "a1fTxgHg0Z0.charger|securemode=2,signmethod=hmacsha256,timestamp=1657637794471|"
/*User Name��     ${DeviceName}&${ProductKey}*/
#define   Aliyun_username   "charger&a1fTxgHg0Z0"
/*ʹ�ù��� MQTT_Password ��������*/
#define   Aliyun_password   "5d0b774f5433a10b6a1cc5f2e40dc588bbcc5156b4a6656531959afb810bfa83"
 
#define   AliyunSubscribeTopic_user_get     "/a1fTxgHg0Z0/charger/user/get"
#define   AliyunPublishTopic_user_update    "/a1fTxgHg0Z0/charger/user/update"
#define   AliyunSet "/sys/a1fTxgHg0Z0/charger/thing/service/property/set"
#define   AliyunPost "/sys/a1fTxgHg0Z0/charger/thing/event/property/post"
 
char mqtt_publish_data1[] = "mqtt connect ok ";
char mqtt_publish_data2[] = "mqtt subscribe successful";
char mqtt_publish_data3[] = "mqtt i am esp32c3 charger";
 
esp_mqtt_client_handle_t client;
 
static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int  msg_id;
	
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, AliyunPublishTopic_user_update, mqtt_publish_data1, strlen(mqtt_publish_data1), 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
 
            msg_id = esp_mqtt_client_subscribe(client, AliyunSubscribeTopic_user_get, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            msg_id = esp_mqtt_client_subscribe(client, AliyunSet, 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
 
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
 
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, AliyunPublishTopic_user_update, mqtt_publish_data2, strlen(mqtt_publish_data2), 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            int C1PowerSwitch = cutNum("C1PowerSwitch", event->data);
            if(C1PowerSwitch != -1)
            {
                sprintf(sendudp, "\n%d     TOPIC=%.*s    DATA=%.*s    ",C1PowerSwitch, event->topic_len, event->topic, event->data_len, event->data);
            }
			
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}
 
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) 
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}
 
static void mqtt_test_task(void *pvParameters)
{
    uint8_t num = 0;
    esp_mqtt_client_publish(client, AliyunPublishTopic_user_update, mqtt_publish_data3, strlen(mqtt_publish_data3), 1, 0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    char json[200];
    uint8_t macAddress[8];
    esp_read_mac(macAddress, ESP_MAC_WIFI_STA);
  
         sprintf(json, "{\"params\":{\"ZB_PAN_ID\":%02d%02d%02d%02d%02d%02d%02d%02d}}", macAddress[0] % 100, macAddress[1] % 100,
          macAddress[2] % 100, macAddress[3] % 100, macAddress[4] % 100, macAddress[5] % 100, macAddress[6] % 100, macAddress[7] % 100
                        
                        );
	   esp_mqtt_client_publish(client, AliyunPost, json, strlen(json), 1, 0);

    vTaskDelete(NULL);
}
 
 
void user_mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
		.host = Aliyun_host,
		.port = Aliyun_port,
		.client_id = Aliyun_client_id,
		.username = Aliyun_username,
		.password = Aliyun_password,
 
    };
 
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
	

	xTaskCreate(&mqtt_test_task, "mqtt_test_task", 4096, NULL, 5, NULL);
}
 