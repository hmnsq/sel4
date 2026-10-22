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

#وصف_ڪريو UTHR        0x0
#وصف_ڪريو ULSR        0x14

#وصف_ڪريو ULSR_THRE   ٻٽو_انگ(5)
#وصف_ڪريو ULSR_RDR    ٻٽو_انگ(0)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    أرجع *UART_REG(UTHR);
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
