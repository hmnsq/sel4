/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#تعریف_کرو UTHR        0x0
#تعریف_کرو ULSR        0x14

#تعریف_کرو ULSR_THRE   ثنائی_ہندسہ(5)
#تعریف_کرو ULSR_RDR    ثنائی_ہندسہ(0)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    واپس *UART_REG(UTHR);
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
