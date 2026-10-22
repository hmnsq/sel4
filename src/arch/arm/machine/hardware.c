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
    أرجع get_ثبات(thread, FaultIP);
}

فراغ مجموعه_بعدی_PC(tcb_t *thread, کلمه_t v)
{
    مجموعه_ثبات(thread, بعدی_PC_REG, v);
}

BOOT_کد فراغ map_kernel_devices(فراغ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    لكل (صحيح i = 0; i < رقم_KERNEL_DEVICE_قابها; i++) {
        ثابت kernel_قاب_t *قاب = &kernel_device_قابها[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        أكد(قاب->armExecuteNever);
        map_kernel_قاب(قاب->paddr, قاب->pptr, VMKernelOnly,
                         vm_صفات_new(قاب->armExecuteNever, false,
                                           false));
        إذا (!قاب->user_موجود) {
            reserve_region((p_region_t) {
                .شروع = قاب->paddr,
                .انتها   = قاب->paddr + بیت(صفحه_بیتها)
            });
        }
    }
}

