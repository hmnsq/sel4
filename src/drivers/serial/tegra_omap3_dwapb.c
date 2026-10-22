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

#تعریف_کرو UTHR        0x0
#تعریف_کرو ULSR        0x14

#تعریف_کرو ULSR_THRE   ثنائی_ہندسہ(5)
#تعریف_کرو ULSR_RDR    ثنائی_ہندسہ(0)

#تعریف_کرو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشانی حرف c)
{
    طالما ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    أرجع *UART_REG(UTHR);
}
#جے_انت /* CONFIG_DEBUG_BUILD */
