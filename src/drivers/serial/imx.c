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

#وصف_ڪريو URXD  0x00 /* UART Receiver Register */
#وصف_ڪريو UTXD  0x40 /* UART Transmitter Register */
#وصف_ڪريو UCR1  0x80 /* UART Control Register 1 */
#وصف_ڪريو UCR2  0x84 /* UART Control Register 2 */
#وصف_ڪريو UCR3  0x88 /* UART Control Register 3 */
#وصف_ڪريو UCR4  0x8c /* UART Control Register 4 */
#وصف_ڪريو UFCR  0x90 /* UART FIFO Control Register */
#وصف_ڪريو USR1  0x94 /* UART Status Register 1 */
#وصف_ڪريو USR2  0x98 /* UART Status Register 2 */
#وصف_ڪريو UESC  0x9c /* UART Escape Character Register */
#وصف_ڪريو UTIM  0xa0 /* UART Escape Timer Register */
#وصف_ڪريو UBIR  0xa4 /* UART BRM Incremental Register */
#وصف_ڪريو UBMR  0xa8 /* UART BRM Modulator Register */
#وصف_ڪريو UBRC  0xac /* UART Baud Rate Counter Register */
#وصف_ڪريو ONEMS 0xb0 /* UART One Millisecond Register */
#وصف_ڪريو UTS   0xb4 /* UART Test Register */

#وصف_ڪريو UART_SR1_TRDY         ٻٽو_انگ(13)
#وصف_ڪريو UART_SR1_RRDY         ٻٽو_انگ(9)
#وصف_ڪريو UART_SR2_TXFIFO_خالي_جايون ٻٽو_انگ(14)
#وصف_ڪريو UART_SR2_RXFIFO_RDR   ٻٽو_انگ(0)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما (!(*UART_REG(USR2) & UART_SR2_TXFIFO_خالي_جايون));
    *UART_REG(UTXD) = c;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    أرجع *UART_REG(URXD);
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
