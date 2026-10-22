/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <object.h>
#ضمن <machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/kernel/thread.h>
#ضمن <linker.h>

فراغ Arch_switch_منزل_Thread(tcb_t *tcb)
{
    /* set PD */
    سيٽ_VMRoot(tcb);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    asm متطاير("movq %[value], %%gs:%c[offset]"
                 :
                 : [قدر] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [وٿي] "i"(OFFSETOF(ڳنڍ_Info_t, currentThreadUserContext)));
#جيڪڏهن_پورو
    إذا (config_سيٽ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    إذا (config_سيٽ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_ڪوڊ فراغ Arch_configureIdleThread(tcb_t *tcb)
{
    سيٽ_رجسٽر(tcb, FLAGS, FLAGS_USER_DEFAULT);
    سيٽ_رجسٽر(tcb, ايندڙ_IP, (لفظ_t)&idle_thread);
    سيٽ_رجسٽر(tcb, CS, SEL_CS_0);
    سيٽ_رجسٽر(tcb, SS, SEL_DS_0);
    /* We set the RSP to 0, even though the idle thread will never touch it, as it
     * allows us to distinguish an interrupt from the idle thread from an interrupt
     * from kernel execution, just by examining the saved RSP value (since the kernel
     * thread will have a valid RSP, and never 0). See traps.S for the other side of this
     */
    سيٽ_رجسٽر(tcb, RSP, 0);
}

فراغ Arch_switch_منزل_Idle_Thread(فراغ)
{
    tcb_t *tcb = ڳنڍ_STATE(ksIdleThread);
    /* Force the idle thread to run on kernel page table */
    سيٽ_VMRoot(tcb);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    asm متطاير("movq %[value], %%gs:%c[offset]"
                 :
                 : [قدر] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [وٿي] "i"(OFFSETOF(ڳنڍ_Info_t, currentThreadUserContext)));
#جيڪڏهن_پورو
}

فراغ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

فراغ گهڻائي_post_Modify_Registers(tcb_t *tptr)
{
    /* Setting Error to 0 will force a return by the interrupt path, which
     * does a full restore. Unless we're the current thread, in which case
     * we still have to go back out via a sysret */
    إذا (tptr != ڳنڍ_STATE(ksCurThread)) {
        سيٽ_رجسٽر(tptr, Error, 0);
    }
}
