/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <arch/machine/registerset.h>

ध्रुव पञ्जिकाकोशः_t सन्देशः_Registers[] = {
    R2, R3, R4, R5
};
compile_assert(
    consistent_सन्देशः_registers,
    परिमाण(सन्देशः_Registers) / परिमाण(सन्देशः_Registers[0]) == n_सन्देशः_Registers
);

ध्रुव पञ्जिकाकोशः_t frameRegisters[] = {
    FaultIP, SP, CPSR,
    R0, R1, R8, R9, R10, R11, R12
};
compile_assert(
    consistent_frame_registers,
    परिमाण(frameRegisters) / परिमाण(frameRegisters[0]) == n_frameRegisters
);

ध्रुव पञ्जिकाकोशः_t gpRegisters[] = {
    R2, R3, R4, R5, R6, R7, R14,
    TPIDRURW, TPIDRURO
};
compile_assert(
    consistent_gp_registers,
    परिमाण(gpRegisters) / परिमाण(gpRegisters[0]) == n_gpRegisters
);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
word_t getNBSendRecvDest(शून्य)
{
    प्रत्यागम get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), nbsendRecvDest);
}
#यदि_अन्तः
