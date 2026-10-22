/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((চঞ্চল uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
শূন্য uart_drv_putchar(unsigned বর্ণ c)
{
    যাবত্ ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned বর্ণ uart_drv_getchar(শূন্য)
{
    যাবত্ ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    প্রত্যাগম *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
