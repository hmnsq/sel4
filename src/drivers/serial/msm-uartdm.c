/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>

#परिभाषय USR                   0x08
#परिभाषय UTF                   0x70
#परिभाषय UNTX                  0x40

#परिभाषय USR_RXRDY             BIT(0)
#परिभाषय USR_RXFUL             BIT(1)
#परिभाषय USR_TXRDY             BIT(2)
#परिभाषय USR_TXEMP             BIT(3)

#परिभाषय UART_REG(X) ((चञ्चल uint32_t *)(UART_PPTR + (X)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् ((*UART_REG(USR) & USR_RXRDY) == 0);

    प्रत्यागम *UART_REG(UTF) & 0xff;
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
