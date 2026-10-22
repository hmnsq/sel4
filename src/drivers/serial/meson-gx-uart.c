/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#تعریف_کړه UART_WFIFO  0x0
#تعریف_کړه UART_RFIFO  0x4
#تعریف_کړه UART_حالت 0xC

#تعریف_کړه UART_TX_ډک        بټ(21)
#تعریف_کړه UART_RX_تشه       بټ(20)

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما ((*UART_REG(UART_حالت) & UART_TX_ډک));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UART_حالت) & UART_RX_تشه));
    أرجع *UART_REG(UART_RFIFO);
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
