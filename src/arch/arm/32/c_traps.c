/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/statedata.h>
#ضمن <arch/fastpath/fastpath.h>
#ضمن <arch/kernel/traps.h>
#ضمن <arch/machine/debug.h>
#ضمن <arch/machine/debug_conf.h>
#ضمن <api/syscall.h>
#ضمن <linker.h>
#ضمن <machine/fpu.h>

#ضمن <benchmark/benchmark_track.h>
#ضمن <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
فراغ VISIBLE NORETURN restore_user_context(فراغ)
{
    کلمه_t cur_thread_reg = (کلمه_t) گره_STATE(ksCurThread);

    c_exit_hook();

#اگر_تعریف_شده ARM_CP14_SAVE_AND_RESTORE_NATIVE_نخها
    restore_user_debug_context(گره_STATE(ksCurThread));
#پایان_اگر

    گره_UNLOCK_IF_HELD;

    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm متطاير(
            /* Set stack pointer to point at the r0 of the user context. */
            "mov sp, %[cur_thread_reg] \n"
            /* Pop user registers */
            "pop {r0-r12}              \n"
            /* Restore the user stack pointer */
            "pop {lr}                  \n"
            "msr sp_usr, lr            \n"
            /* prepare the exception return lr */
            "ldr lr, [sp, #4]          \n"
            "msr elr_hyp, lr           \n"
            /* prepare the user status register */
            "ldr lr, [sp, #8]          \n"
            "msr spsr_fsxc, lr         \n"
            /* Finally, pop our LR */
            "pop {lr}                  \n"
            /* Return to user */
            "eret"
            : /* no output */
            : [cur_thread_reg] "r"(cur_thread_reg)
            : "memory"
        );
    } وإلا {
        asm متطاير("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + بعدی_IP * حجم(کلمه_t))
                    );
    }
    UNREACHABLE();
}
