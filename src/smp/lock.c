/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <smp/lock.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, حجم(big_kernel_تالو) % EXCL_RES_GRANULE_سائيز == 0);

clh_تالو_t big_kernel_تالو;

BOOT_ڪوڊ فراغ clh_تالو_init(فراغ)
{
    /* Check if linker honoured alignment */
    أكد(((se_L4_لفظ)&big_kernel_تالو) % EXCL_RES_GRANULE_سائيز == 0);
    لكل (صحيح i = 0; i < CONFIG_MAX_انگ_NODES; i++) {
        big_kernel_تالو.ڳنڍ[i].myreq = &big_kernel_تالو.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_تالو.request[CONFIG_MAX_انگ_NODES].state = CLHState_Granted;
    big_kernel_تالو.پڇ = &big_kernel_تالو.request[CONFIG_MAX_انگ_NODES];
}

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
