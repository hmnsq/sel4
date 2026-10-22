/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/fastpath/fastpath.h>
#அந்தர்பாவ <arch/kernel/traps.h>
#அந்தர்பாவ <arch/machine/debug.h>
#அந்தர்பாவ <arch/machine/debug_conf.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <machine/fpu.h>

#அந்தர்பாவ <benchmark/benchmark_track.h>
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
வெறுமை VISIBLE NORETURN restore_user_context(வெறுமை)
{
    word_t cur_thread_reg = (word_t) முனை_STATE(ksCurThread);

    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(முனை_STATE(ksCurThread));
#endif

    முனை_UNLOCK_IF_HELD;

    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm நிலையற்ற(
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
    } இல்லையெனில் {
        asm நிலையற்ற("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + NextIP * அளவு(word_t))
                    );
    }
    UNREACHABLE();
}
