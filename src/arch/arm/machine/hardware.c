/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    પાછા getRegister(thread, FaultIP);
}

શૂન્ય setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE શૂન્ય map_kernel_devices(શૂન્ય)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    માટે (પૂર્ણાંક i = 0; i < અંક_KERNEL_DEVICE_FRAMES; i++) {
        અચલ kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        પ્રતિજ્ઞા(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        જો (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

