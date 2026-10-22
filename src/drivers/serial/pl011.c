/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#تعریف_کرو UARTDR                    0x000
#تعریف_کرو UARTFR                    0x018

#تعریف_کرو PL011_UARTFR_TXFF         ثنائی_ہندسہ(5)
#تعریف_کرو PL011_UARTFR_RXFE         ثنائی_ہندسہ(4)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    واپس *UART_REG(UARTDR);
}
#اگر_ختم  /* CONFIG_DEBUG_BUILD */
