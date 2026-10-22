/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, ଆକାର(big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);

clh_lock_t big_kernel_lock;

BOOT_CODE ଶୂନ୍ୟ clh_lock_init(ଶୂନ୍ୟ)
{
    /* Check if linker honoured alignment */
    ପ୍ରତିଜ୍ଞା(((seL4_Word)&big_kernel_lock) % EXCL_RES_GRANULE_SIZE == 0);
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < CONFIG_MAX_ଅଙ୍କ_NODES; i++) {
        big_kernel_lock.ଗଣ୍ଠି[i].myreq = &big_kernel_lock.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_lock.request[CONFIG_MAX_ଅଙ୍କ_NODES].state = CLHState_Granted;
    big_kernel_lock.tail = &big_kernel_lock.request[CONFIG_MAX_ଅଙ୍କ_NODES];
}

#endif /* ENABLE_SMP_SUPPORT */
