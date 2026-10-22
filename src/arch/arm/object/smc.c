/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#ifdef CONFIG_ALLOW_SMC_CALLS
#ضمن <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
تعريفنوع بنية smc_حجوجي_t {
    word_t arg[لمب_SMC_REGS];
} smc_حجوجي_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ساكن مضمن smc_حجوجي_t_2 doSMC(smc_حجوجي_t_2 smc_حجوجي)
{
    سجل seL4_Word r0 asm("x0") = smc_حجوجي.arg[0];
    سجل seL4_Word r1 asm("x1") = smc_حجوجي.arg[1];
    سجل seL4_Word r2 asm("x2") = smc_حجوجي.arg[2];
    سجل seL4_Word r3 asm("x3") = smc_حجوجي.arg[3];
    سجل seL4_Word r4 asm("x4") = smc_حجوجي.arg[4];
    سجل seL4_Word r5 asm("x5") = smc_حجوجي.arg[5];
    سجل seL4_Word r6 asm("x6") = smc_حجوجي.arg[6];
    سجل seL4_Word r7 asm("x7") = smc_حجوجي.arg[7];

    asm متطاير("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_حجوجي.arg[0] = r0;
    smc_حجوجي.arg[1] = r1;
    smc_حجوجي.arg[2] = r2;
    smc_حجوجي.arg[3] = r3;
    smc_حجوجي.arg[4] = r4;
    smc_حجوجي.arg[5] = r5;
    smc_حجوجي.arg[6] = r6;
    smc_حجوجي.arg[7] = r7;
    أرجع smc_حجوجي;
}

ساكن exception_t invokeSMCCall(smc_حجوجي_t_2 smc_حجوجي, bool_t call)
{
    smc_حجوجي = doSMC(smc_حجوجي);

    إذا (call) {
        tcb_t *thread = كلي_STATE(ksCurThread);
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);
        لكل (word_t i = 0; i < لمب_SMC_REGS; i++) {
            setMR(thread, ipcBuffer, i, smc_حجوجي.arg[i]);
        }

        word_t length = ipcBuffer ? لمب_SMC_REGS : n_سكو_Registers;
        setRegister(thread, سكو_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(كلي_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t length, cap_t cap, bool_t call, word_t *buffer)
{
    إذا (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (length < لمب_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, buffer);

    إذا (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    smc_حجوجي_t_2 smc_حجوجي;
    لكل (word_t i = 0; i < لمب_SMC_REGS; i++) {
        smc_حجوجي.arg[i] = getSyscallArg(i, buffer);
    }

    setThreadState(كلي_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSMCCall(smc_حجوجي, call);
}

#endif
