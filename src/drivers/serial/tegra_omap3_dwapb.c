/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((فرار uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
تهی uart_drv_putchar(unsigned نویسه c)
{
    وقتی ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned نویسه uart_drv_getchar(تهی)
{
    وقتی ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    بازگشت *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
