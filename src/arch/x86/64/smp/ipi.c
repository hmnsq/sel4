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
    निवड (call) {
    स्थिती IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (शून्य *)arg1, arg2);
        थांबा;

    स्थिती IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_मूळ_t *)arg0, arg1);
        थांबा;

    पूर्वनिर्धारित:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
