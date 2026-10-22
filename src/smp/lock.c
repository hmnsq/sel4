/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <assert.h>
#شامل <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, اندازه(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE تهی clh_lock_init(تهی)
{
    /* Check if linker honoured alignment */
    أكد(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    برای (صحیح i = 0; i < CONFIG_MAX_رقم_NODES; i++) {
        big_kernel_lock.گره[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_رقم_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_رقم_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
