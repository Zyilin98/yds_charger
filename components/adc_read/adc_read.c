#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adc_read.h"

static const char *TAG = "adc_read";
adc_cali_handle_t cali_handle;

static void print_char_val_type(esp_err_t cali_status)
{
    if (cali_status == ESP_OK) {
        printf("ADC Calibration Initialized Successfully\n");
    } else {
        printf("ADC Calibration Initialization Failed\n");
    }
}

void ADC_Init(void)
{
    adc_cali_curve_fitting_config_t cali_config = {
        .atten = atten1,
        .bitwidth = width
    };

    esp_err_t cali_status = adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle);
    print_char_val_type(cali_status);

    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel1, atten1);
        adc1_config_channel_atten(channel2, atten2);
    }
}

void ADC_getVoltage(uint32_t *adcdate)
{
    uint32_t adc_reading[2] = {0};
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading[0] += adc1_get_raw((adc1_channel_t)channel1);
        adc_reading[1] += adc1_get_raw((adc1_channel_t)channel2);
    }
    adc_reading[0] /= NO_OF_SAMPLES;
    adc_reading[1] /= NO_OF_SAMPLES;

    int voltage;
    adc_cali_raw_to_voltage(cali_handle, adc_reading[0], &voltage);
    *adcdate = voltage;

    adc_cali_raw_to_voltage(cali_handle, adc_reading[1], &voltage);
    *(adcdate + 1) = voltage;
}
