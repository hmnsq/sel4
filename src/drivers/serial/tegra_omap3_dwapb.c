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

#تعریف_کن UTHR        0x0
#تعریف_کن ULSR        0x14

#تعریف_کن ULSR_THRE   بیت(5)
#تعریف_کن ULSR_RDR    بیت(0)

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    أرجع *UART_REG(UTHR);
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
