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
    word_t arg[अङ्क_SMC_REGS];
} smc_args_t;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
स्थिर अन्तःस्थ smc_args_t doSMC(smc_args_t smc_args)
{
    पञ्जिका seL4_Word r0 यंत्र("x0") = smc_args.arg[0];
    पञ्जिका seL4_Word r1 यंत्र("x1") = smc_args.arg[1];
    पञ्जिका seL4_Word r2 यंत्र("x2") = smc_args.arg[2];
    पञ्जिका seL4_Word r3 यंत्र("x3") = smc_args.arg[3];
    पञ्जिका seL4_Word r4 यंत्र("x4") = smc_args.arg[4];
    पञ्जिका seL4_Word r5 यंत्र("x5") = smc_args.arg[5];
    पञ्जिका seL4_Word r6 यंत्र("x6") = smc_args.arg[6];
    पञ्जिका seL4_Word r7 यंत्र("x7") = smc_args.arg[7];

    यंत्र चञ्चल("smc #0\n"
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
    फर्काऊ smc_args;
}

स्थिर exception_t invokeSMCCall(smc_args_t smc_args, bool_t call)
{
    smc_args = doSMC(smc_args);

    यदि (call) {
        tcb_t *thread = गाँठो_STATE(ksCurThread);
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);
        लागि (word_t i = 0; i < अङ्क_SMC_REGS; i++) {
            setMR(thread, ipcBuffer, i, smc_args.arg[i]);
        }

        word_t length = ipcBuffer ? अङ्क_SMC_REGS : n_सन्देश_Registers;
        setRegister(thread, सन्देश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Running);
    फर्काऊ EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t length, cap_t cap, bool_t call, word_t *buffer)
{
    यदि (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (length < अङ्क_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, buffer);

    यदि (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    smc_args_t smc_args;
    लागि (word_t i = 0; i < अङ्क_SMC_REGS; i++) {
        smc_args.arg[i] = getSyscallArg(i, buffer);
    }

    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ invokeSMCCall(smc_args, call);
}

#endif
