/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#ವ್ಯಾಖ್ಯಾನಿಸು UTHR        0x0
#ವ್ಯಾಖ್ಯಾನಿಸು ULSR        0x14

#ವ್ಯಾಖ್ಯಾನಿಸು ULSR_THRE   ಬಿಟ್(5)
#ವ್ಯಾಖ್ಯಾನಿಸು ULSR_RDR    ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆಗ ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    ಹಿಂತಿರುಗಿ *UART_REG(UTHR);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
