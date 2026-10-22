/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <plat/machine/hardware.h>

word_t विशुद्धम् getRestartPC(tcb_t *thread)
{
    प्रत्यागम get_पञ्जिकाकोशः(thread, FaultIP);
}

शून्य समुच्चयः_अग्र_PC(tcb_t *thread, word_t v)
{
    समुच्चयः_पञ्जिकाकोशः(thread, अग्र_PC_REG, v);
}

BOOT_संहिता शून्य map_kernel_devices(शून्य)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    प्रति (पूर्णाङ्क i = 0; i < अङ्कः_KERNEL_DEVICE_चौकटाः; i++) {
        ध्रुव kernel_frame_t *frame = &kernel_device_चौकटाः[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        प्रतिज्ञा(frame->armExecuteNever);
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly,
                         vm_गुणाः_new(frame->armExecuteNever, false,
                                           false));
        यदि (!frame->user_उपलब्धम्) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .अन्तः_2   = frame->paddr + BIT(PAGE_द्वयङ्काः)
            });
        }
    }
}

