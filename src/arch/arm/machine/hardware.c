/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    ଫେରନ୍ତୁ getRegister(thread, FaultIP);
}

ଶୂନ୍ୟ setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE ଶୂନ୍ୟ map_kernel_devices(ଶୂନ୍ୟ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ_KERNEL_DEVICE_FRAMES; i++) {
        ଅଚଳ kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        ପ୍ରତିଜ୍ଞା(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        ଯଦି (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

