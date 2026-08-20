/*
 * w9825g6kh6.c
 *
 *  Created on: Aug 18, 2026
 *      Author: ozpv
 */

#include "fmc.h"
#include "w9825g6kh6.h"

void FMC_Init(void) {
    /* USER CODE BEGIN FMC_Init 2 */

    FMC_SDRAM_CommandTypeDef Command;

    /* Step 1 and Step 2 already done in HAL_SDRAM_Init() */

    /* Step 3: Configure a clock configuration enable command */
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE; /* Set MODE bits to "001" */
    Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1; /* configure the Target Bank bits */
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xfff);

    /* Step 4: Insert 100 us minimum delay - Min HAL Delay is 1ms */
    HAL_Delay(1);

    /* Step 5: Configure a PALL (precharge all) command */
    Command.CommandMode = FMC_SDRAM_CMD_PALL; /* Set MODE bits to "010" */
    HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xfff);

    /* Step 6: Configure an Auto Refresh command */
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE; /* Set MODE bits to "011" */
    Command.AutoRefreshNumber = 2;
    HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xfff);

    /* Step 7: Program the external memory mode register */
    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;/*set the MODE bits to "100" */
    Command.ModeRegisterDefinition = (uint32_t)0 | 0<<3 | 2<<4 | 0<<7 | 1<<9;
    HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xfff);

    /* Step 8: Set the refresh rate counter - refer to section SDRAM refresh timer register in RM0455 */
    /* Set the device refresh rate
    * COUNT = [(SDRAM self refresh time / number of row) x SDRAM CLK] – 20
    = [(64ms/8192) * 100MHz] - 20 = floor(1562.5 - 20) = 761 */
    HAL_SDRAM_ProgramRefreshRate(&hsdram1, 761);

    /* USER CODE END FMC_Init 2 */
}

/* a test using address dependent values */
void SDRAM_Test(void) {
    volatile uint32_t *sdram = (volatile uint32_t *)SDRAM_BASE;

    for (uint32_t i = 0; i < SDRAM_SIZE / sizeof(uint32_t); ++i) {
        sdram[i] = i ^ 0xDEADDEAD;
    }

    for (uint32_t i = 0; i < SDRAM_SIZE / sizeof(uint32_t); ++i) {
        if (sdram[i] != (i ^ 0xDEADDEAD)) {
            Error_Handler();
        }
    }
}
