/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULCON       0x0000 /* line control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCON        0x0004 /* control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UFCON       0x0008 /* fifo control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMCON       0x000C /* modem control */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTRSTAT     0x0010 /* TX/RX status */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UERSTAT     0x0014 /* RX error status */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UFSTAT      0x0018 /* FIFO status */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMSTAT      0x001C /* modem status */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTXH        0x0020 /* TX buffer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ URXH        0x0024 /* RX buffer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBRDIV      0x0028 /* baud rate divisor */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UFRACVAL    0x002C /* divisor fractional value */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINTP       0x0030 /* interrupt pending */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINTSP      0x0034 /* interrupt source pending */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TX_ਖਾਲੀ_ਥਾਵਾਂ        ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TXBUF_ਖਾਲੀ_ਥਾਵਾਂ     ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RXBUF_READY     ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(X) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (X)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ ((*UART_REG(UTRSTAT) & TXBUF_ਖਾਲੀ_ਥਾਵਾਂ) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜੇ ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) * UART_REG(URXH);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -1;
    }
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
