/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <arch/machine/registerset.h>

స్థిరరాశి రిజిస్టరు_t సందేశం_Registers[] = {
    a2, a3, a4, a5
};
compile_assert(
    consistent_సందేశం_registers,
    పరిమాణం(సందేశం_Registers) / పరిమాణం(సందేశం_Registers[0]) == n_సందేశం_Registers
);

స్థిరరాశి రిజిస్టరు_t చట్రం_Registers[] = {
    FaultIP, ra, sp, gp,
    s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
};
compile_assert(
    consistent_చట్రం_registers,
    పరిమాణం(చట్రం_Registers) / పరిమాణం(చట్రం_Registers[0]) == n_చట్రం_Registers
);

స్థిరరాశి రిజిస్టరు_t gpRegisters[] = {
    a0, a1, a2, a3, a4, a5, a6, a7,
    t0, t1, t2, t3, t4, t5, t6,
    tp,
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
