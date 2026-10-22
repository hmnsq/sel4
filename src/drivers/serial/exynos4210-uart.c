/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>

#परिभाषय ULCON       0x0000 /* line control */
#परिभाषय UCON        0x0004 /* control */
#परिभाषय UFCON       0x0008 /* fifo control */
#परिभाषय UMCON       0x000C /* modem control */
#परिभाषय UTRSTAT     0x0010 /* TX/RX status */
#परिभाषय UERSTAT     0x0014 /* RX error status */
#परिभाषय UFSTAT      0x0018 /* FIFO status */
#परिभाषय UMSTAT      0x001C /* modem status */
#परिभाषय UTXH        0x0020 /* TX buffer */
#परिभाषय URXH        0x0024 /* RX buffer */
#परिभाषय UBRDIV      0x0028 /* baud rate divisor */
#परिभाषय UFRACVAL    0x002C /* divisor fractional value */
#परिभाषय UINTP       0x0030 /* interrupt pending */
#परिभाषय UINTSP      0x0034 /* interrupt source pending */
#परिभाषय UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#परिभाषय TX_रिक्तानि        BIT(2)
#परिभाषय TXBUF_रिक्तानि     BIT(1)
#परिभाषय RXBUF_READY     BIT(0)

#परिभाषय UART_REG(X) ((चञ्चल uint32_t *)(UART_PPTR + (X)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् ((*UART_REG(UTRSTAT) & TXBUF_रिक्तानि) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यदि ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        प्रत्यागम (अचिह्नित वर्ण) * UART_REG(URXH);
    } अन्यथा {
        प्रत्यागम -1;
    }
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
