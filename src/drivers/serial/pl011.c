/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((ಚಞ್ಚಲ uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(unsigned ವರ್ಣ c)
{
    ಯಾವತ್ ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಯಾವತ್ ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    ಪ್ರತ್ಯಾಗಮ *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
