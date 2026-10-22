/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((അസ്ഥിര uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ശൂന്യം uart_drv_putchar(unsigned അക്ഷരം c)
{
    ആകുമ്പോൾ ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം uart_drv_getchar(ശൂന്യം)
{
    ആകുമ്പോൾ ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    തിരികെ *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
