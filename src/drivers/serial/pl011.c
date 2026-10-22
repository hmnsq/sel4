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

#تعریف_کړه UARTDR                    0x000
#تعریف_کړه UARTFR                    0x018

#تعریف_کړه PL011_UARTFR_TXFF         بټ(5)
#تعریف_کړه PL011_UARTFR_RXFE         بټ(4)

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    أرجع *UART_REG(UARTDR);
}
#د_که_پای  /* CONFIG_DEBUG_BUILD */
