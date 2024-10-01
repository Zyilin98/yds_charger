/*
 * @Author: [LiaoZhelin]
 * @Date: 2022-05-09 21:17:30
 * @LastEditors: [LiaoZhelin]
 * @LastEditTime: 2022-05-09 22:36:39
 * @Description: 
 */
#ifndef _OTA_H_
#define _OTA_H_

void advanced_ota_example_task(void *pvParameter);
void OTA_Init(void);
void setCheckVersion();
uint16_t getCheckVersion();
void getRunningVersion(char * v);
void getNewVersion(char * v);
uint16_t getIsEqu();
#endif
