/*
 * cs4272.c
 *
 *  Created on: Aug 16, 2026
 *      Author: ozpv
 */

#include "i2c.h"
#include "cs4272.h"
#include "ringmod.h"

volatile int32_t rx_buffer[BUFFER_SIZE] __attribute__((section(".audio_buffers")));
volatile int32_t tx_buffer[BUFFER_SIZE] __attribute__((section(".audio_buffers")));
volatile float samples[BUFFER_SIZE] __attribute__((section(".sdram")));

const uint8_t CS4272_CONFIG[7] = {
        /* MODE CONTROL 1
           single speed i2s slave */
        0b00000001,
        /* DAC CONTROL
          Steep Interpolation Filter
          48 kHz de-emphasis
          Soft Ramp Up and Down and no polarity flip */
        0b01101100,
        /* DAC Volume & Mixing Control
          Channel B Volume = Channel A Volume
          Soft Ramp and Zero Cross
          a = L b = R
          stereo 0b01111001 */
        0b01111000,
        /* Channel A volume
           0 dB
           Binary Code Decimal Value Volume Setting
           0000000 0 0 dB
           0010100 20 -20 dB
           0101000 40 -40 dB
           0111100 60 -60 dB
           1011010 90 -90 dB */
        0b00000000,
        /* Channel B volume
           Muted for mono */
        0b10000000,
        /* ADC control
           I2S up to 24-bit data High Pass filter on
           bits 3:2 active high adc channel mute
           Channel B muted */
        0b00010100,
        /* MODE CONTROL 2
           power down OFF */
        0b00000010,
};

void int32_t_to_float(int32_t *in, float *out, size_t range_min, size_t range_max) {
    for (size_t i = range_min; i < range_max; ++i) {
        out[i] = ((float)(in[i] << 8)) / ((float)INT32_MAX);
    }
}

void float_to_int32_t(float *in, int32_t *out, size_t range_min, size_t range_max) {
    for (size_t i = range_min; i < range_max; ++i) {
        out[i] = ((int32_t)(in[i] * ((float)INT32_MAX))) >> 8;
    }
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
    int32_t_to_float((int32_t *)rx_buffer, (float *)samples, 0, BUFFER_SIZE / 2);

    ringmod((float *)samples, 0, BUFFER_SIZE / 2);

    float_to_int32_t((float *)samples, (int32_t *)tx_buffer, 0, BUFFER_SIZE / 2);
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
    int32_t_to_float((int32_t *)rx_buffer, (float *)samples, BUFFER_SIZE / 2, BUFFER_SIZE);

    ringmod((float *)samples, BUFFER_SIZE / 2, BUFFER_SIZE);

    float_to_int32_t((float *)samples, (int32_t *)tx_buffer, BUFFER_SIZE / 2, BUFFER_SIZE);
}


HAL_StatusTypeDef CS4272_ReadRegister(uint8_t addr, uint8_t *data) {
    return HAL_I2C_Mem_Read(&hi2c4, CS4272_I2C_ADDR, addr, I2C_MEMADD_SIZE_8BIT,
            data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef CS4272_WriteRegister(uint8_t addr, uint8_t data) {
    return HAL_I2C_Mem_Write(&hi2c4, CS4272_I2C_ADDR, addr,
    I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

void CS4272_Reset(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

    HAL_Delay(10);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

    HAL_Delay(10);
}

uint8_t CS4272_Init(void) {
    CS4272_Reset();

    if (HAL_I2C_IsDeviceReady(&hi2c4, CS4272_I2C_ADDR, 3, 1000) != HAL_OK) {
        return HAL_ERROR;
    }

    /* 5.2.1 Recommended Power-Up Sequence
       power down mode and control port enable */
    if (CS4272_WriteRegister(MODE_CONTROL_2, 0x03) != HAL_OK) {
        return HAL_ERROR;
    }

    /* check if device ID is correct */
    uint8_t id = 0;
    if (CS4272_ReadRegister(CHIP_ID, &id) != HAL_OK) {
        return HAL_ERROR;
    }

    if ((id >> 4) != CS4272_CHIP_ID) {
        return 2;
    }

    /* Configure */
    uint8_t cfg = 0;
    for (size_t i = MODE_CONTROL_1; i < CHIP_ID; ++i) {
        if (CS4272_WriteRegister(i, CS4272_CONFIG[i - 1]) != HAL_OK) {
            return HAL_ERROR;
        }

        if (CS4272_ReadRegister(i, &cfg) != HAL_OK) {
            return HAL_ERROR;
        }

        if (cfg != CS4272_CONFIG[i - 1]) {
            return 2;
        }
    }

    return HAL_OK;
}
