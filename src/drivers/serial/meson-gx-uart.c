/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#परिभाषय UART_WFIFO  0x0
#परिभाषय UART_RFIFO  0x4
#परिभाषय UART_स्थितिः 0xC

#परिभाषय UART_TX_पूर्णम्        BIT(21)
#परिभाषय UART_RX_रिक्तानि       BIT(20)

#परिभाषय UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् ((*UART_REG(UART_स्थितिः) & UART_TX_पूर्णम्));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् ((*UART_REG(UART_स्थितिः) & UART_RX_रिक्तानि));
    प्रत्यागम *UART_REG(UART_RFIFO);
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
