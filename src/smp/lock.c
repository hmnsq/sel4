/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, அளவு(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE வெறுமை clh_lock_init(வெறுமை)
{
    /* Check if linker honoured alignment */
    ப்ரதிஜ்ஞா(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    க்கு (முழு i = 0; i < CONFIG_MAX_இலக்கம்_NODES; i++) {
        big_kernel_lock.முனை[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_இலக்கம்_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_இலக்கம்_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
