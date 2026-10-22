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

#وصف_ڪريو UART_WFIFO  0x0
#وصف_ڪريو UART_RFIFO  0x4
#وصف_ڪريو UART_حالت 0xC

#وصف_ڪريو UART_TX_ڀريل        ٻٽو_انگ(21)
#وصف_ڪريو UART_RX_خالي_جايون       ٻٽو_انگ(20)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما ((*UART_REG(UART_حالت) & UART_TX_ڀريل));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما ((*UART_REG(UART_حالت) & UART_RX_خالي_جايون));
    أرجع *UART_REG(UART_RFIFO);
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
