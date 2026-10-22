/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>

#define USR                   0x08
#define UTF                   0x70
#define UNTX                  0x40

#define USR_RXRDY             BIT(0)
#define USR_RXFUL             BIT(1)
#define USR_TXRDY             BIT(2)
#define USR_TXEMP             BIT(3)

#define UART_REG(X) ((चञ्चल uint32_t *)(UART_PPTR + (X)))

#ifdef CONFIG_PRINTING
शून्य uart_drv_putchar(unsigned वर्ण c)
{
    याव ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned वर्ण uart_drv_getchar(शून्य)
{
    याव ((*UART_REG(USR) & USR_RXRDY) == 0);

    पटिनिवत्त *UART_REG(UTF) & 0xff;
}
#endif /* CONFIG_DEBUG_BUILD */
