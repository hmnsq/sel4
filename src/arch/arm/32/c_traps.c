/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <model/statedata.h>
#شامل <arch/fastpath/fastpath.h>
#شامل <arch/kernel/traps.h>
#شامل <arch/machine/debug.h>
#شامل <arch/machine/debug_conf.h>
#شامل <api/syscall.h>
#شامل <linker.h>
#شامل <machine/fpu.h>

#شامل <benchmark/benchmark_track.h>
#شامل <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
خالی VISIBLE NORETURN restore_user_context(خالی)
{
    لفظ_t cur_thread_reg = (لفظ_t) گرہ_STATE(ksCurThread);

    c_exit_hook();

#اگر_تعریف_شدہ ARM_CP14_SAVE_AND_RESTORE_NATIVE_دھاگے
    restore_user_debug_context(گرہ_STATE(ksCurThread));
#اگر_ختم

    گرہ_UNLOCK_IF_HELD;

    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm متلون(
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
    } ورنہ {
        asm متلون("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + اگلا_IP * حجم(لفظ_t))
                    );
    }
    UNREACHABLE();
}
