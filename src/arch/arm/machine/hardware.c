/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <machine/registerset.h>
#समावेश <arch/machine.h>
#समावेश <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    वापस getRegister(thread, FaultIP);
}

व्योम set_अग्र_PC(tcb_t *thread, word_t v)
{
    setRegister(thread, अग्र_PC_REG, v);
}

BOOT_CODE व्योम map_kernel_devices(व्योम)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    क्रम (पूर्णांक i = 0; i < अंक_KERNEL_DEVICE_FRAMES; i++) {
        स्थिर kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        निश्चित(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        अगर (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

