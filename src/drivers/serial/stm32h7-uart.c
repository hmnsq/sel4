/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#ವ್ಯಾಖ್ಯಾನಿಸು USART_ISR               0x1C
#ವ್ಯಾಖ್ಯಾನಿಸು USART_TDR               0x28

#ವ್ಯಾಖ್ಯಾನಿಸು USART_ISR_TXE           0x80U

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ -1;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
