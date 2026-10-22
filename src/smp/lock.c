/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <smp/lock.h>

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, حجم(big_kernel_جندرا) % EXCL_RES_GRANULE_ناپ == 0);

clh_جندرا_t big_kernel_جندرا;

BOOT_کوڈ فراغ clh_جندرا_init(فراغ)
{
    /* Check if linker honoured alignment */
    أكد(((se_L4_لفظ)&big_kernel_جندرا) % EXCL_RES_GRANULE_ناپ == 0);
    لكل (صحيح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        big_kernel_جندرا.گرہ[i].myreq = &big_kernel_جندرا.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_جندرا.request[CONFIG_MAX_عدد_NODES].state = CLHState_Granted;
    big_kernel_جندرا.پوچھ = &big_kernel_جندرا.request[CONFIG_MAX_عدد_NODES];
}

#جے_انت /* ENABLE_SMP_SUPPORT */
