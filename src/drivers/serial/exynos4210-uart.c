/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#define ULCON       0x0000 /* line control */
#define UCON        0x0004 /* control */
#define UFCON       0x0008 /* fifo control */
#define UMCON       0x000C /* modem control */
#define UTRSTAT     0x0010 /* TX/RX status */
#define UERSTAT     0x0014 /* RX error status */
#define UFSTAT      0x0018 /* FIFO status */
#define UMSTAT      0x001C /* modem status */
#define UTXH        0x0020 /* TX buffer */
#define URXH        0x0024 /* RX buffer */
#define UBRDIV      0x0028 /* baud rate divisor */
#define UFRACVAL    0x002C /* divisor fractional value */
#define UINTP       0x0030 /* interrupt pending */
#define UINTSP      0x0034 /* interrupt source pending */
#define UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#define TX_EMPTY        BIT(2)
#define TXBUF_EMPTY     BIT(1)
#define RXBUF_READY     BIT(0)

#define UART_REG(X) ((ಚಞ್ಚಲ uint32_t *)(UART_PPTR + (X)))

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(unsigned ವರ್ಣ c)
{
    ಯಾವತ್ ((*UART_REG(UTRSTAT) & TXBUF_EMPTY) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಯದಿ ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ) * UART_REG(URXH);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}
#endif /* CONFIG_DEBUG_BUILD */
