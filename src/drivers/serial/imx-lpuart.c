/*
 * Copyright 2021, Breakaway Consulting Pty. Ltd.
 *
 * A simple output only UART driver for the NXP i.MX Low Power UART.
 *
 * Technical Reference:
 *   i.MX 8DualX/8DualXPlus/8QuadXPlus Applications Processor Reference Manual
 *   Revision 0 (IMX8DQXPRM.pdf)
 *   Chapter 16.13 (page 7908)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
शून्य uart_drv_putchar(अचिन्हित वर्ण c)
{
    जबसम्म (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
अचिन्हित वर्ण uart_drv_getchar(शून्य)
{
    फर्काऊ 0;
}
#endif /* CONFIG_DEBUG_BUILD */
