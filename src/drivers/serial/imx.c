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

#تعریف_کن URXD  0x00 /* UART Receiver Register */
#تعریف_کن UTXD  0x40 /* UART Transmitter Register */
#تعریف_کن UCR1  0x80 /* UART Control Register 1 */
#تعریف_کن UCR2  0x84 /* UART Control Register 2 */
#تعریف_کن UCR3  0x88 /* UART Control Register 3 */
#تعریف_کن UCR4  0x8c /* UART Control Register 4 */
#تعریف_کن UFCR  0x90 /* UART FIFO Control Register */
#تعریف_کن USR1  0x94 /* UART Status Register 1 */
#تعریف_کن USR2  0x98 /* UART Status Register 2 */
#تعریف_کن UESC  0x9c /* UART Escape Character Register */
#تعریف_کن UTIM  0xa0 /* UART Escape Timer Register */
#تعریف_کن UBIR  0xa4 /* UART BRM Incremental Register */
#تعریف_کن UBMR  0xa8 /* UART BRM Modulator Register */
#تعریف_کن UBRC  0xac /* UART Baud Rate Counter Register */
#تعریف_کن ONEMS 0xb0 /* UART One Millisecond Register */
#تعریف_کن UTS   0xb4 /* UART Test Register */

#تعریف_کن UART_SR1_TRDY         بیت(13)
#تعریف_کن UART_SR1_RRDY         بیت(9)
#تعریف_کن UART_SR2_TXFIFO_خالی بیت(14)
#تعریف_کن UART_SR2_RXFIFO_RDR   بیت(0)

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما (!(*UART_REG(USR2) & UART_SR2_TXFIFO_خالی));
    *UART_REG(UTXD) = c;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    أرجع *UART_REG(URXD);
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
