/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/kernel/boot_sys.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/io.h>
#అన్తర్భావ <drivers/uart.h>

#ఒకవేళ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
శూన్య serial_init(uint16_t పోర్టు)
{
    అయితే (!(in8(పోర్టు + 5) & 0x60)); /* wait until not busy */

    out8(పోర్టు + 1, 0x00); /* disable generating interrupts */
    out8(పోర్టు + 3, 0x80); /* line control register: command: set divisor */
    out8(పోర్టు,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(పోర్టు + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(పోర్టు + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(పోర్టు + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(పోర్టు);     /* clear receiver port */
    in8(పోర్టు + 5); /* clear line status port */
    in8(పోర్టు + 6); /* clear modem status port */
}
#అయితే_ముగింపు /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#నిర్వచితమైతే CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
శూన్య uart_drv_putchar(
    గుర్తులేని అక్షరం c)
{
    అయితే (x86_KSdebug_పోర్టు && (in8(x86_KSdebug_పోర్టు + 5) & 0x20) == 0);
    out8(x86_KSdebug_పోర్టు, c);
}

శూన్య kernel_putDebugChar(గుర్తులేని అక్షరం c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం kernel_getDebugChar(శూన్య)
{
    అయితే ((in8(x86_KSdebug_పోర్టు + 5) & 1) == 0);
    తిరిగి in8(x86_KSdebug_పోర్టు);
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
