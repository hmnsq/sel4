/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/fastpath/fastpath.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/traps.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug_conf.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>

#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ଶୂନ୍ୟ VISIBLE NORETURN restore_user_context(ଶୂନ୍ୟ)
{
    word_t cur_thread_reg = (word_t) ଗଣ୍ଠି_STATE(ksCurThread);

    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(ଗଣ୍ଠି_STATE(ksCurThread));
#endif

    ଗଣ୍ଠି_UNLOCK_IF_HELD;

    ଯଦି (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm ଅସ୍ଥିର(
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
    } ଅନ୍ୟଥା {
        asm ଅସ୍ଥିର("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + NextIP * ଆକାର(word_t))
                    );
    }
    UNREACHABLE();
}
