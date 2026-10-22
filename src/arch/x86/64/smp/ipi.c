/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/smp/ipi.h>
#अन्तर्भाव <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

शून्य Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    विकल्प (call) {
    पक्ष IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (शून्य *)arg1, arg2);
        भङ्ग;

    पक्ष IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_मूलम्_t *)arg0, arg1);
        भङ्ग;

    स्वभाव:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
