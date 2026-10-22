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
    أرجع get_رجسٹر(thread, FaultIP);
}

فراغ سیٹ_اگلا_PC(tcb_t *thread, لفظ_t v)
{
    سیٹ_رجسٹر(thread, اگلا_PC_REG, v);
}

BOOT_کوڈ فراغ map_kernel_devices(فراغ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    لكل (صحيح i = 0; i < عدد_KERNEL_DEVICE_چوکھٹے; i++) {
        ثابت kernel_چوکھٹا_t *چوکھٹا = &kernel_device_چوکھٹے[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        أكد(چوکھٹا->armExecuteNever);
        map_kernel_چوکھٹا(چوکھٹا->paddr, چوکھٹا->pptr, VMKernelOnly,
                         vm_گن_new(چوکھٹا->armExecuteNever, false,
                                           false));
        إذا (!چوکھٹا->user_دستیاب) {
            reserve_region((p_region_t) {
                .شروع = چوکھٹا->paddr,
                .سرا   = چوکھٹا->paddr + ثنائی_ہندسہ(ورقہ_بٹ)
            });
        }
    }
}

