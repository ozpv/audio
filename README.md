Note: Application will not work and get stuck in Reset_Handler with XIP when compiled with no optimization. Ensure you use -Os or -O2.
QUADSPI should not be reinitalized. Only configure it in STM32CubeMX so the clocks are the same and comment out MX_QUADSPI_Init.
Comment out PeriphCommonClock_Config when regenerating.