/*
 * @Author: [LiaoZhelin]
 * @Date: 2022-04-29 20:32:29
 * @LastEditors: [Zyilin98]
 * @LastEditTime: 2025-01-03 18:07:43
 * @Description:
 */
#include <time.h>
#include <sys/time.h>


#include <esp_http_client.h>


#include "menu.h"
#include "u8g2_esp32_hal.h"
#include "adc_read.h"
#include "sw3526.h"
#include "esp_log.h"

#include "esp_netif_ip_addr.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "wifi.h"
#include "esp_netif.h"
#include <tcpip_adapter.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "task.h"
#include "ota.h"
#include "wifi.h"
#include "http_client.h"
#define EXIT_MENU_CHECK \
  if (exit_flag)        \
  {                     \
    exit_flag = 0;      \
    break;              \
  }
#define EXIT_MENU_SET exit_flag = 1;
#define C1REVISE 1
#define C2REVISE 1
 uint8_t exit_flag = 0; 
static const char *TAG = "memu";
int16_t rgbProportion[3] = {0, 20, 0};
extern int16_t light;
//温湿度
 extern uint8_t Humi,Humi_small,Temp,Temp_small;
 //boardMode
 extern int boardMode;

// HTTP配置参数

#define MAX_HTTP_OUTPUT_BUFFER 2048
extern myWeather mWeather;
extern char local_response_buffer[2048];

int OledProtectEnd = 0;
time_t beginTime = 0;
char  oldVersion[20] = {'\0'};
#define TIME_TABLE "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23" 



void nvsWriteCity(char * city)
{
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
  


      err = nvs_set_str(my_handle, "city", city);
      if (err != ESP_OK) 
      {
          ESP_LOGI(TAG, "Error (%s)\n", esp_err_to_name(err));
      } 
      else{
          ESP_LOGI(TAG, "write city success\n");
      }



      err = nvs_commit(my_handle);

    }
    // Close
    nvs_close(my_handle);
}

void nvsWriteAppid(char * appid)
{
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
  


      err = nvs_set_str(my_handle, "appid", appid);
      if (err != ESP_OK) 
      {
          ESP_LOGI(TAG, "Error (%s)\n", esp_err_to_name(err));
      } 
      else{
          ESP_LOGI(TAG, "write appid success\n");
      }



      err = nvs_commit(my_handle);

    }
    // Close
    nvs_close(my_handle);
}

void nvsWriteAppsecret(char * appsecret)
{
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
  


      err = nvs_set_str(my_handle, "appsecret", appsecret);
      if (err != ESP_OK) 
      {
          ESP_LOGI(TAG, "Error (%s)\n", esp_err_to_name(err));
      } 
      else{
          ESP_LOGI(TAG, "write secret success\n");
      }



      err = nvs_commit(my_handle);

    }
    // Close
    nvs_close(my_handle);
}

void nvsWrite()
{
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
  
      char buf[16];
      for(int i = 0; i < 3; i++)
      {
        sprintf(buf, "rgb[%d]", i);
        err = nvs_set_i16(my_handle, buf, rgbProportion[i]);
        if (err != ESP_OK) 
        {
            ESP_LOGI(TAG, "Error (%s)\n", esp_err_to_name(err));
        } 
      }
      err = nvs_set_i16(my_handle, "light", light);



      err = nvs_commit(my_handle);

    }
    // Close
    nvs_close(my_handle);
}
void nvsWriteBoardMode(int mode)
{
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
      err = nvs_set_i16(my_handle, "boardMode", mode);

      err = nvs_commit(my_handle);
    }
     nvs_close(my_handle);
}

void nvsWriteAPortLed(uint8_t aPort)
{
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
      err = nvs_set_i8(my_handle, "aPortLed", aPort);

      err = nvs_commit(my_handle);
    }
     nvs_close(my_handle);
}

void nvsWriteAggreLimit()
{
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
      err = nvs_set_i8(my_handle, "aggreLimit", aggreLimit);

      err = nvs_commit(my_handle);
    }
     nvs_close(my_handle);
}

void nvsWritedisplayInput()
{
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
      err = nvs_set_i8(my_handle, "displayInput", displayInput);

      err = nvs_commit(my_handle);
    }
     nvs_close(my_handle);
}

void nvsWriteOledOffTimeBegin()
{
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
      err = nvs_set_i8(my_handle, "oledOffTimeBg", oledOffTimeBegin);
      err = nvs_commit(my_handle);
      
    }
     nvs_close(my_handle);
}

void nvsWriteOledOffTimeEnd()
{
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
      
      err = nvs_set_i8(my_handle, "oledOffTimeEnd", oledOffTimeEnd);

      err = nvs_commit(my_handle);
    }
     nvs_close(my_handle);
}

static void oledWifiShowTask(void)
{
  wifi_ap_record_t ap;
  tcpip_adapter_ip_info_t ipInfo;
  for (;;)
  {
    char buf[20] = {0};
    esp_wifi_sta_get_ap_info(&ap);
      esp_err_t err = esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ipInfo); 
   
    char ip_Str[16] = {0};
    esp_ip4addr_ntoa(&ipInfo.ip, ip_Str, sizeof(ip_Str));
    uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    u8g2_ClearBuffer(&u8g2);
    sprintf(buf, "SSID名称: %s", ap.ssid);
    u8g2_DrawUTF8(&u8g2, 0, 15, buf);
    sprintf(buf, "IP: %s", ip_Str);
    u8g2_DrawUTF8(&u8g2, 0, 31, buf);
    sprintf(buf, "RSSI信号强度: %d", ap.rssi);
    u8g2_DrawUTF8(&u8g2, 0, 47, buf);
    sprintf(buf, "Primary信道: %d", ap.primary);
    u8g2_DrawUTF8(&u8g2, 0, 63, buf);
    u8g2_SendBuffer(&u8g2);
    if (event == U8X8_MSG_GPIO_MENU_SELECT)
    {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void oledSmartConfigTask(void){
    
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawUTF8(&u8g2, 20, 16, "配网模式");
    u8g2_DrawUTF8(&u8g2, 0, 30, "关机重启");
    u8g2_DrawUTF8(&u8g2, 0, 46, "在开机时不停按中间键");
    u8g2_DrawUTF8(&u8g2, 0, 62, "即可用esptouch配网");
    u8g2_SendBuffer(&u8g2);
    while(u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2)) !=  U8X8_MSG_GPIO_MENU_SELECT);
    
}

static void  oledPowerLimitTask(void){
  uint8_t current_selection = 0;
  current_selection = u8g2_UserInterfaceMessage(&u8g2, "C口功率限制设定", "C1口功率:", "C1口功率:", "更新\n取消");
}

static void oledVIShowTask(void)
{
  reSetOledProtect();
  for (;;)
  {
    
    char buf[20] = {0};
    uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    if(OledProtectBegin == 0)
    {
     oledViAllShow();
    }
    else{
      oledStopDisplay();
    }
    time_t nowTime;
    if(OledProtectBegin == 0)
    {
      time(&nowTime);
      if(nowTime - beginTime >= 60)
      {
        OledProtectBegin = 1;
      }
      if (event == U8X8_MSG_GPIO_MENU_SELECT)
      {
        time(&beginTime);
        break;
      }
    }
    else
    {
       if(event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT || event == U8X8_MSG_GPIO_MENU_DOWN || event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP)
        {
          reSetOledProtect();
        }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void oledProtocolShowTask_1(void)
{
  uint16_t c1CurRevise = sw35xx_c1.OutCur * C1REVISE;
  char buf[20] = {0};
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawUTF8(&u8g2, 25, 16, "C1口");
  switch (sw35xx_c1.protocol)
  {
  case 0:
    u8g2_DrawUTF8(&u8g2, 2, 32, "无协议");
    break;
  case 1:
    u8g2_DrawUTF8(&u8g2, 2, 32, "QC 2.0");
    break;
  case 2:
    u8g2_DrawUTF8(&u8g2, 2, 32, "QC 3.0");
    break;
  case 3:
    u8g2_DrawUTF8(&u8g2, 2, 32, "FCP");
    break;
  case 4:
    u8g2_DrawUTF8(&u8g2, 2, 32, "SCP");
    break;
  case 5:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PD FIX");
    break;
  case 6:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PD PPS");
    break;
  case 7:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PE 1.1");
    break;
  case 8:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PE 2.0");
    break;
  case 9:
    u8g2_DrawUTF8(&u8g2, 2, 32, "VOOC");
    break;
  case 10:
    u8g2_DrawUTF8(&u8g2, 2, 32, "SFCP");
    break;
  case 11:
    u8g2_DrawUTF8(&u8g2, 2, 32, "AFC");
    break;
  }
  switch (sw35xx_c1.pdversion)
  {
  case 0:
    break;
  case 1:
    u8g2_DrawUTF8(&u8g2, 2, 48, "PD 2.0");
    break;
  case 2:
    u8g2_DrawUTF8(&u8g2, 2, 48, "PD 3.0");
    break;
  }
  u8g2_DrawLine(&u8g2, 72, 0, 72, 49);
  u8g2_DrawLine(&u8g2, 0, 49, 127, 49);
  u8g2_DrawUTF8(&u8g2, 75, 31, "降压");
  if (sw35xx_c1.state.buck_open)
    u8g2_DrawUTF8(&u8g2, 102, 31, "工作");
  else
    u8g2_DrawUTF8(&u8g2, 102, 31, "停止");
  sprintf(buf, "输出 %d.%03dV %d.%03dA", (sw35xx_c1.OutVol * 6) / 1000, (sw35xx_c1.OutVol * 6) % 1000, (c1CurRevise * 25 / 10) / 1000, (c1CurRevise * 25 / 10) % 1000);
  u8g2_DrawUTF8(&u8g2, 0, 64, buf);
  u8g2_SendBuffer(&u8g2);
}

static void oledProtocolShowTask_2(void)
{
  uint16_t c2CurRevise = sw35xx_c2.OutCur * C2REVISE;
  char buf[20] = {0};
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawUTF8(&u8g2, 25, 16, "C2口");
  switch (sw35xx_c2.protocol)
  {
  case 0:
    u8g2_DrawUTF8(&u8g2, 2, 32, "无协议");
    break;
  case 1:
    u8g2_DrawUTF8(&u8g2, 2, 32, "QC 2.0");
    break;
  case 2:
    u8g2_DrawUTF8(&u8g2, 2, 32, "QC 3.0");
    break;
  case 3:
    u8g2_DrawUTF8(&u8g2, 2, 32, "FCP");
    break;
  case 4:
    u8g2_DrawUTF8(&u8g2, 2, 32, "SCP");
    break;
  case 5:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PD FIX");
    break;
  case 6:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PD PPS");
    break;
  case 7:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PE 1.1");
    break;
  case 8:
    u8g2_DrawUTF8(&u8g2, 2, 32, "PE 2.0");
    break;
  case 9:
    u8g2_DrawUTF8(&u8g2, 2, 32, "VOOC");
    break;
  case 10:
    u8g2_DrawUTF8(&u8g2, 2, 32, "SFCP");
    break;
  case 11:
    u8g2_DrawUTF8(&u8g2, 2, 32, "AFC");
    break;
  }
  switch (sw35xx_c2.pdversion)
  {
  case 0:
    break;
  case 1:
    u8g2_DrawUTF8(&u8g2, 2, 48, "PD 2.0");
    break;
  case 2:
    u8g2_DrawUTF8(&u8g2, 2, 48, "PD 3.0");
    break;
  }
  u8g2_DrawLine(&u8g2, 72, 0, 72, 49);
  u8g2_DrawLine(&u8g2, 0, 49, 127, 49);
  u8g2_DrawUTF8(&u8g2, 75, 31, "降压");
  if (sw35xx_c2.state.buck_open)
    u8g2_DrawUTF8(&u8g2, 102, 31, "工作");
  else
    u8g2_DrawUTF8(&u8g2, 102, 31, "停止");
  sprintf(buf, "输出 %d.%03dV %d.%03dA", (sw35xx_c2.OutVol * 6) / 1000, (sw35xx_c2.OutVol * 6) % 1000, (c2CurRevise * 25 / 10) / 1000, (c2CurRevise * 25 / 10) % 1000);
  u8g2_DrawUTF8(&u8g2, 0, 64, buf);
  u8g2_SendBuffer(&u8g2);
}

void OTAFailDisplay()
{
    u8g2_ClearBuffer(&u8g2);
  
    u8g2_DrawUTF8(&u8g2, 0, 30, "OTA更新失败");
    u8g2_DrawUTF8(&u8g2, 0, 62, "按确认键退出");
    u8g2_SendBuffer(&u8g2);
    while(u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2)) !=  U8X8_MSG_GPIO_MENU_SELECT);
}

static void oledProtocolShowTask(void)
{
  uint8_t tasknum = 0;
  for (;;)
  {
    uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    if (event == U8X8_MSG_GPIO_MENU_SELECT)
    {
      break;
    }
    else if (event == U8X8_MSG_GPIO_MENU_NEXT || event == U8X8_MSG_GPIO_MENU_DOWN)
    {
      tasknum = (tasknum >= 1 ? 1 : tasknum + 1);
    }
    else if (event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP)
    {
      tasknum = (tasknum <= 0 ? 0 : tasknum - 1);
    }
    switch (tasknum)
    {
    case 0:
      oledProtocolShowTask_1();
      break;
    case 1:
      oledProtocolShowTask_2();
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void oledFaultShowTask(void)
{

  for (;;)
  {
    uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawUTF8(&u8g2, 0, 15, "C1异常代码");
    u8g2_DrawUTF8(&u8g2, 20, 31, "系统无故障");
    if (sw35xx_c1.state.val_input_upmax)
      u8g2_DrawUTF8(&u8g2, 20, 31, "输入电压过高！");
    if (sw35xx_c1.state.tem_alarm_upmax)
      u8g2_DrawUTF8(&u8g2, 20, 31, "芯片温度过高！");
    if (sw35xx_c1.state.tem_shutdown)
      u8g2_DrawUTF8(&u8g2, 20, 31, "高温关机保护！");
    if (sw35xx_c1.state.short_citcuit)
      u8g2_DrawUTF8(&u8g2, 20, 31, "输出短路！"); // 0x05版本的芯片貌似不能用
    u8g2_DrawUTF8(&u8g2, 0, 47, "C2异常代码");
    u8g2_DrawUTF8(&u8g2, 20, 63, "系统无故障");
    if (sw35xx_c2.state.val_input_upmax)
      u8g2_DrawUTF8(&u8g2, 20, 63, "输入电压过高！");
    if (sw35xx_c2.state.tem_alarm_upmax)
      u8g2_DrawUTF8(&u8g2, 20, 63, "芯片温度过高！");
    if (sw35xx_c2.state.tem_shutdown)
      u8g2_DrawUTF8(&u8g2, 20, 63, "高温关机保护！");
    if (sw35xx_c2.state.short_citcuit)
      u8g2_DrawUTF8(&u8g2, 20, 63, "输出短路！"); // 0x05版本的芯片貌似不能用
    u8g2_SendBuffer(&u8g2);
    if (event == U8X8_MSG_GPIO_MENU_SELECT)
    {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void getLastVersion(char *now, char *old)
{
  sprintf(old, "%s", now);
  if(old[4] == '0')
  {
    old[4] = '9';
    if(old[2] == '0')
    {
      old[2] = '9';
      if(old[0] > '0')
      {
        old[0] --;
      }
    }
    else
    {
      old[2]--;
    }
  }
  else
  {
    old[4] --;
  }
}

void fallBack(char * runVersion, char * oldVersion)
{
  char old1[20];
  char old2[20];
  char old3[20];
  char old4[20];
  char old5[20];
  getLastVersion(runVersion, old1);
  getLastVersion(old1, old2);
  getLastVersion(old2, old3);
  getLastVersion(old3, old4);
  getLastVersion(old4, old5);
  char dis[100];
  sprintf(dis, "%s\n%s\n%s\n%s\n%s\n<-返回",old1, old2, old3, old4, old5);
  uint8_t current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "版本选择", 1, dis);
 switch (current_selection)
 {
 case 1:
  sprintf(oldVersion, "%s", old1);
  break;
  case 2:
  sprintf(oldVersion, "%s", old2);
  break;
  case 3:
  sprintf(oldVersion, "%s", old3);
  break;
  case 4:
  sprintf(oldVersion, "%s", old4);
  break;
  case 5:
  sprintf(oldVersion, "%s", old5);
  break;
  case 6:
  return;
  break;
 default:
  break;
 }
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawUTF8(&u8g2, 15, 16, "设备将会自动更新");
  u8g2_DrawUTF8(&u8g2, 15, 32, "回退完成自动重启");
  u8g2_DrawUTF8(&u8g2, 15, 48, "请耐心等待.....");
  u8g2_SendBuffer(&u8g2);
  vTaskSuspend(adcTask_handle); //挂起其他任务
  vTaskSuspend(sw35xxTask_handle);
  vTaskSuspend(ws28xxTask_handle);
//    vTaskSuspend(lis3dhtask_handle);
  vTaskDelay(pdMS_TO_TICKS(1000));
  ota_set
  ota_process

  
}

static void oledManualOTATask(void)
{
  oldVersion[0] = '\0';
  uint8_t current_selection = 0;
  setCheckVersion();
  xTaskCreate(advanced_ota_example_task, "advanced_ota_example_task", 1024 * 8, NULL, 1, NULL);
  time_t oldTime;
  time_t nowTime;
  time(&oldTime);
  char countDown[40];
  while(getCheckVersion() == 1)
  {
    time(&nowTime);
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawUTF8(&u8g2, 2, 16, "正在检查服务器");
    u8g2_DrawUTF8(&u8g2, 2, 30, "是否连通");
    sprintf(countDown, "倒计时%d秒", 5 - (nowTime - oldTime));
    u8g2_DrawUTF8(&u8g2, 10, 46, countDown);  
    if(nowTime - oldTime >= 5)
    {
      u8g2_DrawUTF8(&u8g2, 10, 62, "连接失败！");
      u8g2_SendBuffer(&u8g2);
      uint8_t event;
      while(1)
      {
        event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
        if(event == U8X8_MSG_GPIO_MENU_SELECT)
        {
          return;
        }
      }
    }
    u8g2_SendBuffer(&u8g2);
  }
  char newVersion[10];
  char runVersion[10];
  getNewVersion(newVersion);
  getRunningVersion(runVersion);
  char new[20];
  char run[20];
 
  sprintf(new, "远端: V%s ", newVersion);
  sprintf(run, "当前: V%s ", runVersion);

  if(getIsEqu() == 1)
  {
    current_selection = u8g2_UserInterfaceMessage(&u8g2, run, new, "固件已经最新!", "退出\n回退上一版本");
    if(current_selection == 2)
    {
      fallBack(runVersion, oldVersion);
      return;
      vTaskDelete(NULL);
    }
  }
  else
  {
     current_selection = u8g2_UserInterfaceMessage(&u8g2, run, new, "有新固件!", "更新\n回退\n取消");
    if(current_selection == 1)
    {
      u8g2_ClearBuffer(&u8g2);
      u8g2_DrawUTF8(&u8g2, 15, 16, "设备将会自动更新");
      u8g2_DrawUTF8(&u8g2, 15, 32, "更新完成自动重启");
      u8g2_DrawUTF8(&u8g2, 15, 48, "请耐心等待.....");
      u8g2_SendBuffer(&u8g2);
      vTaskSuspend(adcTask_handle); //挂起其他任务
      vTaskSuspend(sw35xxTask_handle);
      vTaskSuspend(ws28xxTask_handle);
  //    vTaskSuspend(lis3dhtask_handle);
      vTaskDelay(pdMS_TO_TICKS(1000));
      ota_set
      ota_process
  
      vTaskDelete(NULL);
    }
    else if(current_selection == 2)
    {
      fallBack(runVersion, oldVersion);
      
      vTaskDelete(NULL);
    }
   

 
    //删除任务，等待复位
  }
}

static void oledAutoOTATask(void)
{
  
}

static void oledChargeSurface(void)
{
  uint16_t c1CurRevise = sw35xx_c1.OutCur * C1REVISE;
  uint16_t c2CurRevise = sw35xx_c2.OutCur * C2REVISE;
  char buf[20] = {0};
  u8g2_ClearBuffer(&u8g2);
  u8g2_DrawLine(&u8g2, 85, 6, 85, 28);
  u8g2_DrawLine(&u8g2, 84, 6, 84, 28);
  u8g2_DrawLine(&u8g2, 85, 40, 85, 62);
  u8g2_DrawLine(&u8g2, 84, 40, 84, 62);

  u8g2_DrawLine(&u8g2, 40, 27, 85, 27);
  u8g2_DrawLine(&u8g2, 40, 28, 85, 28);
  u8g2_DrawLine(&u8g2, 40, 61, 85, 61);
  u8g2_DrawLine(&u8g2, 40, 62, 85, 62);
  u8g2_DrawRFrame(&u8g2, 13, 1, 28, 28, 10);
  u8g2_DrawRFrame(&u8g2, 13, 35, 28, 28, 10);
  u8g2_SetFont(&u8g2, u8g2_font_helvB14_tr);
  sprintf(buf, "%2dW", (sw35xx_c1.OutVol * 6) * (c1CurRevise * 25 / 10) / 1000000);
  u8g2_DrawStr(&u8g2, 43, 22, buf);
  sprintf(buf, "%2dW", (sw35xx_c2.OutVol * 6) * (c2CurRevise * 25 / 10) / 1000000);
  u8g2_DrawStr(&u8g2, 43, 56, buf);
  u8g2_DrawStr(&u8g2, 15, 22, "C1");
  u8g2_DrawStr(&u8g2, 15, 56, "C2");
  u8g2_SetFont(&u8g2, u8g2_font_siji_t_6x10);

  if (sw35xx_c1.state.pro_fastcharge)
    u8g2_DrawGlyph(&u8g2, 0, 12, 0XE040);
  if (sw35xx_c1.state.buck_open)
    u8g2_DrawGlyph(&u8g2, 0, 27, 0XE040 + 3);

  if (sw35xx_c2.state.pro_fastcharge)
    u8g2_DrawGlyph(&u8g2, 0, 46, 0XE040);
  if (sw35xx_c2.state.buck_open)
    u8g2_DrawGlyph(&u8g2, 0, 61, 0XE040 + 3);

  u8g2_SetFont(&u8g2, u8g2_font_helvB10_tr);
  sprintf(buf, "%d.%02dV", (sw35xx_c1.OutVol * 6) / 1000, ((sw35xx_c1.OutVol * 6) % 1000) / 10);
  u8g2_DrawStr(&u8g2, 88, 16, buf);
  sprintf(buf, "%d.%02dA", (c1CurRevise * 25 / 10) / 1000, ((c1CurRevise * 25 / 10) % 1000) / 10);
  u8g2_DrawStr(&u8g2, 88, 32 - 1, buf);
  sprintf(buf, "%d.%02dV", (sw35xx_c2.OutVol * 6) / 1000, ((sw35xx_c2.OutVol * 6) % 1000) / 10);
  u8g2_DrawStr(&u8g2, 88, 48 + 1, buf);
  sprintf(buf, "%d.%02dA", (c2CurRevise * 25 / 10) / 1000, ((c2CurRevise * 25 / 10) % 1000) / 10);
  u8g2_DrawStr(&u8g2, 88, 64, buf);

  u8g2_SendBuffer(&u8g2);
}

static void oledWeatherSurface(void)
{
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
  u8g2_DrawUTF8(&u8g2, 40, 15, "天气预报");
  char str[200] = {'\0'};
  if(strlen(mWeather.city) == 0)
  {
    u8g2_DrawUTF8(&u8g2, 0, 42, "网络异常！");
    u8g2_SendBuffer(&u8g2);
    return;
  }
  sprintf(str, "%s %s Rh:%s", mWeather.city, mWeather.week, mWeather.humidity);
  u8g2_DrawUTF8(&u8g2, 0, 32, str);

  char str1[200] = {'\0'};
  strcat(str1, mWeather.wea);
  strcat(str1, " ");
  strcat(str1,mWeather.tem);
  strcat(str1, "℃");
  strcat(str1, " upd:");
  strcat(str1,mWeather.updateTime);
  u8g2_DrawUTF8(&u8g2, 0, 46, str1);

   char str2[200] = {'\0'};
  strcat(str2, "白天:");
  strcat(str2, mWeather.temDay);
  strcat(str2, "℃");
  strcat(str2, " 夜晚:");
  strcat(str2, mWeather.temNight);
  strcat(str2, "℃");
  u8g2_DrawUTF8(&u8g2, 0, 60, str2);
  
  u8g2_SendBuffer(&u8g2);
}


void oledStopDisplay()
{
  u8g2_uint_t x[] = {0, 10, 20, 30, 40, 50, 60, 70};
  u8g2_uint_t y[] = {10, 15, 21, 26, 32, 37, 43, 49};
  static time_t nowTimef;
  static time_t oldTimef = 0;
  static uint32_t num = 0;
  struct tm timeinfo;
  char buf[20];
  char mmm = 0;
  time(&nowTimef);
  
  localtime_r(&nowTimef, &timeinfo);

  if(oledOffTimeBegin > oledOffTimeEnd)
  {
    if(timeinfo.tm_hour >= oledOffTimeBegin || timeinfo.tm_hour < oledOffTimeEnd)
    {
      u8g2_ClearBuffer(&u8g2);
      mmm = 1;
    }
  }
  else
  {
      if(timeinfo.tm_hour >= oledOffTimeBegin && timeinfo.tm_hour < oledOffTimeEnd)
      {
        u8g2_ClearBuffer(&u8g2);
        mmm = 1;
      }
  }

  if(mmm == 0)
  {
      sprintf(buf,"%02d:%02d:%02d",timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
      u8g2_ClearBuffer(&u8g2);
      u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
      
      if(nowTimef - oldTimef > 5)
      {
        oldTimef = nowTimef;
        num++;
      }
      if(num > 64)
          num = 0;
        
        
      u8g2_DrawUTF8(&u8g2, x[num / 8 - 1], y[ num % 8], buf);
      if(boardMode == 1 && Humi > 0)
      {
        sprintf(buf,"%4.1f℃  %d%% ",Temp + 0.1 * Temp_small, Humi);
        u8g2_DrawUTF8(&u8g2, x[num / 8 - 1], y[ num % 8] + 15, buf);
      }
      else if(displayInput == 1)
      {
        sprintf(buf,"VIN:%.3fV",((double)sw35xx_c1.InVol) / 100);
        u8g2_DrawUTF8(&u8g2, x[num / 8 - 1], y[ num % 8] + 15, buf);
      }
  }
      u8g2_SendBuffer(&u8g2);

}

void deleteChar(char * buf,int begin, int num)
{
  int i = begin;
  int num1 = 0;
  while(buf[i + num] != '\0')
  {
    buf[i] = buf[i + num];
    i ++;
  }
  buf[i] = '\0';
  i = 0;
  while(buf[i] != '\0')
  {
    if(buf[i] == ':')
    num1++;
    if(buf[i] == ':' && num1 ==2)
    {
      buf[i + 3] = '\0';
      break;
    }
    i++;
  }
  
  
}

static void oledClockSurface(void){
  time_t now;
  struct tm timeinfo;
  int year = 22;
  char buf[64] = {0};

  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
  u8g2_DrawUTF8(&u8g2, 40, 15, "时钟");
//  u8g2_SetFont(&u8g2, u8g2_font_helvB10_tr);
  time(&now);
  localtime_r(&now, &timeinfo);
  sprintf(buf,"%02d:%02d:%02d",timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
  u8g2_DrawStr(&u8g2, 10, 32, buf);
  if(timeinfo.tm_year >= 100)
  {
    year = timeinfo.tm_year - 100 + 2000;  
  }
   sprintf(buf,"%d/%d/%d",year,timeinfo.tm_mon + 1,timeinfo.tm_mday);
   u8g2_DrawStr(&u8g2, 10, 48, buf);
 
  if(Humi > 0)//如果有温湿度传感器的话
  {
    sprintf(buf,"温度:%4.1f℃湿度:%d%% ",Temp + 0.1 * Temp_small, Humi);
  }
  else
  {
    strftime(buf,sizeof(timeinfo),"%c",&timeinfo);
    deleteChar(buf, 7, 3);
  }
  u8g2_DrawUTF8(&u8g2, 0, 60, buf);
  u8g2_SendBuffer(&u8g2);
}
void displayRgbSet(char* header, int16_t* RGB)
{
  while(1)
  {
    uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawUTF8(&u8g2, 20, 16, header);
    u8g2_DrawUTF8(&u8g2, 0, 30, "左右键减小或增大亮度");
    u8g2_DrawUTF8(&u8g2, 0, 46, "按确认键退出");

    if (event == U8X8_MSG_GPIO_MENU_SELECT)
    {
      nvsWrite();
      break;
    }
    else if (event == U8X8_MSG_GPIO_MENU_NEXT || event == U8X8_MSG_GPIO_MENU_DOWN)
    {
      *RGB = *RGB >= 255 ? 255 : *RGB + 1;
    }
    else if (event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP)
    {
      *RGB = *RGB <= 0 ? 0 : *RGB - 1;
    }

    char buf[20];
    sprintf(buf, "当前%s亮度 %d",header, *RGB);
    u8g2_DrawUTF8(&u8g2, 0, 62, buf);
    u8g2_SendBuffer(&u8g2);
  }
}
void rgbLight()
{
    uint8_t current_selection = 0;
    while(1)
    {
      u8g2_ClearBuffer(&u8g2);
      u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
      u8g2_SetFontRefHeightAll(&u8g2);
      current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "RGB设置", 1, "红色比重\n绿色比重\n蓝色比重\n<-返回");
      switch (current_selection)
      {
        case 1:
          displayRgbSet("红灯", &rgbProportion[0]);
          break;
        case 2:
          displayRgbSet("绿灯", &rgbProportion[1]);
          break;
        case 3:
          displayRgbSet("蓝灯", &rgbProportion[2]);
          break;

        case 4:
        EXIT_MENU_SET
          break; 
      }
      EXIT_MENU_CHECK
    }
}


void remoteControl()
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawUTF8(&u8g2, 20, 16, "远程控制");
    u8g2_DrawUTF8(&u8g2, 0, 30, "请配合电脑端软件使用");
    u8g2_DrawUTF8(&u8g2, 0, 46, "配套软件Charger");
    u8g2_DrawUTF8(&u8g2, 0, 62, "按确认键退出");
    u8g2_SendBuffer(&u8g2);
    while(u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2)) !=  U8X8_MSG_GPIO_MENU_SELECT);
}

void oledAggreLimitTask()
{
  uint8_t current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "开关PPS协议", aggreLimit + 1, "开\n关\n<-返回");
  switch (current_selection)
  {
    case 1:
      aggreLimit = 0;
      nvsWriteAggreLimit();
      break;
    case 2:
      aggreLimit = 1;
      nvsWriteAggreLimit();
      break;
    default:
      break;
  }
  SW35xxAggreLimitPPS(aggreLimit);
}

static void oledSettingSurface(void)
{
  uint8_t current_selection = 0;
  uint8_t exit_flag = 0;
  for (;;)
  {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
    u8g2_SetFontRefHeightAll(&u8g2);
    current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "设置", 1, "参数设置\n无线配置\n系统监控\n固件更新\nboardMode\n熄屏时间设置\n关于\n重启\n<-返回");
    ESP_LOGI(TAG, "oledtask!\n");
    switch (current_selection)
    {
    case 1:
      for (;;)
      {
        current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "参数设置", 1, "C口功率限制\n快充协议限制\nRGB指示灯\n熄屏是否显示输入电压\nA口LED是否常亮\n<-返回");
        switch (current_selection)
        {
        case 1:
          oledPowerLimitTask();
          break;
        case 2:
          oledAggreLimitTask();
          break;
        case 3:
          rgbLight();
          break;
        case 4:
          current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "是否显示输入电压", 1, "是\n否\n<-返回");
          switch (current_selection)
          {
            case 1:
              displayInput = 1;
              nvsWritedisplayInput();
              break;
            case 2:
              displayInput = 0;
              nvsWritedisplayInput();
              break;
          }
          break;
        case 5:
          current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "A口LED是否常亮", 1, "是\n否\n<-返回");
          switch (current_selection)
          {
            case 1:
              aPortLed = 1;
              nvsWriteAPortLed(aPortLed);
              break;
            case 2:
              aPortLed = 0;
              nvsWriteAPortLed(aPortLed);
              break;
          }
          break;


        case 6:
          EXIT_MENU_SET
          break;
        }
        EXIT_MENU_CHECK
      }
      break;
    case 2:
      for (;;)
      {
        current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "无线配置", 1, "Wifi状态\n配网\n远程控制\n蓝牙控制\n<-返回");
        switch (current_selection)
        {
        case 1:
          oledWifiShowTask();
          break;
        case 2:
          oledSmartConfigTask();
          break;
        case 3:
          remoteControl();
          break;
        case 4:
          break;
        case 5:
          EXIT_MENU_SET
          break;
        }
        EXIT_MENU_CHECK
      }
      break;
    case 3:
      for (;;)
      {
        current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "系统监控", 1, "电压电流\n快充协议\n异常代码\n<-返回");
        switch (current_selection)
        {
        case 1:
          oledVIShowTask();
          break;
        case 2:
          oledProtocolShowTask();
          break;
        case 3:
          oledFaultShowTask();
          break;
        case 4:
          EXIT_MENU_SET
          break;
        }
        EXIT_MENU_CHECK
      }
      break;
    case 4:
      for (;;)
      {
        current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "固件更新", 1, "手动更新\n自动更新\n<-返回");
        switch (current_selection)
        {
        case 1:
          oledManualOTATask();
          break;
        case 2:
          oledAutoOTATask();
          break;
        case 3:
          EXIT_MENU_SET
          break;
        }
        EXIT_MENU_CHECK
      }
      break;
    case 5:
      for (;;)
      {
        current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "boardMode", 1, "1、原PCB\n2、加了DHT11的PCB\n请勿更改!!!\n修改后A口无法输出\n<-返回");
        switch (current_selection)
        {
        case 3:
          EXIT_MENU_SET
          break;
        case 4:
           EXIT_MENU_SET
           break;
        case 1:
          nvsWriteBoardMode(0);
          esp_restart();
          break;
        case 2:
          
          nvsWriteBoardMode(1);
          esp_restart();
          break;
        case 5:
          EXIT_MENU_SET
          break;
        }
        EXIT_MENU_CHECK
      }
      break;
      case 6:
      for (;;)
      {
        char buf[50];
        sprintf(buf, "设置熄屏起始时间 %2d\n设置熄屏结束时间 %2d\n<-返回", oledOffTimeBegin, oledOffTimeEnd);
        current_selection = u8g2_UserInterfaceSelectionList(&u8g2,"设置熄屏时间", 1, buf);
        switch (current_selection)
        {
        case 1:
          
          oledOffTimeBegin = u8g2_UserInterfaceSelectionList(&u8g2, "熄屏起始时间", oledOffTimeBegin + 1, TIME_TABLE) - 1;
          nvsWriteOledOffTimeBegin();
          
          break;
        case 2:
          oledOffTimeEnd = u8g2_UserInterfaceSelectionList(&u8g2, "熄屏结束时间", oledOffTimeEnd + 1, TIME_TABLE) - 1;
          nvsWriteOledOffTimeEnd();
          break;
        case 3:
          EXIT_MENU_SET
          break;
        }
        
        EXIT_MENU_CHECK
      }
      break;
    case 7:
      current_selection = u8g2_UserInterfaceSelectionList(&u8g2, "关于", 1, "版权所有\n伦敦烟云smog\nModified by Ktx\nFix by Zyilin98\n固件版本: V1.6.7\n<-返回"); // 使用 硬编码 显示固件版本，我确实想不起来还有什么办法可以显示版本
      break;
    case 8:
      esp_restart();
      break;
    case 9:
      EXIT_MENU_SET
      break;
    }
    reSetOledProtect();
    EXIT_MENU_CHECK
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
void oledViAllShow()
{
  uint16_t c1CurRevise = sw35xx_c1.OutCur * C1REVISE;
  uint16_t c2CurRevise = sw35xx_c2.OutCur * C2REVISE;
  char buf[20] = {0};
  u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
  u8g2_SetFontRefHeightAll(&u8g2);
  u8g2_ClearBuffer(&u8g2);
  sprintf(buf, "C1口 %d.%03dV %d.%03dA", (sw35xx_c1.OutVol * 6) / 1000, (sw35xx_c1.OutVol * 6) % 1000, (c1CurRevise * 25 / 10) / 1000, (c1CurRevise * 25 / 10) % 1000);
  u8g2_DrawUTF8(&u8g2, 0, 16, buf);
  sprintf(buf, "C2口 %d.%03dV %d.%03dA", (sw35xx_c2.OutVol * 6) / 1000, (sw35xx_c2.OutVol * 6) % 1000, (c2CurRevise * 25 / 10) / 1000, (c2CurRevise * 25 / 10) % 1000);
  u8g2_DrawUTF8(&u8g2, 0, 32, buf);
  sprintf(buf, "A1口 %d.%03dV", (ADC[0]) / 1000, (ADC[0]) % 1000);
  u8g2_DrawUTF8(&u8g2, 0, 48, buf);
  sprintf(buf, "A2口 %d.%03dV", (ADC[1]) / 1000, (ADC[1]) % 1000);
  u8g2_DrawUTF8(&u8g2, 0, 64, buf);
  u8g2_DrawUTF8(&u8g2, 90, 48, "VIN");
  sprintf(buf, "%6.3fV", ((double)sw35xx_c1.InVol) / 100);
  u8g2_DrawUTF8(&u8g2, 80, 64, buf);
  u8g2_SendBuffer(&u8g2);
}

void reSetOledProtect()
{
    OledProtectBegin = 0;
    xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
    time(&beginTime);
}

void findChange(double c1POld, double c2POld, uint8_t *Mode)
{
    
    //拔插电 熄屏判断
    double c1P = ((double)sw35xx_c1.OutVol * 6) * ((double)sw35xx_c1.OutCur * 25 / 10) / 1000000;
    double c2P = ((double)sw35xx_c2.OutVol * 6) * ((double)sw35xx_c2.OutCur * 25 / 10) / 1000000;


    if((c1P > 0.3 && c1POld < 0.2) || (c1P < 0.2 && c1POld > 0.3) || (c2P > 0.3 && c2POld < 0.2) || (c2P < 0.2 && c2POld > 0.3))
    {
      if(OledProtectBegin == 1)
      {
        *Mode = 0;
        reSetOledProtect();
      }
      
    }
    
}
void oledChargePower(uint8_t event)
{
 
  char buf[20] = {0};
  u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
  u8g2_SetFontRefHeightAll(&u8g2);
  u8g2_ClearBuffer(&u8g2);
  sprintf(buf, " C1口   %6.3fW", ((double)sw35xx_c1.OutVol * 6) / 1000 * ((double)sw35xx_c1.OutCur * 25 / 10) / 1000);
  u8g2_DrawUTF8(&u8g2, 0, 16, buf);
  sprintf(buf, " %5.2fWh%7.1fmAh", c1Power, c1Power / 3.7 * 1000);
  u8g2_DrawUTF8(&u8g2, 0, 31, buf);

  sprintf(buf, " C2口   %6.3fW", ((double)sw35xx_c2.OutVol * 6) / 1000 * ((double)sw35xx_c2.OutCur * 25 / 10) / 1000);
  u8g2_DrawUTF8(&u8g2, 0, 46, buf);
  sprintf(buf, " %5.2fWh%7.1fmAh", c2Power, c2Power / 3.7 * 1000);
  u8g2_DrawUTF8(&u8g2, 0, 62, buf);

  u8g2_SendBuffer(&u8g2);
  if(event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP)
  {
    c1Power = 0;
    c2Power = 0;
  }
}
void oledTask(void *pvParameters)
{
  uint8_t Mode = 0; // 0-3分别对应主界面1，主界面2，主界面3
  time_t nowTime = 0;
  time(&beginTime);
  double c1POld = (sw35xx_c1.OutVol * 6) * (sw35xx_c1.OutCur * 25 / 10) / 1000000;
  double c2POld = (sw35xx_c2.OutVol * 6) * (sw35xx_c2.OutCur * 25 / 10) / 1000000;
  double c1P = (sw35xx_c1.OutVol * 6) * (sw35xx_c1.OutCur * 25 / 10) / 1000000;
  double c2P = (sw35xx_c2.OutVol * 6) * (sw35xx_c2.OutCur * 25 / 10) / 1000000;
  int viShowState = 0;
  for (;;)
  {
    uint8_t event = u8x8_GetMenuEvent(u8g2_GetU8x8(&u8g2));
    findChange(c1POld, c2POld, &Mode);
    c1POld = ((double)sw35xx_c1.OutVol * 6) * ((double)sw35xx_c1.OutCur * 25 / 10) / 1000000;
    c2POld = ((double)sw35xx_c2.OutVol * 6) * ((double)sw35xx_c2.OutCur * 25 / 10) / 1000000;

    if(OledProtectBegin == 0)
    {
      switch (Mode)
      {
      case 0:
          if(viShowState == 0)
            oledChargeSurface();
          else if(viShowState == 1)
            oledViAllShow();
          else if(viShowState == 2)
            oledChargePower(event);
          break;
       case 1:
         oledWeatherSurface();
         break;
      case 2:
        oledClockSurface();
        break;
      case 3:
        oledSettingSurface();
        Mode = 0;
        break;
      }
    }
    else
    {
        oledStopDisplay();
    }
   
    if(OledProtectBegin == 0)
    {
    
      time(&nowTime);
      if(nowTime - beginTime >= 45)
      {
        OledProtectBegin = 1;
        
      }
     
      if (event == U8X8_MSG_GPIO_MENU_SELECT)
      {
        time(&beginTime);
        if(Mode == 0)
        {
          viShowState = (viShowState == 2) ? 0 : viShowState + 1;
        }
        if(Mode == 1)
        {
          xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
        }
        
      }
      else if (event == U8X8_MSG_GPIO_MENU_NEXT || event == U8X8_MSG_GPIO_MENU_DOWN)
      {
        time(&beginTime);
        Mode = (Mode >= 3 ? 3 : Mode + 1);
      }
      else if (event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP)
      {
        time(&beginTime);
        Mode = (Mode <= 0 ? 0 : Mode - 1);
      }
      
    }
    else
    {
      if(event == U8X8_MSG_GPIO_MENU_SELECT || event == U8X8_MSG_GPIO_MENU_NEXT || event == U8X8_MSG_GPIO_MENU_DOWN || event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP)
      {
        reSetOledProtect();
      }
    }
  
    
    vTaskDelay(pdMS_TO_TICKS(20));
  }
  vTaskDelete(NULL);
}

void oledInitMessageTask(uint8_t num,char *state){
  char buf[25] = {0};
	u8g2_SetFont(&u8g2, u8g2_font_wqy13_t_gb2312);
  switch(num){
    case 1:
      sprintf(buf,"内部外设初始化...%s",state);
      u8g2_DrawUTF8(&u8g2, 0, 15, buf);    
      break;
    case 2:
      sprintf(buf,"功率芯片初始化...%s",state);
      u8g2_DrawUTF8(&u8g2, 0, 31, buf);
      break;
    case 3:
      sprintf(buf,"加速度计初始化...%s",state);
      u8g2_DrawUTF8(&u8g2, 0, 47, buf);
      break;
    case 4:
      sprintf(buf,"WIFI网络初始化...%s",state);
      u8g2_DrawUTF8(&u8g2, 0, 63, buf);
      break;
      case 5:
      sprintf(buf,"使用esptouch配网...%s",state);
      u8g2_DrawUTF8(&u8g2, 0, 63, buf);
      break;
  }
  u8g2_SendBuffer(&u8g2);
}
