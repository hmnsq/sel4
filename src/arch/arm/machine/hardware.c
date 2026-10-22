/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <machine/registerset.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    প্রত্যাগম getRegister(thread, FaultIP);
}

শূন্য setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE শূন্য map_kernel_devices(শূন্য)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_KERNEL_DEVICE_FRAMES; i++) {
        ধ্রুব kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        প্রতিজ্ঞা(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        যদি (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

