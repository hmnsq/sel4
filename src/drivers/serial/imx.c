/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/devices_gen.h>

#घोषणा URXD  0x00 /* UART Receiver Register */
#घोषणा UTXD  0x40 /* UART Transmitter Register */
#घोषणा UCR1  0x80 /* UART Control Register 1 */
#घोषणा UCR2  0x84 /* UART Control Register 2 */
#घोषणा UCR3  0x88 /* UART Control Register 3 */
#घोषणा UCR4  0x8c /* UART Control Register 4 */
#घोषणा UFCR  0x90 /* UART FIFO Control Register */
#घोषणा USR1  0x94 /* UART Status Register 1 */
#घोषणा USR2  0x98 /* UART Status Register 2 */
#घोषणा UESC  0x9c /* UART Escape Character Register */
#घोषणा UTIM  0xa0 /* UART Escape Timer Register */
#घोषणा UBIR  0xa4 /* UART BRM Incremental Register */
#घोषणा UBMR  0xa8 /* UART BRM Modulator Register */
#घोषणा UBRC  0xac /* UART Baud Rate Counter Register */
#घोषणा ONEMS 0xb0 /* UART One Millisecond Register */
#घोषणा UTS   0xb4 /* UART Test Register */

#घोषणा UART_SR1_TRDY         BIT(13)
#घोषणा UART_SR1_RRDY         BIT(9)
#घोषणा UART_SR2_TXFIFO_EMPTY BIT(14)
#घोषणा UART_SR2_RXFIFO_RDR   BIT(0)

#घोषणा UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक (!(*UART_REG(USR2) & UART_SR2_TXFIFO_EMPTY));
    *UART_REG(UTXD) = c;
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    जबतक (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    वापस *UART_REG(URXD);
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
