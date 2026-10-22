/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/fastpath/fastpath.h>
#అన్తర్భావ <arch/kernel/traps.h>
#అన్తర్భావ <arch/machine/debug.h>
#అన్తర్భావ <arch/machine/debug_conf.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <machine/fpu.h>

#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
శూన్య VISIBLE NORETURN restore_user_context(శూన్య)
{
    మాట_t cur_thread_reg = (మాట_t) కణుపు_STATE(ksCurThread);

    c_exit_hook();

#నిర్వచితమైతే ARM_CP14_SAVE_AND_RESTORE_NATIVE_దారాలు
    restore_user_debug_context(కణుపు_STATE(ksCurThread));
#అయితే_ముగింపు

    కణుపు_UNLOCK_IF_HELD;

    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        asm అస్థిర(
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
    } లేకపోతే {
        asm అస్థిర("mov sp, %[cur_thread] \n\
                  ldmdb sp, {r0-lr}^ \n\
                  rfeia sp"
                     : /* no output */
                     : [cur_thread] "r"(cur_thread_reg + తదుపరి_IP * పరిమాణం(మాట_t))
                    );
    }
    UNREACHABLE();
}
