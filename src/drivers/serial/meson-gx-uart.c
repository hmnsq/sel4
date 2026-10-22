/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#تعریف_کرو UART_WFIFO  0x0
#تعریف_کرو UART_RFIFO  0x4
#تعریف_کرو UART_حالت 0xC

#تعریف_کرو UART_TX_بھرا        ثنائی_ہندسہ(21)
#تعریف_کرو UART_RX_خالی_خانے       ثنائی_ہندسہ(20)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب ((*UART_REG(UART_حالت) & UART_TX_بھرا));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب ((*UART_REG(UART_حالت) & UART_RX_خالی_خانے));
    واپس *UART_REG(UART_RFIFO);
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
