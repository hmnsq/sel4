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

#define UART_WFIFO  0x0
#define UART_RFIFO  0x4
#define UART_STATUS 0xC

#define UART_TX_FULL        BIT(21)
#define UART_RX_EMPTY       BIT(20)

#define UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
शून्य uart_drv_putchar(unsigned वर्ण c)
{
    जाव ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned वर्ण uart_drv_getchar(शून्य)
{
    जाव ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    प्रत्यागम *UART_REG(UART_RFIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
