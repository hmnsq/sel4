/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#تعریف_کرو USR                   0x08
#تعریف_کرو UTF                   0x70
#تعریف_کرو UNTX                  0x40

#تعریف_کرو USR_RXRDY             ثنائی_ہندسہ(0)
#تعریف_کرو USR_RXFUL             ثنائی_ہندسہ(1)
#تعریف_کرو USR_TXRDY             ثنائی_ہندسہ(2)
#تعریف_کرو USR_TXEMP             ثنائی_ہندسہ(3)

#تعریف_کرو UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشانی حرف c)
{
    طالما ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(USR) & USR_RXRDY) == 0);

    أرجع *UART_REG(UTF) & 0xff;
}
#جے_انت /* CONFIG_DEBUG_BUILD */
