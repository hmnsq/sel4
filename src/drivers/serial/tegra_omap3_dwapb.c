/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((ଅସ୍ଥିର uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ଶୂନ୍ୟ uart_drv_putchar(unsigned ଅକ୍ଷର c)
{
    ଯେତେବେଳେ ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ଅକ୍ଷର uart_drv_getchar(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    ଫେରନ୍ତୁ *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
