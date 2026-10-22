/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <smp/lock.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, حجم(big_kernel_قلف) % EXCL_RES_GRANULE_اندازه == 0);

clh_قلف_t big_kernel_قلف;

BOOT_کوډ فراغ clh_قلف_init(فراغ)
{
    /* Check if linker honoured alignment */
    أكد(((se_L4_کلمه)&big_kernel_قلف) % EXCL_RES_GRANULE_اندازه == 0);
    لكل (صحيح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        big_kernel_قلف.غوټه[i].myreq = &big_kernel_قلف.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_قلف.request[CONFIG_MAX_عدد_NODES].state = CLHState_Granted;
    big_kernel_قلف.لکۍ = &big_kernel_قلف.request[CONFIG_MAX_عدد_NODES];
}

#د_که_پای /* ENABLE_SMP_SUPPORT */
