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

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
शून्य uart_drv_putchar(अचिन्हित अक्षर c)
{
    पर्यंत ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(शून्य)
{
    पर्यंत ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    परत *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
