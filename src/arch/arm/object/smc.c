/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
#شامل <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
قسمتعریف ساخت smc_دلائل_t {
    لفظ_t arg[عدد_SMC_REGS];
} smc_دلائل_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ساکن اندرونی smc_دلائل_t_2 doSMC(smc_دلائل_t_2 smc_دلائل)
{
    اندراج se_L4_لفظ r0 asm("x0") = smc_دلائل.arg[0];
    اندراج se_L4_لفظ r1 asm("x1") = smc_دلائل.arg[1];
    اندراج se_L4_لفظ r2 asm("x2") = smc_دلائل.arg[2];
    اندراج se_L4_لفظ r3 asm("x3") = smc_دلائل.arg[3];
    اندراج se_L4_لفظ r4 asm("x4") = smc_دلائل.arg[4];
    اندراج se_L4_لفظ r5 asm("x5") = smc_دلائل.arg[5];
    اندراج se_L4_لفظ r6 asm("x6") = smc_دلائل.arg[6];
    اندراج se_L4_لفظ r7 asm("x7") = smc_دلائل.arg[7];

    asm متلون("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_دلائل.arg[0] = r0;
    smc_دلائل.arg[1] = r1;
    smc_دلائل.arg[2] = r2;
    smc_دلائل.arg[3] = r3;
    smc_دلائل.arg[4] = r4;
    smc_دلائل.arg[5] = r5;
    smc_دلائل.arg[6] = r6;
    smc_دلائل.arg[7] = r7;
    واپس smc_دلائل;
}

ساکن exception_t invokeSMCCall(smc_دلائل_t_2 smc_دلائل, bool_t call)
{
    smc_دلائل = doSMC(smc_دلائل);

    اگر (call) {
        tcb_t *thread = گرہ_STATE(ksCurThread);
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);

        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        برائے (لفظ_t i = 0; i < عدد_SMC_REGS; i++) {
            سیٹ_MR(thread, ipc_بفر, i, smc_دلائل.arg[i]);
        }

        لفظ_t لمبائی = ipc_بفر ? عدد_SMC_REGS : n_پیغام_Registers;
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(لفظ_t label, لفظ_t لمبائی, cap_t cap, bool_t call, لفظ_t *بفر)
{
    اگر (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < عدد_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t badge = cap_smc_cap_get_capSMCBadge(cap);
    لفظ_t smc_func_id = getSyscallArg(0, بفر);

    اگر (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    smc_دلائل_t_2 smc_دلائل;
    برائے (لفظ_t i = 0; i < عدد_SMC_REGS; i++) {
        smc_دلائل.arg[i] = getSyscallArg(i, بفر);
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeSMCCall(smc_دلائل, call);
}

#اگر_ختم
