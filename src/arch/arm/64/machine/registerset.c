/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <arch/machine/registerset.h>

స్థిరరాశి రిజిస్టరు_t సందేశం_Registers[] = {
    X2, X3, X4, X5
};
compile_assert(
    consistent_సందేశం_registers,
    పరిమాణం(సందేశం_Registers) / పరిమాణం(సందేశం_Registers[0]) == n_సందేశం_Registers
);

స్థిరరాశి రిజిస్టరు_t చట్రం_Registers[] = {
    FaultIP, SP_EL0, SPSR_EL1,
    X0, X1, X2, X3, X4, X5, X6, X7, X8, X16, X17, X18, X29, X30
};
compile_assert(
    consistent_చట్రం_registers,
    పరిమాణం(చట్రం_Registers) / పరిమాణం(చట్రం_Registers[0]) == n_చట్రం_Registers
);

స్థిరరాశి రిజిస్టరు_t gpRegisters[] = {
    X9, X10, X11, X12, X13, X14, X15,
    X19, X20, X21, X22, X23, X24, X25, X26, X27, X28,
    TPIDR_EL0, TPIDRRO_EL0,
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
