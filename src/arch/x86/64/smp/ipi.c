/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <mode/kernel/tlb.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

ಶೂನ್ಯ ಬಹುಲಕ_handle_Remote_Call(Ipi_ಬಹುಲಕ_Remote_Call_t call, ಶಬ್ದ_t arg0, ಶಬ್ದ_t arg1, ಶಬ್ದ_t arg2)
{
    ಆಯ್ಕೆ (call) {
    ಸ್ಥಿತಿ IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (ಶೂನ್ಯ *)arg1, arg2);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_ಬೇರು_t *)arg0, arg1);
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid remote call");
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
