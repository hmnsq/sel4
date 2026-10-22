/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot_sys.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/io.h>
#ಅನ್ತರ್ಭಾವ <drivers/uart.h>

#ಆದರೆ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ಶೂನ್ಯ serial_init(uint16_t ಪೋರ್ಟ್)
{
    ಆಗ (!(in8(ಪೋರ್ಟ್ + 5) & 0x60)); /* wait until not busy */

    out8(ಪೋರ್ಟ್ + 1, 0x00); /* disable generating interrupts */
    out8(ಪೋರ್ಟ್ + 3, 0x80); /* line control register: command: set divisor */
    out8(ಪೋರ್ಟ್,     0x01); /* set low byte of divisor to 0x01 = 115200 baud */
    out8(ಪೋರ್ಟ್ + 1, 0x00); /* set high byte of divisor to 0x00 */
    out8(ಪೋರ್ಟ್ + 3, 0x03); /* line control register: set 8 bit, no parity, 1 stop bit */
    out8(ಪೋರ್ಟ್ + 4, 0x0b); /* modem control register: set DTR/RTS/OUT2 */

    in8(ಪೋರ್ಟ್);     /* clear receiver port */
    in8(ಪೋರ್ಟ್ + 5); /* clear line status port */
    in8(ಪೋರ್ಟ್ + 6); /* clear modem status port */
}
#ಆದರೆ_ಅಂತ್ಯ /* defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING) */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

/* there is no UART driver at drivers/serial/... for the pc99 platform, but we
 * implement parts of the API here, so we can reuse the generic code.
 */
ಶೂನ್ಯ uart_drv_putchar(
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ (x86_KSdebug_ಪೋರ್ಟ್ && (in8(x86_KSdebug_ಪೋರ್ಟ್ + 5) & 0x20) == 0);
    out8(x86_KSdebug_ಪೋರ್ಟ್, c);
}

ಶೂನ್ಯ kernel_putDebugChar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    /* this will take care of CR/LF handling and call uart_drv_putchar() */
    uart_console_putchar(c);
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ kernel_getDebugChar(ಶೂನ್ಯ)
{
    ಆಗ ((in8(x86_KSdebug_ಪೋರ್ಟ್ + 5) & 1) == 0);
    ಹಿಂತಿರುಗಿ in8(x86_KSdebug_ಪೋರ್ಟ್);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
