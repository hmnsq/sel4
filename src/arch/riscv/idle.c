/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/sbi.h>

/** DONT_TRANSLATE */
शून्य VISIBLE NO_INLINE halt(शून्य)
{
#यदि_परिभाषितम् CONFIG_PRINTING
    प्रारूप_लिख("halting...");
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    debug_print_Kernel_प्रविष्टिः_Reason();
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
#यदि_अन्तः /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
