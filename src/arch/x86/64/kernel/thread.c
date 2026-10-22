/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Thread(tcb_t *tcb)
{
    /* set PD */
    ਸੈੱਟ_VMRoot(tcb);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    asm ਅਸਥਿਰ("movq %[value], %%gs:%c[offset]"
                 :
                 : [ਮੁੱਲ] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [ਵਿੱਥ] "i"(OFFSETOF(ਗੰਢ_Info_t, currentThreadUserContext)));
#ਜੇ_ਅੰਤ
    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_ਕੋਡ ਖਾਲੀ Arch_configureIdleThread(tcb_t *tcb)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, FLAGS, FLAGS_USER_DEFAULT);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, ਅਗਲਾ_IP, (ਸ਼ਬਦ_t)&idle_thread);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, CS, SEL_CS_0);
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, SS, SEL_DS_0);
    /* We set the RSP to 0, even though the idle thread will never touch it, as it
     * allows us to distinguish an interrupt from the idle thread from an interrupt
     * from kernel execution, just by examining the saved RSP value (since the kernel
     * thread will have a valid RSP, and never 0). See traps.S for the other side of this
     */
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, RSP, 0);
}

ਖਾਲੀ Arch_switch_ਮੰਜ਼ਿਲ_Idle_Thread(ਖਾਲੀ)
{
    tcb_t *tcb = ਗੰਢ_STATE(ksIdleThread);
    /* Force the idle thread to run on kernel page table */
    ਸੈੱਟ_VMRoot(tcb);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    asm ਅਸਥਿਰ("movq %[value], %%gs:%c[offset]"
                 :
                 : [ਮੁੱਲ] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [ਵਿੱਥ] "i"(OFFSETOF(ਗੰਢ_Info_t, currentThreadUserContext)));
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

ਖਾਲੀ ਬਹੁਲਕ_post_Modify_Registers(tcb_t *tptr)
{
    /* Setting Error to 0 will force a return by the interrupt path, which
     * does a full restore. Unless we're the current thread, in which case
     * we still have to go back out via a sysret */
    ਜੇ (tptr != ਗੰਢ_STATE(ksCurThread)) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(tptr, Error, 0);
    }
}
