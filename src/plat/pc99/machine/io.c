/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <arch/kernel/boot_sys.h>
#समावेश <arch/model/statedata.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/io.h>
#समावेश <drivers/uart.h>

#अगर defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
व्योम serial_init(uint16_t port)
{
    जबतक (!(in8(port + 5) & 0x60)); /* wait until not busy */

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
#पूर्ण_अगर /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#अगर_घोषित CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
व्योम uart_drv_putchar(
    अचिन्हित अक्षर c)
{
    जबतक (x86KSdebugPort && (in8(x86KSdebugPort + 5) & 0x20) == 0);
    out8(x86KSdebugPort, c);
}

व्योम kernel_putDebugChar(अचिन्हित अक्षर c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर kernel_getDebugChar(व्योम)
{
    जबतक ((in8(x86KSdebugPort + 5) & 1) == 0);
    वापस in8(x86KSdebugPort);
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
