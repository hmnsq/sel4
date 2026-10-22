/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
#ضمن <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
تعريفنوع بنية smc_دلیلونه_t {
    کلمه_t arg[عدد_SMC_REGS];
} smc_دلیلونه_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ساكن مضمن smc_دلیلونه_t_2 doSMC(smc_دلیلونه_t_2 smc_دلیلونه)
{
    سجل se_L4_کلمه r0 asm("x0") = smc_دلیلونه.arg[0];
    سجل se_L4_کلمه r1 asm("x1") = smc_دلیلونه.arg[1];
    سجل se_L4_کلمه r2 asm("x2") = smc_دلیلونه.arg[2];
    سجل se_L4_کلمه r3 asm("x3") = smc_دلیلونه.arg[3];
    سجل se_L4_کلمه r4 asm("x4") = smc_دلیلونه.arg[4];
    سجل se_L4_کلمه r5 asm("x5") = smc_دلیلونه.arg[5];
    سجل se_L4_کلمه r6 asm("x6") = smc_دلیلونه.arg[6];
    سجل se_L4_کلمه r7 asm("x7") = smc_دلیلونه.arg[7];

    asm متطاير("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_دلیلونه.arg[0] = r0;
    smc_دلیلونه.arg[1] = r1;
    smc_دلیلونه.arg[2] = r2;
    smc_دلیلونه.arg[3] = r3;
    smc_دلیلونه.arg[4] = r4;
    smc_دلیلونه.arg[5] = r5;
    smc_دلیلونه.arg[6] = r6;
    smc_دلیلونه.arg[7] = r7;
    أرجع smc_دلیلونه;
}

ساكن exception_t invokeSMCCall(smc_دلیلونه_t_2 smc_دلیلونه, bool_t call)
{
    smc_دلیلونه = doSMC(smc_دلیلونه);

    إذا (call) {
        tcb_t *thread = غوټه_STATE(ksCurThread);
        کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);

        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        لكل (کلمه_t i = 0; i < عدد_SMC_REGS; i++) {
            سېټ_MR(thread, ipc_بفر, i, smc_دلیلونه.arg[i]);
        }

        کلمه_t اوږدتیا = ipc_بفر ? عدد_SMC_REGS : n_پیغام_Registers;
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(کلمه_t label, کلمه_t اوږدتیا, cap_t cap, bool_t call, کلمه_t *بفر)
{
    إذا (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < عدد_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t badge = cap_smc_cap_get_capSMCBadge(cap);
    کلمه_t smc_func_id = getSyscallArg(0, بفر);

    إذا (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    smc_دلیلونه_t_2 smc_دلیلونه;
    لكل (کلمه_t i = 0; i < عدد_SMC_REGS; i++) {
        smc_دلیلونه.arg[i] = getSyscallArg(i, بفر);
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSMCCall(smc_دلیلونه, call);
}

#د_که_پای
