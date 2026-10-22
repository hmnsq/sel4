/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <arch/model/statedata.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/io.h>
#ضمن <drivers/uart.h>

#إذا defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
فراغ serial_init(uint16_t پورټ)
{
    طالما (!(in8(پورټ + 5) & 0x60)); /* wait until not busy */

    out8(پورټ + 1, 0x00); /* disable generating interrupts */
    out8(پورټ + 3, 0x80); /* line control register: command: set divisor */
    out8(پورټ,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(پورټ + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(پورټ + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(پورټ + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(پورټ);     /* clear receiver port */
    in8(پورټ + 5); /* clear line status port */
    in8(پورټ + 6); /* clear modem status port */
}
#د_که_پای /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#که_تعریف_شوی CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
فراغ uart_drv_putchar(
    بینښه حرف c)
{
    طالما (x86_KSdebug_پورټ && (in8(x86_KSdebug_پورټ + 5) & 0x20) == 0);
    out8(x86_KSdebug_پورټ, c);
}

فراغ kernel_putDebugChar(بینښه حرف c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف kernel_getDebugChar(فراغ)
{
    طالما ((in8(x86_KSdebug_پورټ + 5) & 1) == 0);
    أرجع in8(x86_KSdebug_پورټ);
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
