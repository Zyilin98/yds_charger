#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "adc_read.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t cali_handle = NULL;

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
    // Initialize ADC calibration
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten1,
        .bitwidth = width,
    };

    esp_err_t cali_status = adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle);
    print_char_val_type(cali_status);

    // Configure ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = unit,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t chan_config = {
        .atten = atten1,
        .bitwidth = width,
    };
    adc_oneshot_config_channel(adc1_handle, channel1, &chan_config);
    adc_oneshot_config_channel(adc1_handle, channel2, &chan_config);
}

void ADC_getVoltage(uint32_t *adcdate)
{
    uint32_t adc_reading[2] = {0};
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_oneshot_read(adc1_handle, channel1, (int *)&adc_reading[0]);
        adc_oneshot_read(adc1_handle, channel2, (int *)&adc_reading[1]);
    }
    adc_reading[0] /= NO_OF_SAMPLES;
    adc_reading[1] /= NO_OF_SAMPLES;

    // Convert raw ADC readings to voltage (mV)
    int voltage;
    adc_cali_raw_to_voltage(cali_handle, adc_reading[0], &voltage);
    *adcdate = voltage;

    adc_cali_raw_to_voltage(cali_handle, adc_reading[1], &voltage);
    *(adcdate + 1) = voltage;
}