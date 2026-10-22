/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <config.h>

#ifdef CONFIG_ALLOW_SMC_CALLS
#अन्तर्भाव <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
प्रकारनिर्देश संरचना smc_args_t_ {
    word_t arg[अङ्कः_SMC_REGS];
} smc_args_t;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
स्थिर अन्तःस्थ smc_args_t doSMC(smc_args_t smc_args)
{
    पञ्जिका seL4_Word r0 asm("x0") = smc_args.arg[0];
    पञ्जिका seL4_Word r1 asm("x1") = smc_args.arg[1];
    पञ्जिका seL4_Word r2 asm("x2") = smc_args.arg[2];
    पञ्जिका seL4_Word r3 asm("x3") = smc_args.arg[3];
    पञ्जिका seL4_Word r4 asm("x4") = smc_args.arg[4];
    पञ्जिका seL4_Word r5 asm("x5") = smc_args.arg[5];
    पञ्जिका seL4_Word r6 asm("x6") = smc_args.arg[6];
    पञ्जिका seL4_Word r7 asm("x7") = smc_args.arg[7];

    asm चञ्चल("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_args.arg[0] = r0;
    smc_args.arg[1] = r1;
    smc_args.arg[2] = r2;
    smc_args.arg[3] = r3;
    smc_args.arg[4] = r4;
    smc_args.arg[5] = r5;
    smc_args.arg[6] = r6;
    smc_args.arg[7] = r7;
    पटिनिवत्त smc_args;
}

स्थिर exception_t invokeSMCCall(smc_args_t smc_args, bool_t call)
{
    smc_args = doSMC(smc_args);

    सचे (call) {
        tcb_t *thread = ग्रन्थिः_STATE(ksCurThread);
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);
        अत्थाय (word_t i = 0; i < अङ्कः_SMC_REGS; i++) {
            setMR(thread, ipcBuffer, i, smc_args.arg[i]);
        }

        word_t length = ipcBuffer ? अङ्कः_SMC_REGS : n_सन्देशः_Registers;
        setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t length, cap_t cap, bool_t call, word_t *buffer)
{
    सचे (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (length < अङ्कः_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, buffer);

    सचे (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    smc_args_t smc_args;
    अत्थाय (word_t i = 0; i < अङ्कः_SMC_REGS; i++) {
        smc_args.arg[i] = getSyscallArg(i, buffer);
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeSMCCall(smc_args, call);
}

#endif
