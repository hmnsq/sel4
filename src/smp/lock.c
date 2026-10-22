/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, ಗಾತ್ರ(big_kernel_ಬೀಗ) % EXCL_RES_GRANULE_ಗಾತ್ರ == 0);

clh_ಬೀಗ_t big_kernel_ಬೀಗ;

BOOT_ಸಂಕೇತ ಶೂನ್ಯ clh_ಬೀಗ_init(ಶೂನ್ಯ)
{
    /* Check if linker honoured alignment */
    ಪ್ರತಿಜ್ಞಾ(((se_L4_ಶಬ್ದ)&big_kernel_ಬೀಗ) % EXCL_RES_GRANULE_ಗಾತ್ರ == 0);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_MAX_ಅಂಕೆ_NODES; i++) {
        big_kernel_ಬೀಗ.ಗಂಟು[i].myreq = &big_kernel_ಬೀಗ.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_ಬೀಗ.request[CONFIG_MAX_ಅಂಕೆ_NODES].state = CLHState_Granted;
    big_kernel_ಬೀಗ.ಬಾಲ = &big_kernel_ಬೀಗ.request[CONFIG_MAX_ಅಂಕೆ_NODES];
}

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
