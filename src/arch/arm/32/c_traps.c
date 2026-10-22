/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/fastpath/fastpath.h>
#अन्तर्भाव <arch/kernel/traps.h>
#अन्तर्भाव <arch/machine/debug.h>
#अन्तर्भाव <arch/machine/debug_conf.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <machine/fpu.h>

#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
शून्य VISIBLE NORETURN restore_user_context(शून्य)
{
    word_t cur_thread_reg = (word_t) ग्रन्थिः_STATE(ksCurThread);

    c_exit_hook();

#यदि_परिभाषितम् ARM_CP14_SAVE_AND_RESTORE_NATIVE_तन्तवः
    restore_user_debug_context(ग्रन्थिः_STATE(ksCurThread));
#यदि_अन्तः

    ग्रन्थिः_UNLOCK_IF_HELD;

    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm चञ्चल(
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
        asm चञ्चल("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + अग्र_IP * परिमाण(word_t))
                    );
    }
    UNREACHABLE();
}
