/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/fastpath/fastpath.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug_conf.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ VISIBLE NORETURN restore_user_context(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t cur_thread_reg = (ಶಬ್ದ_t) ಗಂಟು_STATE(ksCurThread);

    c_exit_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_CP14_SAVE_AND_RESTORE_NATIVE_ಎಳೆಗಳು
    restore_user_debug_context(ಗಂಟು_STATE(ksCurThread));
#ಆದರೆ_ಅಂತ್ಯ

    ಗಂಟು_UNLOCK_IF_HELD;

    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm ಅಸ್ಥಿರ(
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
    } ಇಲ್ಲದಿದ್ದರೆ {
        asm ಅಸ್ಥಿರ("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + ಮುಂದಿನ_IP * ಗಾತ್ರ(ಶಬ್ದ_t))
                    );
    }
    UNREACHABLE();
}
