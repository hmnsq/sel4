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
تهی VISIBLE NORETURN restore_user_context(تهی)
{
    word_t cur_thread_reg = (word_t) گره_STATE(ksCurThread);

    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(گره_STATE(ksCurThread));
#endif

    گره_UNLOCK_IF_HELD;

    اگر (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm فرار(
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
    } وگرنه {
        asm فرار("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + NextIP * اندازه(word_t))
                    );
    }
    UNREACHABLE();
}
