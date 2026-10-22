/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/devices_gen.h>

#define UTHR        0x0
#define ULSR        0x14

#define ULSR_THRE   BIT(5)
#define ULSR_RDR    BIT(0)

#define UART_REG(x) ((நிலையற்ற uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
வெறுமை uart_drv_putchar(unsigned எழுத்து c)
{
    போது ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned எழுத்து uart_drv_getchar(வெறுமை)
{
    போது ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    திரும்பு *UART_REG(UTHR);
}
#endif /* CONFIG_DEBUG_BUILD */
