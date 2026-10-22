/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/kernel/boot_sys.h>
#شامل <arch/model/statedata.h>
#شامل <machine/io.h>
#شامل <plat/machine/io.h>
#شامل <drivers/uart.h>

#اگر defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
خالی serial_init(uint16_t پورٹ)
{
    جب (!(in8(پورٹ + 5) & 0x60)); /* wait until not busy */

    out8(پورٹ + 1, 0x00); /* disable generating interrupts */
    out8(پورٹ + 3, 0x80); /* line control register: command: set divisor */
    out8(پورٹ,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(پورٹ + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(پورٹ + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(پورٹ + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(پورٹ);     /* clear receiver port */
    in8(پورٹ + 5); /* clear line status port */
    in8(پورٹ + 6); /* clear modem status port */
}
#اگر_ختم /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#اگر_تعریف_شدہ CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
خالی uart_drv_putchar(
    غیرعلامتی حرف c)
{
    جب (x86_KSdebug_پورٹ && (in8(x86_KSdebug_پورٹ + 5) & 0x20) == 0);
    out8(x86_KSdebug_پورٹ, c);
}

خالی kernel_putDebugChar(غیرعلامتی حرف c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف kernel_getDebugChar(خالی)
{
    جب ((in8(x86_KSdebug_پورٹ + 5) & 1) == 0);
    واپس in8(x86_KSdebug_پورٹ);
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
