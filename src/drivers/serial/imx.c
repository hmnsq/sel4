/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ URXD  0x00 /* UART Receiver Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTXD  0x40 /* UART Transmitter Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCR1  0x80 /* UART Control Register 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCR2  0x84 /* UART Control Register 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCR3  0x88 /* UART Control Register 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCR4  0x8c /* UART Control Register 4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UFCR  0x90 /* UART FIFO Control Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR1  0x94 /* UART Status Register 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR2  0x98 /* UART Status Register 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UESC  0x9c /* UART Escape Character Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTIM  0xa0 /* UART Escape Timer Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIR  0xa4 /* UART BRM Incremental Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBMR  0xa8 /* UART BRM Modulator Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBRC  0xac /* UART Baud Rate Counter Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ONEMS 0xb0 /* UART One Millisecond Register */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTS   0xb4 /* UART Test Register */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_SR1_TRDY         ਦੋਆਧਾਰੀ_ਅੰਕ(13)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_SR1_RRDY         ਦੋਆਧਾਰੀ_ਅੰਕ(9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_SR2_TXFIFO_ਖਾਲੀ_ਥਾਵਾਂ ਦੋਆਧਾਰੀ_ਅੰਕ(14)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_SR2_RXFIFO_RDR   ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(x) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (x)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ (!(*UART_REG(USR2) & UART_SR2_TXFIFO_ਖਾਲੀ_ਥਾਵਾਂ));
    *UART_REG(UTXD) = c;
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜਦੋਂ (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    ਵਾਪਸ *UART_REG(URXD);
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
