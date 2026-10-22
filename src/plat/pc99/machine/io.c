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
فراغ serial_init(uint16_t پورٽ)
{
    طالما (!(in8(پورٽ + 5) & 0x60)); /* wait until not busy */

    out8(پورٽ + 1, 0x00); /* disable generating interrupts */
    out8(پورٽ + 3, 0x80); /* line control register: command: set divisor */
    out8(پورٽ,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(پورٽ + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(پورٽ + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(پورٽ + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(پورٽ);     /* clear receiver port */
    in8(پورٽ + 5); /* clear line status port */
    in8(پورٽ + 6); /* clear modem status port */
}
#جيڪڏهن_پورو /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
فراغ uart_drv_putchar(
    بنانشان حرف c)
{
    طالما (x86_KSdebug_پورٽ && (in8(x86_KSdebug_پورٽ + 5) & 0x20) == 0);
    out8(x86_KSdebug_پورٽ, c);
}

فراغ kernel_putDebugChar(بنانشان حرف c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف kernel_getDebugChar(فراغ)
{
    طالما ((in8(x86_KSdebug_پورٽ + 5) & 1) == 0);
    أرجع in8(x86_KSdebug_پورٽ);
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
