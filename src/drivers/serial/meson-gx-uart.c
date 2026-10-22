/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#define UART_WFIFO  0x0
#define UART_RFIFO  0x4
#define UART_STATUS 0xC

#define UART_TX_FULL        BIT(21)
#define UART_RX_EMPTY       BIT(20)

#define UART_REG(x) ((ಚಞ್ಚಲ uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(unsigned ವರ್ಣ c)
{
    ಯಾವತ್ ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಯಾವತ್ ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    ಪ್ರತ್ಯಾಗಮ *UART_REG(UART_RFIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
