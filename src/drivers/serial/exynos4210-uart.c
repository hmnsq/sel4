/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>

#تعریف_کرو ULCON       0x0000 /* line control */
#تعریف_کرو UCON        0x0004 /* control */
#تعریف_کرو UFCON       0x0008 /* fifo control */
#تعریف_کرو UMCON       0x000C /* modem control */
#تعریف_کرو UTRSTAT     0x0010 /* TX/RX status */
#تعریف_کرو UERSTAT     0x0014 /* RX error status */
#تعریف_کرو UFSTAT      0x0018 /* FIFO status */
#تعریف_کرو UMSTAT      0x001C /* modem status */
#تعریف_کرو UTXH        0x0020 /* TX buffer */
#تعریف_کرو URXH        0x0024 /* RX buffer */
#تعریف_کرو UBRDIV      0x0028 /* baud rate divisor */
#تعریف_کرو UFRACVAL    0x002C /* divisor fractional value */
#تعریف_کرو UINTP       0x0030 /* interrupt pending */
#تعریف_کرو UINTSP      0x0034 /* interrupt source pending */
#تعریف_کرو UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#تعریف_کرو TX_خالی_خانے        ثنائی_ہندسہ(2)
#تعریف_کرو TXBUF_خالی_خانے     ثنائی_ہندسہ(1)
#تعریف_کرو RXBUF_READY     ثنائی_ہندسہ(0)

#تعریف_کرو UART_REG(X) ((متلون uint32_t *)(UART_PPTR + (X)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب ((*UART_REG(UTRSTAT) & TXBUF_خالی_خانے) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    اگر ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        واپس (غیرعلامتی حرف) * UART_REG(URXH);
    } ورنہ {
        واپس -1;
    }
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
