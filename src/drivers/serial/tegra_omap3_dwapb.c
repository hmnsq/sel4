/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((અસ્થિર uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
શૂન્ય uart_drv_putchar(unsigned અક્ષર c)
{
    જ્યારે ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned અક્ષર uart_drv_getchar(શૂન્ય)
{
    જ્યારે ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    પાછા *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
