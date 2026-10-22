/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#تعریف_کرو URXD  0x00 /* UART Receiver Register */
#تعریف_کرو UTXD  0x40 /* UART Transmitter Register */
#تعریف_کرو UCR1  0x80 /* UART Control Register 1 */
#تعریف_کرو UCR2  0x84 /* UART Control Register 2 */
#تعریف_کرو UCR3  0x88 /* UART Control Register 3 */
#تعریف_کرو UCR4  0x8c /* UART Control Register 4 */
#تعریف_کرو UFCR  0x90 /* UART FIFO Control Register */
#تعریف_کرو USR1  0x94 /* UART Status Register 1 */
#تعریف_کرو USR2  0x98 /* UART Status Register 2 */
#تعریف_کرو UESC  0x9c /* UART Escape Character Register */
#تعریف_کرو UTIM  0xa0 /* UART Escape Timer Register */
#تعریف_کرو UBIR  0xa4 /* UART BRM Incremental Register */
#تعریف_کرو UBMR  0xa8 /* UART BRM Modulator Register */
#تعریف_کرو UBRC  0xac /* UART Baud Rate Counter Register */
#تعریف_کرو ONEMS 0xb0 /* UART One Millisecond Register */
#تعریف_کرو UTS   0xb4 /* UART Test Register */

#تعریف_کرو UART_SR1_TRDY         ثنائی_ہندسہ(13)
#تعریف_کرو UART_SR1_RRDY         ثنائی_ہندسہ(9)
#تعریف_کرو UART_SR2_TXFIFO_خالی_خانے ثنائی_ہندسہ(14)
#تعریف_کرو UART_SR2_RXFIFO_RDR   ثنائی_ہندسہ(0)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب (!(*UART_REG(USR2) & UART_SR2_TXFIFO_خالی_خانے));
    *UART_REG(UTXD) = c;
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    واپس *UART_REG(URXD);
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
