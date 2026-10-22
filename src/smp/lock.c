/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <assert.h>
#شامل <smp/lock.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, حجم(big_kernel_تالا) % EXCL_RES_GRANULE_سائز == 0);

clh_تالا_t big_kernel_تالا;

BOOT_رمز خالی clh_تالا_init(خالی)
{
    /* Check if linker honoured alignment */
    أكد(((se_L4_لفظ)&big_kernel_تالا) % EXCL_RES_GRANULE_سائز == 0);
    برائے (صحیح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        big_kernel_تالا.گرہ[i].myreq = &big_kernel_تالا.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_تالا.request[CONFIG_MAX_عدد_NODES].state = CLHState_Granted;
    big_kernel_تالا.دم = &big_kernel_تالا.request[CONFIG_MAX_عدد_NODES];
}

#اگر_ختم /* ENABLE_SMP_SUPPORT */
