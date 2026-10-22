/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <object.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <linker.h>

शून्य Arch_switch_गन्तव्यम्_Thread(tcb_t *tcb)
{
    /* set PD */
    समुच्चयः_VMRoot(tcb);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    asm चञ्चल("movq %[value], %%gs:%c[offset]"
                 :
                 : [मानम्] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [विस्थापनम्] "i"(OFFSETOF(ग्रन्थिः_Info_t, currentThreadUserContext)));
#यदि_अन्तः
    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_IBPB_ON_CONTEXT_SWITCH)) {
        x86_ibpb();
    }

    यदि (config_समुच्चयः(CONFIG_KERNEL_X86_RSB_ON_CONTEXT_SWITCH)) {
        x86_flush_rsb();
    }
}

BOOT_संहिता शून्य Arch_configureIdleThread(tcb_t *tcb)
{
    समुच्चयः_पञ्जिकाकोशः(tcb, FLAGS, FLAGS_USER_DEFAULT);
    समुच्चयः_पञ्जिकाकोशः(tcb, अग्र_IP, (word_t)&idle_thread);
    समुच्चयः_पञ्जिकाकोशः(tcb, CS, SEL_CS_0);
    समुच्चयः_पञ्जिकाकोशः(tcb, SS, SEL_DS_0);
    /* We set the RSP to 0, even though the idle thread will never touch it, as it
     * allows us to distinguish an interrupt from the idle thread from an interrupt
     * from kernel execution, just by examining the saved RSP value (since the kernel
     * thread will have a valid RSP, and never 0). See traps.S for the other side of this
     */
    समुच्चयः_पञ्जिकाकोशः(tcb, RSP, 0);
}

शून्य Arch_switch_गन्तव्यम्_Idle_Thread(शून्य)
{
    tcb_t *tcb = ग्रन्थिः_STATE(ksIdleThread);
    /* Force the idle thread to run on kernel page table */
    समुच्चयः_VMRoot(tcb);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    asm चञ्चल("movq %[value], %%gs:%c[offset]"
                 :
                 : [मानम्] "r"(&tcb->tcbArch.tcbContext.registers[Error + 1]),
                 [विस्थापनम्] "i"(OFFSETOF(ग्रन्थिः_Info_t, currentThreadUserContext)));
#यदि_अन्तः
}

शून्य Arch_activateIdleThread(tcb_t *tcb)
{
    /* Don't need to do anything */
}

शून्य बहुलकम्_post_Modify_Registers(tcb_t *tptr)
{
    /* Setting Error to 0 will force a return by the interrupt path, which
     * does a full restore. Unless we're the current thread, in which case
     * we still have to go back out via a sysret */
    यदि (tptr != ग्रन्थिः_STATE(ksCurThread)) {
        समुच्चयः_पञ्जिकाकोशः(tptr, Error, 0);
    }
}
