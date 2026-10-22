/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>

word_t PURE getRestartPC(tcb_t *thread)
{
    തിരികെ getRegister(thread, FaultIP);
}

ശൂന്യം setNextPC(tcb_t *thread, word_t v)
{
    setRegister(thread, NEXT_PC_REG, v);
}

BOOT_CODE ശൂന്യം map_kernel_devices(ശൂന്യം)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_KERNEL_DEVICE_FRAMES; i++) {
        സ്ഥിരം kernel_frame_t *frame = &kernel_device_frames[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        പ്രതിജ്ഞാ(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_attributes_new(frame->armExecuteNever, false,
                                           false));
        എങ്കിൽ (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(PAGE_BITS)
            });
        }
    }
}

