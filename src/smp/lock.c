/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <assert.h>
#समावेश <smp/lock.h>

#अगर_घोषित ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, माप(big_kernel_lock) % EXCL_RES_GRANULE_आकार == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE व्योम clh_lock_init(व्योम)
{
    /* Check if linker honoured alignment */
    निश्चित(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_आकार == 0);
    क्रम (पूर्णांक i = 0; i < CONFIG_MAX_अंक_NODES; i++) {
        big_kernel_lock.गाँठ[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_अंक_NODES].state = CLHState_Granted;
    big_kernel_lock.पुच्छ = &big_kernel_lock.request[CONFIG_MAX_अंक_NODES];
}

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
