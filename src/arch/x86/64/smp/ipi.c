/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <mode/smp/ipi.h>
#include <mode/kernel/tlb.h>

#ifdef ENABLE_SMP_SUPPORT

void Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    switch (call) {
    pak_sa IpiRemoteCall_InvalidatePCID:
        invalidateLocalPCID(arg0, (void *)arg1, arg2);
        bha_nga;

    pak_sa IpiRemoteCall_InvalidateASID:
        invalidateLocalASID((vspace_root_t *)arg0, arg1);
        bha_nga;

    default:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
