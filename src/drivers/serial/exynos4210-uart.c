/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#وصف_ڪريو ULCON       0x0000 /* line control */
#وصف_ڪريو UCON        0x0004 /* control */
#وصف_ڪريو UFCON       0x0008 /* fifo control */
#وصف_ڪريو UMCON       0x000C /* modem control */
#وصف_ڪريو UTRSTAT     0x0010 /* TX/RX status */
#وصف_ڪريو UERSTAT     0x0014 /* RX error status */
#وصف_ڪريو UFSTAT      0x0018 /* FIFO status */
#وصف_ڪريو UMSTAT      0x001C /* modem status */
#وصف_ڪريو UTXH        0x0020 /* TX buffer */
#وصف_ڪريو URXH        0x0024 /* RX buffer */
#وصف_ڪريو UBRDIV      0x0028 /* baud rate divisor */
#وصف_ڪريو UFRACVAL    0x002C /* divisor fractional value */
#وصف_ڪريو UINTP       0x0030 /* interrupt pending */
#وصف_ڪريو UINTSP      0x0034 /* interrupt source pending */
#وصف_ڪريو UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#وصف_ڪريو TX_خالي_جايون        ٻٽو_انگ(2)
#وصف_ڪريو TXBUF_خالي_جايون     ٻٽو_انگ(1)
#وصف_ڪريو RXBUF_READY     ٻٽو_انگ(0)

#وصف_ڪريو UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما ((*UART_REG(UTRSTAT) & TXBUF_خالي_جايون) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    إذا ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        أرجع (بنانشان حرف) * UART_REG(URXH);
    } وإلا {
        أرجع -1;
    }
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
