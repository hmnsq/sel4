/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/fastpath/fastpath.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug_conf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ਖਾਲੀ VISIBLE NORETURN restore_user_context(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t cur_thread_reg = (ਸ਼ਬਦ_t) ਗੰਢ_STATE(ksCurThread);

    c_exit_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARM_CP14_SAVE_AND_RESTORE_NATIVE_ਧਾਗੇ
    restore_user_debug_context(ਗੰਢ_STATE(ksCurThread));
#ਜੇ_ਅੰਤ

    ਗੰਢ_UNLOCK_IF_HELD;

    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm ਅਸਥਿਰ(
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
    } ਨਹੀਂਤਾਂ {
        asm ਅਸਥਿਰ("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + ਅਗਲਾ_IP * ਆਕਾਰ(ਸ਼ਬਦ_t))
                    );
    }
    UNREACHABLE();
}
