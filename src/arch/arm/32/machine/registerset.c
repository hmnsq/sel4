/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <arch/machine/registerset.h>

స్థిరరాశి రిజిస్టరు_t సందేశం_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_సందేశం_registers,
    పరిమాణం(సందేశం_Registers) / పరిమాణం(సందేశం_Registers[0]) == n_సందేశం_Registers
);

స్థిరరాశి రిజిస్టరు_t చట్రం_Registers[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_చట్రం_registers,
    పరిమాణం(చట్రం_Registers) / పరిమాణం(చట్రం_Registers[0]) == n_చట్రం_Registers
);

స్థిరరాశి రిజిస్టరు_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    పరిమాణం(gpRegisters) / పరిమాణం(gpRegisters[0]) == n_gpRegisters
);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
మాట_t getNBSendRecvDest(శూన్య)
{
    తిరిగి get_రిజిస్టరు(కణుపు_STATE(ksCurThread), nbsendRecvDest);
}
#అయితే_ముగింపు
