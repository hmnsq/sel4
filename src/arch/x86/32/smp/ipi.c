/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

ശൂന്യം Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    തിരഞ്ഞെടുപ്പ് (call) {
    അവസ്ഥ IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        നിർത്തുക;

    അവസ്ഥ IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        നിർത്തുക;

    അവസ്ഥ IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
