/*
 * @Author: [LiaoZhelin]
 * @Date: 2022-04-29 20:35:49
 * @LastEditors: [LiaoZhelin]
 * @LastEditTime: 2022-05-12 00:38:34
 * @Description: 
 */
#ifndef MENU_H
#define MENU_H

#include <stdio.h>
typedef struct 
{
  char city[20];
  char date[20];
  char week[20];
  char updateTime[20];
  char wea[20];
  char tem[20];
  char temDay[20];
  char temNight[20];
  char humidity[20];
}myWeather;
myWeather mWeather;
void oledTask(void *pvParameters);
void oledStopDisplay();
void oledInitMessageTask(uint8_t num,char *state);
void reSetOledProtect();
void nvsWriteCity();
void nvsWriteAppid();
void nvsWriteAppsecret();
void nvsWrite();
void oledViAllShow();
int8_t oledOffTimeBegin;
int8_t oledOffTimeEnd;
int8_t displayInput;
double c1Power;
double c2Power;
uint32_t timeTest;
int OledProtectBegin;
int8_t aPortLed;
int8_t aggreLimit;
#endif