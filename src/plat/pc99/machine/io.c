/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot_sys.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/uart.h>

#ਜੇ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ਖਾਲੀ serial_init(uint16_t ਪੋਰਟ)
{
    ਜਦੋਂ (!(in8(ਪੋਰਟ + 5) & 0x60)); /* wait until not busy */

    out8(ਪੋਰਟ + 1, 0x00); /* disable generating interrupts */
    out8(ਪੋਰਟ + 3, 0x80); /* line control register: command: set divisor */
    out8(ਪੋਰਟ,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(ਪੋਰਟ + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(ਪੋਰਟ + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(ਪੋਰਟ + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(ਪੋਰਟ);     /* clear receiver port */
    in8(ਪੋਰਟ + 5); /* clear line status port */
    in8(ਪੋਰਟ + 6); /* clear modem status port */
}
#ਜੇ_ਅੰਤ /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
ਖਾਲੀ uart_drv_putchar(
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ (x86_KSdebug_ਪੋਰਟ && (in8(x86_KSdebug_ਪੋਰਟ + 5) & 0x20) == 0);
    out8(x86_KSdebug_ਪੋਰਟ, c);
}

ਖਾਲੀ kernel_putDebugChar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ kernel_getDebugChar(ਖਾਲੀ)
{
    ਜਦੋਂ ((in8(x86_KSdebug_ਪੋਰਟ + 5) & 1) == 0);
    ਵਾਪਸ in8(x86_KSdebug_ਪੋਰਟ);
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
