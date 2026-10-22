/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>

#घोषणा ULCON       0x0000 /* line control */
#घोषणा UCON        0x0004 /* control */
#घोषणा UFCON       0x0008 /* fifo control */
#घोषणा UMCON       0x000C /* modem control */
#घोषणा UTRSTAT     0x0010 /* TX/RX status */
#घोषणा UERSTAT     0x0014 /* RX error status */
#घोषणा UFSTAT      0x0018 /* FIFO status */
#घोषणा UMSTAT      0x001C /* modem status */
#घोषणा UTXH        0x0020 /* TX buffer */
#घोषणा URXH        0x0024 /* RX buffer */
#घोषणा UBRDIV      0x0028 /* baud rate divisor */
#घोषणा UFRACVAL    0x002C /* divisor fractional value */
#घोषणा UINTP       0x0030 /* interrupt pending */
#घोषणा UINTSP      0x0034 /* interrupt source pending */
#घोषणा UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#घोषणा TX_EMPTY        BIT(2)
#घोषणा TXBUF_EMPTY     BIT(1)
#घोषणा RXBUF_READY     BIT(0)

#घोषणा UART_REG(X) ((अस्थिर uint32_t *)(UART_PPTR + (X)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक ((*UART_REG(UTRSTAT) & TXBUF_EMPTY) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    अगर ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        वापस (अचिन्हित अक्षर) * UART_REG(URXH);
    } अन्यथा {
        वापस -1;
    }
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
