/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_WFIFO  0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RFIFO  0x4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਸਥਿਤੀ 0xC

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TX_ਭਰਿਆ        ਦੋਆਧਾਰੀ_ਅੰਕ(21)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RX_ਖਾਲੀ_ਥਾਵਾਂ       ਦੋਆਧਾਰੀ_ਅੰਕ(20)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(x) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (x)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ ((*UART_REG(UART_ਸਥਿਤੀ) & UART_TX_ਭਰਿਆ));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜਦੋਂ ((*UART_REG(UART_ਸਥਿਤੀ) & UART_RX_ਖਾਲੀ_ਥਾਵਾਂ));
    ਵਾਪਸ *UART_REG(UART_RFIFO);
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
