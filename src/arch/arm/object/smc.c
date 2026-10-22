/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ smc_ਦਲੀਲਾਂ_t {
    ਸ਼ਬਦ_t arg[ਅੰਕ_SMC_REGS];
} smc_ਦਲੀਲਾਂ_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ਸਥਿਰ ਸਤਰਅੰਦਰ smc_ਦਲੀਲਾਂ_t_2 doSMC(smc_ਦਲੀਲਾਂ_t_2 smc_ਦਲੀਲਾਂ)
{
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r0 asm("x0") = smc_ਦਲੀਲਾਂ.arg[0];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r1 asm("x1") = smc_ਦਲੀਲਾਂ.arg[1];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r2 asm("x2") = smc_ਦਲੀਲਾਂ.arg[2];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r3 asm("x3") = smc_ਦਲੀਲਾਂ.arg[3];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r4 asm("x4") = smc_ਦਲੀਲਾਂ.arg[4];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r5 asm("x5") = smc_ਦਲੀਲਾਂ.arg[5];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r6 asm("x6") = smc_ਦਲੀਲਾਂ.arg[6];
    ਰਜਿਸਟਰ se_L4_ਸ਼ਬਦ r7 asm("x7") = smc_ਦਲੀਲਾਂ.arg[7];

    asm ਅਸਥਿਰ("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_ਦਲੀਲਾਂ.arg[0] = r0;
    smc_ਦਲੀਲਾਂ.arg[1] = r1;
    smc_ਦਲੀਲਾਂ.arg[2] = r2;
    smc_ਦਲੀਲਾਂ.arg[3] = r3;
    smc_ਦਲੀਲਾਂ.arg[4] = r4;
    smc_ਦਲੀਲਾਂ.arg[5] = r5;
    smc_ਦਲੀਲਾਂ.arg[6] = r6;
    smc_ਦਲੀਲਾਂ.arg[7] = r7;
    ਵਾਪਸ smc_ਦਲੀਲਾਂ;
}

ਸਥਿਰ exception_t invokeSMCCall(smc_ਦਲੀਲਾਂ_t_2 smc_ਦਲੀਲਾਂ, bool_t call)
{
    smc_ਦਲੀਲਾਂ = doSMC(smc_ਦਲੀਲਾਂ);

    ਜੇ (call) {
        tcb_t *thread = ਗੰਢ_STATE(ksCurThread);
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);

        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਅੰਕ_SMC_REGS; i++) {
            ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, i, smc_ਦਲੀਲਾਂ.arg[i]);
        }

        ਸ਼ਬਦ_t ਲੰਮਾਈ = ipc_ਬਫਰ ? ਅੰਕ_SMC_REGS : n_ਸੁਨੇਹਾ_Registers;
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < ਅੰਕ_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸ਼ਬਦ_t badge = cap_smc_cap_get_capSMCBadge(cap);
    ਸ਼ਬਦ_t smc_func_id = getSyscallArg(0, ਬਫਰ);

    ਜੇ (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    smc_ਦਲੀਲਾਂ_t_2 smc_ਦਲੀਲਾਂ;
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਅੰਕ_SMC_REGS; i++) {
        smc_ਦਲੀਲਾਂ.arg[i] = getSyscallArg(i, ਬਫਰ);
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeSMCCall(smc_ਦਲੀਲਾਂ, call);
}

#ਜੇ_ਅੰਤ
