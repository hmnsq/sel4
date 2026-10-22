/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <model/statedata.h>
#অন্তর্ভাৱ <arch/fastpath/fastpath.h>
#অন্তর্ভাৱ <arch/kernel/traps.h>
#অন্তর্ভাৱ <arch/machine/debug.h>
#অন্তর্ভাৱ <arch/machine/debug_conf.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <machine/fpu.h>

#অন্তর্ভাৱ <benchmark/benchmark_track.h>
#অন্তর্ভাৱ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
শূন্য VISIBLE NORETURN restore_user_context(শূন্য)
{
    word_t cur_thread_reg = (word_t) গ্ৰন্থি_STATE(ksCurThread);

    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(গ্ৰন্থি_STATE(ksCurThread));
#endif

    গ্ৰন্থি_UNLOCK_IF_HELD;

    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm চঞ্চল(
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
    } অন্যথা {
        asm চঞ্চল("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + NextIP * পরিমাণ(word_t))
                    );
    }
    UNREACHABLE();
}
