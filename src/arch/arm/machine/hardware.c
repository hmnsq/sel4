/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>

ਸ਼ਬਦ_t ਸ਼ੁੱਧ getRestartPC(tcb_t *thread)
{
    ਵਾਪਸ get_ਰਜਿਸਟਰ(thread, FaultIP);
}

ਖਾਲੀ ਸੈੱਟ_ਅਗਲਾ_PC(tcb_t *thread, ਸ਼ਬਦ_t v)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਅਗਲਾ_PC_REG, v);
}

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_devices(ਖਾਲੀ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_KERNEL_DEVICE_ਚੌਖਟੇ; i++) {
        ਅਚਲ kernel_ਚੌਖਟਾ_t *ਚੌਖਟਾ = &kernel_device_ਚੌਖਟੇ[i];
        /* all frames are supposed to describe device memory, so they should
         * never be marked as executable.
         */
        ਪ੍ਰਤਿਜ੍ਞਾ(ਚੌਖਟਾ->armExecuteNever);
        map_kernel_ਚੌਖਟਾ(ਚੌਖਟਾ->paddr, ਚੌਖਟਾ->pptr, VMKernelOnly,
                         vm_ਗੁਣ_new(ਚੌਖਟਾ->armExecuteNever, false,
                                           false));
        ਜੇ (!ਚੌਖਟਾ->user_ਉਪਲਬਧ) {
            reserve_region((p_region_t) {
                .ਸ਼ੁਰੂ = ਚੌਖਟਾ->paddr,
                .ਸਿਰਾ   = ਚੌਖਟਾ->paddr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)
            });
        }
    }
}

