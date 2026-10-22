/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, परिमाण(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE शून्य clh_lock_init(शून्य)
{
    /* Check if linker honoured alignment */
    प्रतिज्ञा(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    प्रति (पूर्णाङ्क i = 0; i < CONFIG_MAX_अंक_NODES; i++) {
        big_kernel_lock.गाँठ[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_अंक_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_अंक_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
