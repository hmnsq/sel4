/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <smp/lock.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, परिमाण(big_kernel_तालकम्) % EXCL_RES_GRANULE_आकार == 0);

clh_तालकम्_t big_kernel_तालकम्;

BOOT_संहिता शून्य clh_तालकम्_init(शून्य)
{
    /* Check if linker honoured alignment */
    प्रतिज्ञा(((seL4_Word)&big_kernel_तालकम्) % EXCL_RES_GRANULE_आकार == 0);
    प्रति (पूर्णाङ्क i = 0; i < CONFIG_MAX_अङ्कः_NODES; i++) {
        big_kernel_तालकम्.ग्रन्थिः[i].myreq = &big_kernel_तालकम्.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_तालकम्.request[CONFIG_MAX_अङ्कः_NODES].state = CLHState_Granted;
    big_kernel_तालकम्.पुच्छ = &big_kernel_तालकम्.request[CONFIG_MAX_अङ्कः_NODES];
}

#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
