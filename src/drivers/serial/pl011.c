/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#परिभाषय UARTDR                    0x000
#परिभाषय UARTFR                    0x018

#परिभाषय PL011_UARTFR_TXFF         BIT(5)
#परिभाषय PL011_UARTFR_RXFE         BIT(4)

#परिभाषय UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    प्रत्यागम *UART_REG(UARTDR);
}
#यदि_अन्तः  /* CONFIG_DEBUG_BUILD */
