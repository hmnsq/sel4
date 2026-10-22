/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <smp/lock.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
compile_assert(BKL_not_padded, పరిమాణం(big_kernel_తాళం) % EXCL_RES_GRANULE_కొలత == 0);

clh_తాళం_t big_kernel_తాళం;

BOOT_సంకేతం శూన్య clh_తాళం_init(శూన్య)
{
    /* Check if linker honoured alignment */
    ప్రతిజ్ఞా(((se_L4_మాట)&big_kernel_తాళం) % EXCL_RES_GRANULE_కొలత == 0);
    కొరకు (పూర్ణాంకం i = 0; i < CONFIG_MAX_అంకె_NODES; i++) {
        big_kernel_తాళం.కణుపు[i].myreq = &big_kernel_తాళం.request[i];
    }

    /* Initialize the CLH tail */
    big_kernel_తాళం.request[CONFIG_MAX_అంకె_NODES].state = CLHState_Granted;
    big_kernel_తాళం.తోక = &big_kernel_తాళం.request[CONFIG_MAX_అంకె_NODES];
}

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
