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

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/devices_gen.h>

#घोषणा STAT 0x14
#घोषणा TRANSMIT 0x1c

#घोषणा STAT_TDRE (1 << 23)

#घोषणा UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#अगर defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    वापस 0;
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
