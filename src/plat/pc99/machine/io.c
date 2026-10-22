/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <arch/kernel/boot_sys.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/io.h>
#അന്തര്ഭാവ <drivers/uart.h>

#എങ്കിൽ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ശൂന്യം serial_init(uint16_t port)
{
    ആകുമ്പോൾ (!(in8(port + 5) & 0x60)); /* wait until not busy */

    out8(port + 1, 0x00); /* disable generating interrupts */
    out8(port + 3, 0x80); /* line control register: command: set divisor */
    out8(port,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(port + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(port + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(port + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(port);     /* clear receiver port */
    in8(port + 5); /* clear line status port */
    in8(port + 6); /* clear modem status port */
}
#endif /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#ifdef CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
ശൂന്യം uart_drv_putchar(
    unsigned അക്ഷരം c)
{
    ആകുമ്പോൾ (x86KSdebugPort && (in8(x86KSdebugPort + 5) & 0x20) == 0);
    out8(x86KSdebugPort, c);
}

ശൂന്യം kernel_putDebugChar(unsigned അക്ഷരം c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം kernel_getDebugChar(ശൂന്യം)
{
    ആകുമ്പോൾ ((in8(x86KSdebugPort + 5) & 1) == 0);
    തിരികെ in8(x86KSdebugPort);
}
#endif /* CONFIG_DEBUG_BUILD */
