/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, পরিমাণ(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE শূন্য clh_lock_init(শূন্য)
{
    /* Check if linker honoured alignment */
    প্রতিজ্ঞা(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    বাবে (পূর্ণাঙ্ক i = 0; i < CONFIG_MAX_অঙ্ক_NODES; i++) {
        big_kernel_lock.গ্ৰন্থি[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_অঙ্ক_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_অঙ্ক_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
