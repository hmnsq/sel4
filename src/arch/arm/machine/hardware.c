/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <machine/registerset.h>
#ضمن <arch/machine.h>
#ضمن <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    أرجع getRegister(thread, FaultIP);
}

فراغ setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE فراغ map_kernel_devices(فراغ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    لكل (صحيح i = 0; i < رەقەم_KERNEL_DEVICE_FRAMES; i++) {
        ثابت kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        أكد(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        إذا (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

