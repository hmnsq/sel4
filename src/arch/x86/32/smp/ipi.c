/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

શૂન્ય Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    પસંદગી (call) {
    સ્થિતિ IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        તોડો;

    સ્થિતિ IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        તોડો;

    સ્થિતિ IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        તોડો;

    મૂળભૂત:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
