/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, ಪರಿಮಾಣ(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE ಶೂನ್ಯ clh_lock_init(ಶೂನ್ಯ)
{
    /* Check if linker honoured alignment */
    ಪ್ರತಿಜ್ಞಾ(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < CONFIG_MAX_ಅಂಕೆ_NODES; i++) {
        big_kernel_lock.ಗಂಟು[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_ಅಂಕೆ_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_ಅಂಕೆ_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
