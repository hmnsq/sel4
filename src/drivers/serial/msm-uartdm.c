/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#تعریف_کړه USR                   0x08
#تعریف_کړه UTF                   0x70
#تعریف_کړه UNTX                  0x40

#تعریف_کړه USR_RXRDY             بټ(0)
#تعریف_کړه USR_RXFUL             بټ(1)
#تعریف_کړه USR_TXRDY             بټ(2)
#تعریف_کړه USR_TXEMP             بټ(3)

#تعریف_کړه UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(USR) & USR_RXRDY) == 0);

    أرجع *UART_REG(UTF) & 0xff;
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
