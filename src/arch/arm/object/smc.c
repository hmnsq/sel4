/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#اگر_تعریف_شده CONFIG_ALLOW_SMC_CALLS
#ضمن <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
تعريفنوع بنية smc_آرگومانها_t {
    کلمه_t arg[رقم_SMC_REGS];
} smc_آرگومانها_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ساكن مضمن smc_آرگومانها_t_2 doSMC(smc_آرگومانها_t_2 smc_آرگومانها)
{
    سجل se_L4_کلمه r0 asm("x0") = smc_آرگومانها.arg[0];
    سجل se_L4_کلمه r1 asm("x1") = smc_آرگومانها.arg[1];
    سجل se_L4_کلمه r2 asm("x2") = smc_آرگومانها.arg[2];
    سجل se_L4_کلمه r3 asm("x3") = smc_آرگومانها.arg[3];
    سجل se_L4_کلمه r4 asm("x4") = smc_آرگومانها.arg[4];
    سجل se_L4_کلمه r5 asm("x5") = smc_آرگومانها.arg[5];
    سجل se_L4_کلمه r6 asm("x6") = smc_آرگومانها.arg[6];
    سجل se_L4_کلمه r7 asm("x7") = smc_آرگومانها.arg[7];

    asm متطاير("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_آرگومانها.arg[0] = r0;
    smc_آرگومانها.arg[1] = r1;
    smc_آرگومانها.arg[2] = r2;
    smc_آرگومانها.arg[3] = r3;
    smc_آرگومانها.arg[4] = r4;
    smc_آرگومانها.arg[5] = r5;
    smc_آرگومانها.arg[6] = r6;
    smc_آرگومانها.arg[7] = r7;
    أرجع smc_آرگومانها;
}

ساكن exception_t invokeSMCCall(smc_آرگومانها_t_2 smc_آرگومانها, bool_t call)
{
    smc_آرگومانها = doSMC(smc_آرگومانها);

    إذا (call) {
        tcb_t *thread = گره_STATE(ksCurThread);
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);

        مجموعه_ثبات(thread, badge_ثبات, 0);
        لكل (کلمه_t i = 0; i < رقم_SMC_REGS; i++) {
            مجموعه_MR(thread, ipc_میانگیر, i, smc_آرگومانها.arg[i]);
        }

        کلمه_t درازی = ipc_میانگیر ? رقم_SMC_REGS : n_پیام_Registers;
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(کلمه_t label, کلمه_t درازی, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    إذا (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < رقم_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t badge = cap_smc_cap_get_capSMCBadge(cap);
    کلمه_t smc_func_id = getSyscallArg(0, میانگیر);

    إذا (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    smc_آرگومانها_t_2 smc_آرگومانها;
    لكل (کلمه_t i = 0; i < رقم_SMC_REGS; i++) {
        smc_آرگومانها.arg[i] = getSyscallArg(i, میانگیر);
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSMCCall(smc_آرگومانها, call);
}

#پایان_اگر
