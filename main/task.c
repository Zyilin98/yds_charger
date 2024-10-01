/*
 * @Author: [LiaoZhelin]
 * @Date: 2022-05-10 14:35:47
 * @LastEditors: [LiaoZhelin]
 * @LastEditTime: 2022-06-14 13:44:33
 * @Description: 
 */
#include "task.h"
#include <stdio.h>
#include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "adc_read.h"
#include "lis3dh.h"
//#include "led_strip.h"
#include "sw3526.h"
#include "dht11.h"
#include "menu.h"
static const char *TAG = "task";
extern int16_t rgbProportion[3];
extern int16_t light;
int8_t rgbOn[4] = {0};

void dht11Task(void *pvParameters)
{
  for(;;)
  {
     DHT11();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  vTaskDelete(NULL);
}

void adcTask(void *pvParameters)
{
  for (;;)
  {
    ADC_getVoltage(ADC);
    if(aPortLed == 0)
    {
      if(OledProtectBegin == 0)
      {
        rgbOn[3] = 1;
        rgbOn[2] = 1;
      }
      else
      {
      rgbOn[3] = ADC[0] / 1000 > 6 ? 1 : 0;
      rgbOn[2] = ADC[1] / 1000 > 6 ? 1 : 0;
      }
    }
    else
    {
      rgbOn[3] = 1;
      rgbOn[2] = 1;
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
  vTaskDelete(NULL);
}

void sw35xxTask(void *pvParameters)
{
  for (;;)
  {
    
    SW35XXUpdate();
    double c1P = ((double)sw35xx_c1.OutVol * 6) * ((double)sw35xx_c1.OutCur * 25 / 10) / 1000000;
    double c2P = ((double)sw35xx_c2.OutVol * 6) * ((double)sw35xx_c2.OutCur * 25 / 10) / 1000000;
    if(c1P > 0.2)
    {
      rgbOn[0] = 1;
    }
    else if(c1P < 0.1)
    {
      rgbOn[0] = 0;
    }


    if(c2P > 0.2)
    {
      rgbOn[1] = 1;
    }
    else if(c2P < 0.1)
    {
      rgbOn[1] = 0;
    }
    c1Power += (((double)sw35xx_c1.OutVol * 6) / 1000 * ((double)sw35xx_c1.OutCur * 25 / 10) / 1000) / 18000;
    c2Power += (((double)sw35xx_c2.OutVol * 6) / 1000 * ((double)sw35xx_c2.OutCur * 25 / 10) / 1000) / 18000;
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void ws28xxTask(void *pvParameters)
{
  uint32_t red = 0;
  uint32_t green = 1;
  uint32_t blue = 0;
  uint8_t rgb_flag = 0;
  for (;;)
  {
    red = green;
    blue = green;
    for (int j = 0; j < 4; j += 1)
    {
      ESP_ERROR_CHECK(strip->set_pixel(strip, j, red * rgbProportion[0] * light / 100 / 150 * rgbOn[j], green * rgbProportion[1] * light / 100 / 150 * rgbOn[j], blue * rgbProportion[2] * light / 100 / 150 * rgbOn[j]));
    }
    ESP_ERROR_CHECK(strip->refresh(strip, 100));


    if (!rgb_flag)
    { 
      green = (green > 150 ? 150 : green + 1);
      if (green == 150)
      {
        rgb_flag = 1;
      }
    }
    else
    {
      green = (green <= 0 ? 0 : green - 1);
      if (green == 0)
      {
        rgb_flag = 0;
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void lis3dhTask(void *pvParameters)
{
  uint8_t buffer1,buffer2;
  uint16_t X_V,Y_V,Z_V;
  for (;;)
  {
    LIS3DH_ReadReg(LIS3DH_REG_OUT_X_L,&buffer1);
    LIS3DH_ReadReg(LIS3DH_REG_OUT_X_H,&buffer2);
    X_V = ((buffer2<<8)|buffer1);

    LIS3DH_ReadReg(LIS3DH_REG_OUT_Y_L,&buffer1);
    LIS3DH_ReadReg(LIS3DH_REG_OUT_Y_H,&buffer2);
    Y_V = ((buffer2<<8)|buffer1);
    LIS3DH_ReadReg(LIS3DH_REG_OUT_Z_L,&buffer1);
    LIS3DH_ReadReg(LIS3DH_REG_OUT_Z_H,&buffer2);
    Z_V = ((buffer2<<8)|buffer1);
    //ESP_LOGI(TAG, "LIS3DH_X=%d  LIS3DH_Y=%d  LIS3DH_Z=%d",X_V,Y_V,Z_V);
    vTaskDelay(pdMS_TO_TICKS(40));
  }
}

void ntpClockTask(void *pvParameters){
  for (;;){
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void taskMonitor(void *pvParameters){
  UBaseType_t uxHighWaterMark;
  for (;;){
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("--------------------------------------------\r\n");
    uxHighWaterMark = uxTaskGetStackHighWaterMark(adcTask_handle);
    printf("Task: adcTask_handle stacksize=%d\r\n",uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(sw35xxTask_handle);
    printf("Task: sw35xxTask_handle stacksize=%d\r\n",uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(ws28xxTask_handle);
    printf("Task: ws28xxTask_handle stacksize=%d\r\n",uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(lis3dhtask_handle);
    printf("Task: lis3dhtask_handle stacksize=%d\r\n",uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(oledTask_handle);
    printf("Task: oledTask_handle stacksize=%d\r\n",uxHighWaterMark);

    uxHighWaterMark = uxTaskGetStackHighWaterMark(ntpTask_handle);
    printf("Task: ntpTask_handle stacksize=%d\r\n",uxHighWaterMark);
  }
}
