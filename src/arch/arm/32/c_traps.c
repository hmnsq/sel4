/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <arch/fastpath/fastpath.h>
#അന്തര്ഭാവ <arch/kernel/traps.h>
#അന്തര്ഭാവ <arch/machine/debug.h>
#അന്തര്ഭാവ <arch/machine/debug_conf.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <machine/fpu.h>

#അന്തര്ഭാവ <benchmark/benchmark_track.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ശൂന്യം VISIBLE NORETURN restore_user_context(ശൂന്യം)
{
    word_t cur_thread_reg = (word_t) കണ്ണി_STATE(ksCurThread);

    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(കണ്ണി_STATE(ksCurThread));
#endif

    കണ്ണി_UNLOCK_IF_HELD;

    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm അസ്ഥിര(
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
    } അല്ലെങ്കിൽ {
        asm അസ്ഥിര("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + NextIP * വലുപ്പം(word_t))
                    );
    }
    UNREACHABLE();
}
