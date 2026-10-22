/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, કદ(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE શૂન્ય clh_lock_init(શૂન્ય)
{
    /* Check if linker honoured alignment */
    પ્રતિજ્ઞા(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    માટે (પૂર્ણાંક i = 0; i < CONFIG_MAX_અંક_NODES; i++) {
        big_kernel_lock.ગાંઠ[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_અંક_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_અંક_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
