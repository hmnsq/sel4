/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#تعریف_کرو UART_WFIFO  0x0
#تعریف_کرو UART_RFIFO  0x4
#تعریف_کرو UART_حالت 0xC

#تعریف_کرو UART_TX_بھریا        ثنائی_ہندسہ(21)
#تعریف_کرو UART_RX_خالی_تھاواں       ثنائی_ہندسہ(20)

#تعریف_کرو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشانی حرف c)
{
    طالما ((*UART_REG(UART_حالت) & UART_TX_بھریا));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UART_حالت) & UART_RX_خالی_تھاواں));
    أرجع *UART_REG(UART_RFIFO);
}
#جے_انت /* CONFIG_DEBUG_BUILD */
