/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/smp/ipi.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT

శూన్య బహుళకం_handle_Remote_Call(Ipi_బహుళకం_Remote_Call_t call, మాట_t arg0, మాట_t arg1, మాట_t arg2)
{
    ఎంపిక (call) {
    స్థితి IpiRemoteCall_InvalidateTLBEntry:
        invalidateLocalTLBEntry(arg0);
        ఆపు;

    స్థితి Ipi_Remote_Call_Invalidate_పుట_Structure_Cache:
        invalidate_Local_పుట_Structure_Cache();
        ఆపు;

    స్థితి IpiRemoteCall_InvalidateTLB:
        invalidateLocalTLB();
        ఆపు;

    అప్రమేయ:
        fail("Invalid remote call");
    }
}

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
