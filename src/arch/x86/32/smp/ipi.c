/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <mode/smp/ipi.h>

#ifdef ENABLE_SMP_SUPPORT

வெறுமை Mode_handleRemoteCall(IpiModeRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2)
{
    தேர்வு (call) {
    நிலை IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        நிறுத்து;

    நிலை IpiRemoteCall_InvalidatePageStructureCache:
        invalidateLocalPageStructureCache();
        நிறுத்து;

    நிலை IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        நிறுத்து;

    இயல்பு:
        fail("Invalid remote call");
    }
}

#endif /* ENABLE_SMP_SUPPORT */
