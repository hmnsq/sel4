/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

#تعریف_کن ULCON       0x0000 /* line control */
#تعریف_کن UCON        0x0004 /* control */
#تعریف_کن UFCON       0x0008 /* fifo control */
#تعریف_کن UMCON       0x000C /* modem control */
#تعریف_کن UTRSTAT     0x0010 /* TX/RX status */
#تعریف_کن UERSTAT     0x0014 /* RX error status */
#تعریف_کن UFSTAT      0x0018 /* FIFO status */
#تعریف_کن UMSTAT      0x001C /* modem status */
#تعریف_کن UTXH        0x0020 /* TX buffer */
#تعریف_کن URXH        0x0024 /* RX buffer */
#تعریف_کن UBRDIV      0x0028 /* baud rate divisor */
#تعریف_کن UFRACVAL    0x002C /* divisor fractional value */
#تعریف_کن UINTP       0x0030 /* interrupt pending */
#تعریف_کن UINTSP      0x0034 /* interrupt source pending */
#تعریف_کن UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#تعریف_کن TX_خالی        بیت(2)
#تعریف_کن TXBUF_خالی     بیت(1)
#تعریف_کن RXBUF_READY     بیت(0)

#تعریف_کن UART_REG(X) ((متطاير uint32_t *)(UART_PPTR + (X)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما ((*UART_REG(UTRSTAT) & TXBUF_خالی) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    إذا ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        أرجع (بیعلامت حرف) * UART_REG(URXH);
    } وإلا {
        أرجع -1;
    }
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
