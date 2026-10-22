/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <arch/machine/registerset.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/object/structures.h>

స్థిరరాశి రిజిస్టరు_t సందేశం_Registers[] = {
    R10, R8, R9, R15
};
compile_assert(
    consistent_సందేశం_registers,
    పరిమాణం(సందేశం_Registers) / పరిమాణం(సందేశం_Registers[0]) == n_సందేశం_Registers
);

స్థిరరాశి రిజిస్టరు_t చట్రం_Registers[] = {
    FaultIP, RSP, FLAGS, RAX, RBX, RCX, RDX, RSI, RDI, RBP,
    R8, R9, R10, R11, R12, R13, R14, R15
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
    context->registers[RAX] = 0;
    context->registers[RBX] = 0;
    context->registers[RCX] = 0;
    context->registers[RDX] = 0;
    context->registers[RSI] = 0;
    context->registers[RDI] = 0;
    context->registers[RBP] = 0;
    context->registers[R8]  = 0;
    context->registers[R9]  = 0;
    context->registers[R10] = 0;
    context->registers[R11] = 0;
    context->registers[R12] = 0;
    context->registers[R13] = 0;
    context->registers[R14] = 0;
    context->registers[R15] = 0;
    context->registers[RSP] = 0;
}

మాట_t బహుళకం_sanitise_రిజిస్టరు(రిజిస్టరు_t reg, మాట_t v)
{
    ఒకవేళ (reg == FaultIP || reg == తదుపరి_IP || reg == FS_పునాది || reg == GS_పునాది) {
        /* ensure instruction address is canonical */
        ఒకవేళ (v > 0x00007fffffffffff && v < 0xffff800000000000) {
            /* no way to guess what the user wanted so give them zero */
            v = 0;
        }
    }
    తిరిగి v;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
మాట_t getNBSendRecvDest(శూన్య)
{
    తిరిగి get_రిజిస్టరు(కణుపు_STATE(ksCurThread), nbsendRecvDest);
}
#అయితే_ముగింపు
