/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#تعریف_کن USR                   0x08
#تعریف_کن UTF                   0x70
#تعریف_کن UNTX                  0x40

#تعریف_کن USR_RXRDY             بیت(0)
#تعریف_کن USR_RXFUL             بیت(1)
#تعریف_کن USR_TXRDY             بیت(2)
#تعریف_کن USR_TXEMP             بیت(3)

#تعریف_کن UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(USR) & USR_RXRDY) == 0);

    أرجع *UART_REG(UTF) & 0xff;
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
