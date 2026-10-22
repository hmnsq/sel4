/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <smp/lock.h>

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, حجم(big_kernel_قفل) % EXCL_RES_GRANULE_اندازه == 0);

clh_قفل_t big_kernel_قفل;

BOOT_کد فراغ clh_قفل_init(فراغ)
{
    /* Check if linker honoured alignment */
    أكد(((se_L4_کلمه)&big_kernel_قفل) % EXCL_RES_GRANULE_اندازه == 0);
    لكل (صحيح i = 0; i < CONFIG_MAX_رقم_NODES; i++) {
        big_kernel_قفل.گره[i].myreq = &big_kernel_قفل.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_قفل.request[CONFIG_MAX_رقم_NODES].state = CLHState_Granted;
    big_kernel_قفل.دم = &big_kernel_قفل.request[CONFIG_MAX_رقم_NODES];
}

#پایان_اگر /* ENABLE_SMP_SUPPORT */
