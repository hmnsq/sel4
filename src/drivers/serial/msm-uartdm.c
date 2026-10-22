/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR                   0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTF                   0x70
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNTX                  0x40

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_RXRDY             ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_RXFUL             ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_TXRDY             ਦੋਆਧਾਰੀ_ਅੰਕ(2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_TXEMP             ਦੋਆਧਾਰੀ_ਅੰਕ(3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(X) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (X)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜਦੋਂ ((*UART_REG(USR) & USR_RXRDY) == 0);

    ਵਾਪਸ *UART_REG(UTF) & 0xff;
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
