/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <mode/smp/ipi.h>
#અન્તર્ભાવ <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

શૂન્ય Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    પસંદગી (call) {
    સ્થિતિ IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (શૂન્ય *)arg1, arg2);
        તોડો;

    સ્થિતિ IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_મૂળ_t *)arg0, arg1);
        તોડો;

    મૂળભૂત:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
