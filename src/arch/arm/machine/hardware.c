/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    फर्काऊ getRegister(thread, FaultIP);
}

शून्य setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE शून्य map_kernel_devices(शून्य)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    लागि (पूर्णाङ्क i = 0; i < अङ्क_KERNEL_DEVICE_FRAMES; i++) {
        अचर kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        प्रतिज्ञा(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        यदि (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

