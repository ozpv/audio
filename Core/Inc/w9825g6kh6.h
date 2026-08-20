/*
 * w9825g6kh6.h
 *
 *  Created on: Aug 18, 2026
 *      Author: ozpv
 */

#ifndef INC_W9825G6KH6_H_
#define INC_W9825G6KH6_H_

#define SDRAM_BASE 0xC0000000
#define SDRAM_SIZE 0x02000000

void FMC_Init(void);
void SDRAM_Test(void);

#endif /* INC_W9825G6KH6_H_ */
