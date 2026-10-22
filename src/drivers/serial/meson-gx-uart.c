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

#تعریف_کن UART_WFIFO  0x0
#تعریف_کن UART_RFIFO  0x4
#تعریف_کن UART_وضعیت 0xC

#تعریف_کن UART_TX_پر        بیت(21)
#تعریف_کن UART_RX_خالی       بیت(20)

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما ((*UART_REG(UART_وضعیت) & UART_TX_پر));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UART_وضعیت) & UART_RX_خالی));
    أرجع *UART_REG(UART_RFIFO);
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
