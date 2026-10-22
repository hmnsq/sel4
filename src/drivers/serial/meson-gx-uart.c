/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/devices_gen.h>

#define UART_WFIFO  0x0
#define UART_RFIFO  0x4
#define UART_STATUS 0xC

#define UART_TX_FULL        BIT(21)
#define UART_RX_EMPTY       BIT(20)

#define UART_REG(x) ((நிலையற்ற uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
வெறுமை uart_drv_putchar(unsigned எழுத்து c)
{
    போது ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned எழுத்து uart_drv_getchar(வெறுமை)
{
    போது ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    திரும்பு *UART_REG(UART_RFIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
