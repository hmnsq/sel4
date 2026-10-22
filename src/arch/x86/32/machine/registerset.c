/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <config.h>
#అన్తర్భావ <sel4/arch/constants.h>
#అన్తర్భావ <arch/machine/registerset.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/object/structures.h>
#అన్తర్భావ <machine/debug.h>

స్థిరరాశి రిజిస్టరు_t సందేశం_Registers[] = {
    EDI,
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    EBP
#అయితే_ముగింపు
};
compile_assert(
    consistent_సందేశం_registers,
    పరిమాణం(సందేశం_Registers) / పరిమాణం(సందేశం_Registers[0]) == n_సందేశం_Registers
);

స్థిరరాశి రిజిస్టరు_t చట్రం_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_చట్రం_registers,
    పరిమాణం(చట్రం_Registers) / పరిమాణం(చట్రం_Registers[0]) == n_చట్రం_Registers
);

స్థిరరాశి రిజిస్టరు_t gpRegisters[] = {
    FS_పునాది, GS_పునాది
};
compile_assert(
    consistent_gp_registers,
    పరిమాణం(gpRegisters) / పరిమాణం(gpRegisters[0]) == n_gpRegisters
);

శూన్య బహుళకం_init_Context(user_context_t *context)
{
    context->registers[EAX] = 0;
    context->registers[EBX] = 0;
    context->registers[ECX] = 0;
    context->registers[EDX] = 0;
    context->registers[ESI] = 0;
    context->registers[EDI] = 0;
    context->registers[EBP] = 0;
    context->registers[ESP] = 0;
}

మాట_t బహుళకం_sanitise_రిజిస్టరు(రిజిస్టరు_t reg, మాట_t v)
{
    తిరిగి v;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
మాట_t getNBSendRecvDest(శూన్య)
{
    seL4_IPCBuffer *బఫరు = (seL4_IPCBuffer *) lookupIPCBuffer(false, కణుపు_STATE(ksCurThread));
    ఒకవేళ (బఫరు != ఖాళీ) {
        తిరిగి బఫరు->user_దత్తాంశం;
    } లేకపోతే {
        తిరిగి 0;
    }
}
#అయితే_ముగింపు
