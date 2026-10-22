/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

ಶೂನ್ಯ Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    ವಿಕಲ್ಪ (call) {
    ಪಕ್ಷ IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (ಶೂನ್ಯ *)arg1, arg2);
        ಭಙ್ಗ;

    ಪಕ್ಷ IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_ಬೇರು_t *)arg0, arg1);
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
