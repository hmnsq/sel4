/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/arch/constants.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>

ਅਚਲ ਰਜਿਸਟਰ_t ਸੁਨੇਹਾ_Registers[] = {
    EDI,
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    EBP
#ਜੇ_ਅੰਤ
};
compile_assert(
    consistent_ਸੁਨੇਹਾ_registers,
    ਆਕਾਰ(ਸੁਨੇਹਾ_Registers) / ਆਕਾਰ(ਸੁਨੇਹਾ_Registers[0]) == n_ਸੁਨੇਹਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t ਚੌਖਟਾ_Registers[] = {
    FaultIP, ESP, FLAGS, EAX, EBX, ECX, EDX, ESI, EDI, EBP
};
compile_assert(
    consistent_ਚੌਖਟਾ_registers,
    ਆਕਾਰ(ਚੌਖਟਾ_Registers) / ਆਕਾਰ(ਚੌਖਟਾ_Registers[0]) == n_ਚੌਖਟਾ_Registers
);

ਅਚਲ ਰਜਿਸਟਰ_t gpRegisters[] = {
    FS_ਅਧਾਰ, GS_ਅਧਾਰ
};
compile_assert(
    consistent_gp_registers,
    ਆਕਾਰ(gpRegisters) / ਆਕਾਰ(gpRegisters[0]) == n_gpRegisters
);

ਖਾਲੀ ਬਹੁਲਕ_init_Context(user_context_t *context)
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

ਸ਼ਬਦ_t ਬਹੁਲਕ_sanitise_ਰਜਿਸਟਰ(ਰਜਿਸਟਰ_t reg, ਸ਼ਬਦ_t v)
{
    ਵਾਪਸ v;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸ਼ਬਦ_t getNBSendRecvDest(ਖਾਲੀ)
{
    seL4_IPCBuffer *ਬਫਰ = (seL4_IPCBuffer *) lookupIPCBuffer(false, ਗੰਢ_STATE(ksCurThread));
    ਜੇ (ਬਫਰ != ਨਿਰਾ) {
        ਵਾਪਸ ਬਫਰ->user_ਅੰਕੜੇ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}
#ਜੇ_ਅੰਤ
