/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>

ಶಬ್ದ_t ಶುದ್ಧ getRestartPC(tcb_t *thread)
{
    ಹಿಂತಿರುಗಿ get_ರಿಜಿಸ್ಟರ್(thread, FaultIP);
}

ಶೂನ್ಯ ಗಣ_ಮುಂದಿನ_PC(tcb_t *thread, ಶಬ್ದ_t v)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಮುಂದಿನ_PC_REG, v);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_devices(ಶೂನ್ಯ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_KERNEL_DEVICE_ಚೌಕಟ್ಟುಗಳು; i++) {
        ಸ್ಥಿರಾಂಕ kernel_ಚೌಕಟ್ಟು_t *ಚೌಕಟ್ಟು = &kernel_device_ಚೌಕಟ್ಟುಗಳು[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        ಪ್ರತಿಜ್ಞಾ(ಚೌಕಟ್ಟು->armExecuteNever);
        map_kernel_ಚೌಕಟ್ಟು(ಚೌಕಟ್ಟು->paddr, ಚೌಕಟ್ಟು->pptr, VMKernelOnly,
                         vm_ಗುಣಲಕ್ಷಣಗಳು_new(ಚೌಕಟ್ಟು->armExecuteNever, false,
                                           false));
        ಆದರೆ (!ಚೌಕಟ್ಟು->user_ಲಭ್ಯ) {
            reserve_region((p_region_t) {
                .ಆರಂಭ = ಚೌಕಟ್ಟು->paddr,
                .ಕೊನೆ   = ಚೌಕಟ್ಟು->paddr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)
            });
        }
    }
}

