/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UARTDR                    0x000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UARTFR                    0x018

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PL011_UARTFR_TXFF         ਦੋਆਧਾਰੀ_ਅੰਕ(5)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PL011_UARTFR_RXFE         ਦੋਆਧਾਰੀ_ਅੰਕ(4)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(x) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (x)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜਦੋਂ ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    ਵਾਪਸ *UART_REG(UARTDR);
}
#ਜੇ_ਅੰਤ  /* CONFIG_DEBUG_BUILD */
