/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <model/statedata.h>
#समावेश <arch/fastpath/fastpath.h>
#समावेश <arch/kernel/traps.h>
#समावेश <arch/machine/debug.h>
#समावेश <arch/machine/debug_conf.h>
#समावेश <api/syscall.h>
#समावेश <linker.h>
#समावेश <machine/fpu.h>

#समावेश <benchmark/benchmark_track.h>
#समावेश <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
व्योम VISIBLE NORETURN restore_user_context(व्योम)
{
    word_t cur_thread_reg = (word_t) गाँठ_STATE(ksCurThread);

    c_exit_hook();

#अगर_घोषित ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(गाँठ_STATE(ksCurThread));
#पूर्ण_अगर

    गाँठ_UNLOCK_IF_HELD;

    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        यंत्र अस्थिर(
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
    } अन्यथा {
        यंत्र अस्थिर("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + अग्र_IP * माप(word_t))
                    );
    }
    UNREACHABLE();
}
