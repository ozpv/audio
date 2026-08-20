/*
 * cs4272.h
 *
 *  Created on: Aug 16, 2026
 *      Author: ozpv
 */

#ifndef INC_CS4272_H_
#define INC_CS4272_H_

#include <stdint.h>

/* cs4272k-czzr 7. REGISTER QUICK REFERENCE */
#define MODE_CONTROL_1 0x01
#define DAC_CONTROL 0x02
#define DAC_VOLUME_AND_MIXING_CONTROL 0x03
#define DAC_CH_A_VOLUME_CONTROL 0x04
#define DAC_CH_B_VOLUME_CONTROL 0x05
#define ADC_CONTROL 0x06
#define MODE_CONTROL_2 0x07
#define CHIP_ID 0x08

#define CS4272_CHIP_ID 0x0

/* set by ad0 ncs pin */
#define CS4272_I2C_ADDR (0b0010000 << 1)

#define SAMPLE_RATE 48000
#define BUFFER_SIZE 512

extern volatile int32_t rx_buffer[BUFFER_SIZE];
extern volatile int32_t tx_buffer[BUFFER_SIZE];
extern volatile float samples[BUFFER_SIZE];

extern const uint8_t CS4272_CONFIG[7];

uint8_t CS4272_Init(void);

#define INT24_MAX 0x7FFFFF

#endif /* INC_CS4272_H_ */
