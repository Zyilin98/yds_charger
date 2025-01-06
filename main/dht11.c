#include <stdio.h>
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "string.h"
 
#define DHT11_PIN   21//定义DHT11的引脚
 
#define uchar uint8_t
#define uint8 uint8_t
#define uint16 uint16_t
 
//温湿度定义
uchar ucharFLAG,uchartemp;
uint8_t Humi,Humi_small,Temp,Temp_small;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;
 
static void InputInitial(void)//设置端口为输入
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
}
 
static void OutputHigh(void)//输出1
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(DHT11_PIN, 1);
}
 
static void OutputLow(void)//输出0
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(DHT11_PIN, 0);
}
 
static uint8 getData()//读取状态
{
	return gpio_get_level(DHT11_PIN);
}
 
//读取一个字节数据
static void COM(void)    // 温湿写入
{
    uchar i;
    for(i=0;i<8;i++)
    {
        ucharFLAG=2;
        //等待IO口变低，变低后，通过延时去判断是0还是1
        while((getData()==0)&&ucharFLAG++) ets_delay_us(10);
        ets_delay_us(35);//延时35us
        uchartemp=0;
 
        //如果这个位是1，35us后，还是1，否则为0
        if(getData()==1) 
          uchartemp=1;
        ucharFLAG=2;
 
        //等待IO口变高，变高后，表示可以读取下一位
        while((getData()==1)&&ucharFLAG++) 
          ets_delay_us(10);
        if(ucharFLAG==1)
          break;
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp;
    }
}
 
void Delay_ms(uint16 ms)
{
	int i=0;
	for(i=0; i<ms; i++){
		ets_delay_us(1000);
	}
}
 
int DHT11(void)   //温湿传感启动
{
    OutputLow();
    Delay_ms(19);  //>18MS
    OutputHigh();
    InputInitial(); //输入
    ets_delay_us(30);
    if(!getData())//表示传感器拉低总线
    {
        ucharFLAG=2;
        //等待总线被传感器拉高
        while((!getData())&&ucharFLAG++) 
          ets_delay_us(10);
        ucharFLAG=2;
        //等待总线被传感器拉低
        while((getData())&&ucharFLAG++) 
          ets_delay_us(10);
        COM();//读取第1字节，
        ucharRH_data_H_temp=ucharcomdata;
        COM();//读取第2字节，
        ucharRH_data_L_temp=ucharcomdata;
        COM();//读取第3字节，
        ucharT_data_H_temp=ucharcomdata;
        COM();//读取第4字节，
        ucharT_data_L_temp=ucharcomdata;
        COM();//读取第5字节，
        ucharcheckdata_temp=ucharcomdata;
        OutputHigh();
        //判断校验和是否一致
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
        if(uchartemp==ucharcheckdata_temp)
        {
            //校验和一致，
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
            //保存温度和湿度
            Humi=ucharRH_data_H;
            Humi_small=ucharRH_data_L;
            Temp=ucharT_data_H;
            Temp_small=ucharT_data_L;
        }
        else
        {
          return 2;
        }
    }
    else //没用成功读取，返回0
    {
    	return 1;
    }
   
    OutputHigh(); //输出
    return 0;
}
 
 