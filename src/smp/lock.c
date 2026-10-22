/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, വലുപ്പം(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE ശൂന്യം clh_lock_init(ശൂന്യം)
{
    /* Check if linker honoured alignment */
    പ്രതിജ്ഞാ(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < CONFIG_MAX_അക്കം_NODES; i++) {
        big_kernel_lock.കണ്ണി[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_അക്കം_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_അക്കം_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
