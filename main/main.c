/*
 * @Author: [LiaoZhelin]
 * @Date: 2022-04-03 10:05:08
 * @LastEditors: [Zyilin98]
 * @LastEditTime: 2025-01-03 18:07:43
 * @Description:
 */
// System:
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_event.h"
#include "nvs.h"
// User:
#include "wifi.h"
#include "adc_read.h"
#include "lis3dh.h"
#include "led_strip.h"
#include "sw3526.h"
#include "led_strip.h"
#include "driver/rmt.h"
#include <driver/gpio.h>
#include "menu.h"
#include "u8g2_esp32_hal.h"
#include "ota.h"
#include "sntptime.h"
#include "udp_server.h"


#include "task.h"
#include "http_client.h"
// #include "mqtt.h"

static const char *TAG = "main";
#define RMT_TX_CHANNEL RMT_CHANNEL_0
extern int16_t rgbProportion[3];
extern char city[50];
extern char appid[20];
extern char appsecret[20];
int16_t light = 10;
int16_t boardMode = 0;

void nvsRead(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // Open
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) 
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else 
    {
        printf("Done\n");

        // Read
        printf("Reading restart counter from NVS ... ");
        char buf[20];
        for(int i = 0; i < 3; i++)
        {
          sprintf(buf, "rgb[%d]", i);
          err = nvs_get_i16(my_handle, buf, &rgbProportion[i]);
          switch (err) 
          {
              case ESP_OK:
                  
                  break;
              case ESP_ERR_NVS_NOT_FOUND:
                  rgbProportion[i] = 50;
                  break;
          }
        }

        err = nvs_get_i16(my_handle, "light", &light);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "light = %d", light);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                light = 10;

                break;
            default:
                ESP_LOGI(TAG, "ERROR light = %s", esp_err_to_name(err));
        }

         err = nvs_get_i16(my_handle, "boardMode", &boardMode);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "boardMode = %d", boardMode);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                boardMode = 0;

                break;
            default:
                ESP_LOGI(TAG, "ERROR boardMode = %s", esp_err_to_name(err));
        }


        err = nvs_get_i8(my_handle, "oledOffTimeBg", &oledOffTimeBegin);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "oledOffTimeBegin = %d", oledOffTimeBegin);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                oledOffTimeBegin = 23;

                break;
            default:
                ESP_LOGI(TAG, "ERROR oledOffTimeBegin = %s", esp_err_to_name(err));
        }

        err = nvs_get_i8(my_handle, "oledOffTimeEnd", &oledOffTimeEnd);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "oledOffTimeEnd = %d", oledOffTimeEnd);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                oledOffTimeEnd = 8;

                break;
            default:
                ESP_LOGI(TAG, "ERROR oledOffTimeEnd = %s", esp_err_to_name(err));
        }

        err = nvs_get_i8(my_handle, "aPortLed", &aPortLed);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "aPortLed = %d", aPortLed);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                aPortLed = 0;

                break;
            default:
                ESP_LOGI(TAG, "ERROR aPortLed = %s", esp_err_to_name(err));
        }

        err = nvs_get_i8(my_handle, "aggreLimit", &aggreLimit);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "aggreLimit = %d", aggreLimit);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                aggreLimit = 0;

                break;
            default:
                ESP_LOGI(TAG, "ERROR aggreLimit = %s", esp_err_to_name(err));
        }

        err = nvs_get_i8(my_handle, "displayInput", &displayInput);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "displayInput = %d", displayInput);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                displayInput = 0;

                break;
            default:
                ESP_LOGI(TAG, "ERROR displayInput = %s", esp_err_to_name(err));
        }
        

        size_t lencity = 50;
        err = nvs_get_str(my_handle, "city", city, &lencity);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "CITYGET = %s", city);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                city[0] = '\0';
                ESP_LOGI(TAG, "ERROR CITYGET = %s", city);
                break;
            default:
                ESP_LOGI(TAG, "ERROR CITYGET = %s", esp_err_to_name(err));
        }

        lencity = 20;
        err = nvs_get_str(my_handle, "appid", appid, &lencity);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "appidGET = %s", appid);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                appid[0] = '\0';
                ESP_LOGI(TAG, "ERROR appidGET = %s", appid);
                break;
            default:
                ESP_LOGI(TAG, "ERROR appidGET = %s", esp_err_to_name(err));
        }

        lencity = 20;
        err = nvs_get_str(my_handle, "appsecret", appsecret, &lencity);
        switch (err) 
        {
            case ESP_OK:
                ESP_LOGI(TAG, "appsecret = %s", appid);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                appsecret[0] = '\0';
                ESP_LOGI(TAG, "ERROR appsecret = %s", appsecret);
                break;
            default:
                ESP_LOGI(TAG, "ERROR appsecret = %s", esp_err_to_name(err));
        }

       

        nvs_close(my_handle);
    }
}



void GPIO_Init(void){
  //Input GPIO
  esp_rom_gpio_pad_select_gpio(8);
  gpio_set_direction(8, GPIO_MODE_INPUT);
  gpio_set_pull_mode(8,GPIO_PULLUP_ONLY);
  esp_rom_gpio_pad_select_gpio(9);
  gpio_set_direction(9, GPIO_MODE_INPUT);
  gpio_set_pull_mode(9,GPIO_PULLUP_ONLY);
  esp_rom_gpio_pad_select_gpio(10);
  gpio_set_direction(10, GPIO_MODE_INPUT);
  gpio_set_pull_mode(10,GPIO_PULLUP_ONLY);

  if(boardMode == 1)
  {
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = 1ULL<< 21;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
  }
  else
  {
    esp_rom_gpio_pad_select_gpio(20);
    gpio_set_direction(20, GPIO_MODE_OUTPUT);
    gpio_set_level(20, 1);
    
    //Output GPIO
    esp_rom_gpio_pad_select_gpio(21);
    gpio_set_direction(21, GPIO_MODE_OUTPUT);
    gpio_set_level(21, 1);
  }
}

void NVS_Init(void){
  esp_err_t ret;
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
}

void sntpTask(void *pvParameters)
{
  SNTP_Init();
  vTaskDelete(NULL);
}
void ALL_Init(void){
  //Initization
  uint8_t wifiMode = 0;
  
  nvs_handle_t wificonfig_handle;
  uint8_t s_WifiConfigVal;
  nvsRead();
  u8g2_Init();// Init I2C
  oledInitMessageTask(1,"");
  NVS_Init();
  ADC_Init();
  strip = led_strip_init(RMT_CHANNEL_0, GPIO_NUM_4, 4);
  GPIO_Init();
  
  uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
  if (event == U8X8_MSG_GPIO_MENU_SELECT)
  {
      wifiMode = 1;
  }
  oledInitMessageTask(1,"OK");
  vTaskDelay(pdMS_TO_TICKS(200));
  oledInitMessageTask(2,"");
  SW35xxInit();
  oledInitMessageTask(2,"OK");
  vTaskDelay(pdMS_TO_TICKS(200));
  oledInitMessageTask(3,"");
  LIS3DH_Init();// No init I2C
  oledInitMessageTask(3,"OK");
  vTaskDelay(pdMS_TO_TICKS(200));
  
  uint32_t i = 10000000 / 40;
  while(i--)
  {
    event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    if (event == U8X8_MSG_GPIO_MENU_SELECT)
    {
        wifiMode = 1;
        ESP_LOGI(TAG, "an jian jin lai l");
        break;
    }
  }
  oledInitMessageTask(4 + wifiMode,"");
  ESP_ERROR_CHECK(nvs_open("WifiConfigFlag",NVS_READWRITE,&wificonfig_handle));
  nvs_get_u8(wificonfig_handle,"WifiConfigFlag",&s_WifiConfigVal);
  
  if(s_WifiConfigVal == wifi_configed || wifiMode == 1){
    if(wifi_init_sta(wifiMode))
    {
      oledInitMessageTask(4 + wifiMode,"OK");
      vTaskDelay(pdMS_TO_TICKS(200));
      
      xTaskCreate(sntpTask, "sntpTask", 4096, NULL, 1, NULL);
      xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 4, NULL);
      xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 1, NULL);
    }
    else
    {
      oledInitMessageTask(4 + wifiMode,"ER");
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
    }
    else{
    //wifi_init_sta(0);
    oledInitMessageTask(4 + wifiMode,"NO");
    ESP_LOGI(TAG,"No Config Wifi");
    vTaskDelay(pdMS_TO_TICKS(2000));  
    nvs_close(wificonfig_handle);
  }
  //OTA_Init(); // Init Wifi
  SW35xxAggreLimitPPS(aggreLimit);
  
  //TaskCreate
  xTaskCreate(adcTask, "adcTask", 1024, NULL, 1, &adcTask_handle);
  xTaskCreate(sw35xxTask, "sw35xxTask", 1024, NULL, 5, &sw35xxTask_handle);
 // xTaskCreate(lis3dhTask, "lis3dhtask", 1024, NULL, 1, &lis3dhtask_handle);
  xTaskCreate(oledTask, "oledtask", 1024 * 3, NULL, 3, &oledTask_handle);
  xTaskCreate(ws28xxTask, "ws28xxTask", 1024, NULL, 2, &ws28xxTask_handle); 
  //xTaskCreate(ntpClockTask, "ntpClockTask", 1024, NULL, 0, &ntpTask_handle);
  //DEBUG STACK SIZE
  //xTaskCreate(taskMonitor,"taskMonitor",1024*2,NULL,1,NULL);
  
  

  //user_mqtt_app_start();
  if(boardMode == 1)
  {
     xTaskCreate(dht11Task, "dht11Task", 4096, NULL, 2, NULL);
     
  }

  

}

void app_main(void)
{
  ALL_Init();
  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
    //ota_process
  }
}
