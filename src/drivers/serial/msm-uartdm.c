/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <machine/io.h>

#define USR                   0x08
#define UTF                   0x70
#define UNTX                  0x40

#define USR_RXRDY             BIT(0)
#define USR_RXFUL             BIT(1)
#define USR_TXRDY             BIT(2)
#define USR_TXEMP             BIT(3)

#define UART_REG(X) ((চঞ্চল uint32_t *)(UART_PPTR + (X)))

#ifdef CONFIG_PRINTING
শূন্য uart_drv_putchar(unsigned বর্ণ c)
{
    যাবত্ ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned বর্ণ uart_drv_getchar(শূন্য)
{
    যাবত্ ((*UART_REG(USR) & USR_RXRDY) == 0);

    প্রত্যাগম *UART_REG(UTF) & 0xff;
}
#endif /* CONFIG_DEBUG_BUILD */
