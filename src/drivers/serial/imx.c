/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#परिभाषय URXD  0x00 /* UART Receiver Register */
#परिभाषय UTXD  0x40 /* UART Transmitter Register */
#परिभाषय UCR1  0x80 /* UART Control Register 1 */
#परिभाषय UCR2  0x84 /* UART Control Register 2 */
#परिभाषय UCR3  0x88 /* UART Control Register 3 */
#परिभाषय UCR4  0x8c /* UART Control Register 4 */
#परिभाषय UFCR  0x90 /* UART FIFO Control Register */
#परिभाषय USR1  0x94 /* UART Status Register 1 */
#परिभाषय USR2  0x98 /* UART Status Register 2 */
#परिभाषय UESC  0x9c /* UART Escape Character Register */
#परिभाषय UTIM  0xa0 /* UART Escape Timer Register */
#परिभाषय UBIR  0xa4 /* UART BRM Incremental Register */
#परिभाषय UBMR  0xa8 /* UART BRM Modulator Register */
#परिभाषय UBRC  0xac /* UART Baud Rate Counter Register */
#परिभाषय ONEMS 0xb0 /* UART One Millisecond Register */
#परिभाषय UTS   0xb4 /* UART Test Register */

#परिभाषय UART_SR1_TRDY         BIT(13)
#परिभाषय UART_SR1_RRDY         BIT(9)
#परिभाषय UART_SR2_TXFIFO_रिक्तानि BIT(14)
#परिभाषय UART_SR2_RXFIFO_RDR   BIT(0)

#परिभाषय UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् (!(*UART_REG(USR2) & UART_SR2_TXFIFO_रिक्तानि));
    *UART_REG(UTXD) = c;
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    प्रत्यागम *UART_REG(URXD);
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
