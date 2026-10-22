/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#परिभाषय UTHR        0x0
#परिभाषय ULSR        0x14

#परिभाषय ULSR_THRE   BIT(5)
#परिभाषय ULSR_RDR    BIT(0)

#परिभाषय UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् ((*UART_REG(ULSR) & ULSR_THRE) == 0);

    *UART_REG(UTHR) = c;
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् ((*UART_REG(ULSR) & ULSR_RDR) == 0);

    प्रत्यागम *UART_REG(UTHR);
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
