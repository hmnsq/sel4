/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    திரும்பு getRegister(thread, FaultIP);
}

வெறுமை setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE வெறுமை map_kernel_devices(வெறுமை)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    க்கு (முழு i = 0; i < இலக்கம்_KERNEL_DEVICE_FRAMES; i++) {
        மாறிலி kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        ப்ரதிஜ்ஞா(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        எனில் (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

