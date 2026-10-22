/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <plat/machine/hardware.h>

మాట_t శుద్ధ getRestartPC(tcb_t *thread)
{
    తిరిగి get_రిజిస్టరు(thread, FaultIP);
}

శూన్య సమితి_తదుపరి_PC(tcb_t *thread, మాట_t v)
{
    సమితి_రిజిస్టరు(thread, తదుపరి_PC_REG, v);
}

BOOT_సంకేతం శూన్య map_kernel_devices(శూన్య)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    కొరకు (పూర్ణాంకం i = 0; i < అంకె_KERNEL_DEVICE_చట్రాలు; i++) {
        స్థిరరాశి kernel_చట్రం_t *చట్రం = &kernel_device_చట్రాలు[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        ప్రతిజ్ఞా(చట్రం->armExecuteNever);
        map_kernel_చట్రం(చట్రం->paddr, చట్రం->pptr, VMKernelOnly,
                         vm_లక్షణాలు_new(చట్రం->armExecuteNever, false,
                                           false));
        ఒకవేళ (!చట్రం->user_అందుబాటు) {
            reserve_region((p_region_t) {
                .ప్రారంభం = చట్రం->paddr,
                .చివర   = చట్రం->paddr + బిట్టు(పుట_బిట్లు)
            });
        }
    }
}

