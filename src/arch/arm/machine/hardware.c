/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <machine/registerset.h>
#ضمن <arch/machine.h>
#ضمن <plat/machine/hardware.h>

لفظ_t خالص getRestartPC(tcb_t *thread)
{
    أرجع get_رجسٽر(thread, FaultIP);
}

فراغ سيٽ_ايندڙ_PC(tcb_t *thread, لفظ_t v)
{
    سيٽ_رجسٽر(thread, ايندڙ_PC_REG, v);
}

BOOT_ڪوڊ فراغ map_kernel_devices(فراغ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    لكل (صحيح i = 0; i < انگ_KERNEL_DEVICE_چوکٺا; i++) {
        ثابت kernel_چوکٺو_t *چوکٺو = &kernel_device_چوکٺا[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        أكد(چوکٺو->armExecuteNever);
        map_kernel_چوکٺو(چوکٺو->paddr, چوکٺو->pptr, VMKernelOnly,
                         vm_خاصيتون_new(چوکٺو->armExecuteNever, false,
                                           false));
        إذا (!چوکٺو->user_موجود) {
            reserve_region((p_region_t) {
                .شروعات = چوکٺو->paddr,
                .پڇاڙي   = چوکٺو->paddr + ٻٽو_انگ(صفحو_بٽ)
            });
        }
    }
}

