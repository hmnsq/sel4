/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <machine/io.h>

#define USR                   0x08
#define UTF                   0x70
#define UNTX                  0x40

#define USR_RXRDY             BIT(0)
#define USR_RXFUL             BIT(1)
#define USR_TXRDY             BIT(2)
#define USR_TXEMP             BIT(3)

#define UART_REG(X) ((நிலையற்ற uint32_t *)(UART_PPTR + (X)))

#ifdef CONFIG_PRINTING
வெறுமை uart_drv_putchar(unsigned எழுத்து c)
{
    போது ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned எழுத்து uart_drv_getchar(வெறுமை)
{
    போது ((*UART_REG(USR) & USR_RXRDY) == 0);

    திரும்பு *UART_REG(UTF) & 0xff;
}
#endif /* CONFIG_DEBUG_BUILD */
