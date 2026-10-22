/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#تعریف_کړه URXD  0x00 /* UART Receiver Register */
#تعریف_کړه UTXD  0x40 /* UART Transmitter Register */
#تعریف_کړه UCR1  0x80 /* UART Control Register 1 */
#تعریف_کړه UCR2  0x84 /* UART Control Register 2 */
#تعریف_کړه UCR3  0x88 /* UART Control Register 3 */
#تعریف_کړه UCR4  0x8c /* UART Control Register 4 */
#تعریف_کړه UFCR  0x90 /* UART FIFO Control Register */
#تعریف_کړه USR1  0x94 /* UART Status Register 1 */
#تعریف_کړه USR2  0x98 /* UART Status Register 2 */
#تعریف_کړه UESC  0x9c /* UART Escape Character Register */
#تعریف_کړه UTIM  0xa0 /* UART Escape Timer Register */
#تعریف_کړه UBIR  0xa4 /* UART BRM Incremental Register */
#تعریف_کړه UBMR  0xa8 /* UART BRM Modulator Register */
#تعریف_کړه UBRC  0xac /* UART Baud Rate Counter Register */
#تعریف_کړه ONEMS 0xb0 /* UART One Millisecond Register */
#تعریف_کړه UTS   0xb4 /* UART Test Register */

#تعریف_کړه UART_SR1_TRDY         بټ(13)
#تعریف_کړه UART_SR1_RRDY         بټ(9)
#تعریف_کړه UART_SR2_TXFIFO_تشه بټ(14)
#تعریف_کړه UART_SR2_RXFIFO_RDR   بټ(0)

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما (!(*UART_REG(USR2) & UART_SR2_TXFIFO_تشه));
    *UART_REG(UTXD) = c;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    أرجع *UART_REG(URXD);
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
