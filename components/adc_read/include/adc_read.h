#ifndef _ADC_READ_H_
#define _ADC_READ_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

 static const adc_unit_t unit = ADC_UNIT_1;
 static const adc_bitwidth_t width = ADC_BITWIDTH_12;

 static const adc_atten_t atten1 = ADC_ATTEN_DB_12;
 static const adc_channel_t channel1 = ADC_CHANNEL_0;

 static const adc_atten_t atten2 = ADC_ATTEN_DB_12;
 static const adc_channel_t channel2 = ADC_CHANNEL_1;

#define DEFAULT_VREF    1100
#define NO_OF_SAMPLES   64

 extern adc_cali_handle_t cali_handle;
 extern adc_oneshot_unit_handle_t adc1_handle;

 void ADC_Init(void);
 void ADC_getVoltage(uint32_t *adcdate);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif