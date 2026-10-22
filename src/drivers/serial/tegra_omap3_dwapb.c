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

#تعریف_کړه UTHR        0x0
#تعریف_کړه ULSR        0x14

#تعریف_کړه ULSR_THRE   بټ(5)
#تعریف_کړه ULSR_RDR    بټ(0)

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    أرجع *UART_REG(UTHR);
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
