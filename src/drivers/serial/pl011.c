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

#وصف_ڪريو UARTDR                    0x000
#وصف_ڪريو UARTFR                    0x018

#وصف_ڪريو PL011_UARTFR_TXFF         ٻٽو_انگ(5)
#وصف_ڪريو PL011_UARTFR_RXFE         ٻٽو_انگ(4)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    أرجع *UART_REG(UARTDR);
}
#جيڪڏهن_پورو  /* CONFIG_DEBUG_BUILD */
