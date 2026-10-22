/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/kernel/boot_sys.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/io.h>
#अन्तर्भाव <drivers/uart.h>

#यदि defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
शून्य serial_init(uint16_t पत्तनम्)
{
    यावत् (!(in8(पत्तनम् + 5) & 0x60)); /* wait until not busy */

    out8(पत्तनम् + 1, 0x00); /* disable generating interrupts */
    out8(पत्तनम् + 3, 0x80); /* line control register: command: set divisor */
    out8(पत्तनम्,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(पत्तनम् + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(पत्तनम् + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(पत्तनम् + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(पत्तनम्);     /* clear receiver port */
    in8(पत्तनम् + 5); /* clear line status port */
    in8(पत्तनम् + 6); /* clear modem status port */
}
#यदि_अन्तः /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#यदि_परिभाषितम् CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
शून्य uart_drv_putchar(
    अचिह्नित वर्ण c)
{
    यावत् (x86_KSdebug_पत्तनम् && (in8(x86_KSdebug_पत्तनम् + 5) & 0x20) == 0);
    out8(x86_KSdebug_पत्तनम्, c);
}

शून्य kernel_putDebugChar(अचिह्नित वर्ण c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण kernel_getDebugChar(शून्य)
{
    यावत् ((in8(x86_KSdebug_पत्तनम् + 5) & 1) == 0);
    प्रत्यागम in8(x86_KSdebug_पत्तनम्);
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
