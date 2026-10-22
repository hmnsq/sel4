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

#تعریف_کرو UARTDR                    0x000
#تعریف_کرو UARTFR                    0x018

#تعریف_کرو PL011_UARTFR_TXFF         ثنائی_ہندسہ(5)
#تعریف_کرو PL011_UARTFR_RXFE         ثنائی_ہندسہ(4)

#تعریف_کرو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشانی حرف c)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    أرجع *UART_REG(UARTDR);
}
#جے_انت  /* CONFIG_DEBUG_BUILD */
