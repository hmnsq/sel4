/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <mode/smp/ipi.h>
#समावेश <mode/kernel/tlb.h>

#अगर_घोषित ENABLE_SMP_SUPPORT

व्योम Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    चयन (call) {
    हाल IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (व्योम *)arg1, arg2);
        अवरोध;

    हाल IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_मूल_t *)arg0, arg1);
        अवरोध;

    शेष:
        fail("Invalid remote call");
    }
}

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
