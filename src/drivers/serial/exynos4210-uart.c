/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#تعریف_کړه ULCON       0x0000 /* line control */
#تعریف_کړه UCON        0x0004 /* control */
#تعریف_کړه UFCON       0x0008 /* fifo control */
#تعریف_کړه UMCON       0x000C /* modem control */
#تعریف_کړه UTRSTAT     0x0010 /* TX/RX status */
#تعریف_کړه UERSTAT     0x0014 /* RX error status */
#تعریف_کړه UFSTAT      0x0018 /* FIFO status */
#تعریف_کړه UMSTAT      0x001C /* modem status */
#تعریف_کړه UTXH        0x0020 /* TX buffer */
#تعریف_کړه URXH        0x0024 /* RX buffer */
#تعریف_کړه UBRDIV      0x0028 /* baud rate divisor */
#تعریف_کړه UFRACVAL    0x002C /* divisor fractional value */
#تعریف_کړه UINTP       0x0030 /* interrupt pending */
#تعریف_کړه UINTSP      0x0034 /* interrupt source pending */
#تعریف_کړه UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#تعریف_کړه TX_تشه        بټ(2)
#تعریف_کړه TXBUF_تشه     بټ(1)
#تعریف_کړه RXBUF_READY     بټ(0)

#تعریف_کړه UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما ((*UART_REG(UTRSTAT) & TXBUF_تشه) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    إذا ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        أرجع (بینښه حرف) * UART_REG(URXH);
    } وإلا {
        أرجع -1;
    }
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
