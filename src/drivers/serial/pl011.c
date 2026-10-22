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

#تعریف_کن UARTDR                    0x000
#تعریف_کن UARTFR                    0x018

#تعریف_کن PL011_UARTFR_TXFF         بیت(5)
#تعریف_کن PL011_UARTFR_RXFE         بیت(4)

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    أرجع *UART_REG(UARTDR);
}
#پایان_اگر  /* CONFIG_DEBUG_BUILD */
