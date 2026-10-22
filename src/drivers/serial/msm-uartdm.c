/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#وصف_ڪريو USR                   0x08
#وصف_ڪريو UTF                   0x70
#وصف_ڪريو UNTX                  0x40

#وصف_ڪريو USR_RXRDY             ٻٽو_انگ(0)
#وصف_ڪريو USR_RXFUL             ٻٽو_انگ(1)
#وصف_ڪريو USR_TXRDY             ٻٽو_انگ(2)
#وصف_ڪريو USR_TXEMP             ٻٽو_انگ(3)

#وصف_ڪريو UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(USR) & USR_RXRDY) == 0);

    أرجع *UART_REG(UTF) & 0xff;
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
