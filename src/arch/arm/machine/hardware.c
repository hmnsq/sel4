/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <machine/registerset.h>
#ضمن <arch/machine.h>
#ضمن <plat/machine/hardware.h>

کلمه_t خالص getRestartPC(tcb_t *thread)
{
    أرجع get_ثبتوونکی(thread, FaultIP);
}

فراغ سېټ_بل_PC(tcb_t *thread, کلمه_t v)
{
    سېټ_ثبتوونکی(thread, بل_PC_REG, v);
}

BOOT_کوډ فراغ map_kernel_devices(فراغ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    لكل (صحيح i = 0; i < عدد_KERNEL_DEVICE_چوکاټونه; i++) {
        ثابت kernel_چوکاټ_t *چوکاټ = &kernel_device_چوکاټونه[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        أكد(چوکاټ->armExecuteNever);
        map_kernel_چوکاټ(چوکاټ->paddr, چوکاټ->pptr, VMKernelOnly,
                         vm_ځانګړتیاوې_new(چوکاټ->armExecuteNever, false,
                                           false));
        إذا (!چوکاټ->user_شته) {
            reserve_region((p_region_t) {
                .پیل = چوکاټ->paddr,
                .پای_ټکی   = چوکاټ->paddr + بټ(پاڼه_بټونه)
            });
        }
    }
}

