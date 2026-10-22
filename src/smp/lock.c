/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/lock.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, ਆਕਾਰ(big_kernel_ਤਾਲਾ) % EXCL_RES_GRANULE_ਮਾਪ == 0);

clh_ਤਾਲਾ_t big_kernel_ਤਾਲਾ;

BOOT_ਕੋਡ ਖਾਲੀ clh_ਤਾਲਾ_init(ਖਾਲੀ)
{
    /* Check if linker honoured alignment */
    ਪ੍ਰਤਿਜ੍ਞਾ(((se_L4_ਸ਼ਬਦ)&big_kernel_ਤਾਲਾ) % EXCL_RES_GRANULE_ਮਾਪ == 0);
    ਲਈ (ਪੂਰਨ i = 0; i < CONFIG_MAX_ਅੰਕ_NODES; i++) {
        big_kernel_ਤਾਲਾ.ਗੰਢ[i].myreq = &big_kernel_ਤਾਲਾ.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_ਤਾਲਾ.request[CONFIG_MAX_ਅੰਕ_NODES].state = CLHState_Granted;
    big_kernel_ਤਾਲਾ.ਪੂਛ = &big_kernel_ਤਾਲਾ.request[CONFIG_MAX_ਅੰਕ_NODES];
}

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
