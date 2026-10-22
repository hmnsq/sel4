/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>

#تعریف_کرو USR                   0x08
#تعریف_کرو UTF                   0x70
#تعریف_کرو UNTX                  0x40

#تعریف_کرو USR_RXRDY             ثنائی_ہندسہ(0)
#تعریف_کرو USR_RXFUL             ثنائی_ہندسہ(1)
#تعریف_کرو USR_TXRDY             ثنائی_ہندسہ(2)
#تعریف_کرو USR_TXEMP             ثنائی_ہندسہ(3)

#تعریف_کرو UART_REG(X) ((متلون uint32_t *)(UART_PPTR + (X)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب ((*UART_REG(USR) & USR_RXRDY) == 0);

    واپس *UART_REG(UTF) & 0xff;
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
