/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

शून्य Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    निवड (call) {
    स्थिती IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        थांबा;

    स्थिती IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        थांबा;

    स्थिती IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        थांबा;

    पूर्वनिर्धारित:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
