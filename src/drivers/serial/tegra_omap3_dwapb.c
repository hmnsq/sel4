/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
فراغ uart_drv_putchar(unsigned حرف c)
{
    ܟܡܐ ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned حرف uart_drv_getchar(فراغ)
{
    ܟܡܐ ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    ܦܢܐ *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
