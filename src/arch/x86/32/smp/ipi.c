/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

void Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    switch (call) {
    pak_sa IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        bha_nga;

    pak_sa IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        bha_nga;

    pak_sa IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        bha_nga;

    default:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
