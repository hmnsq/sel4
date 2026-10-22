/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#समावेश <config.h>

#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
#समावेश <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
प्रकार काष्ठा smc_args_t_ {
    word_t arg[अंक_SMC_REGS];
} smc_args_t;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
अटल अंतरभूत smc_args_t doSMC(smc_args_t smc_args)
{
    रेजिस्टर seL4_Word r0 यंत्र("x0") = smc_args.arg[0];
    रेजिस्टर seL4_Word r1 यंत्र("x1") = smc_args.arg[1];
    रेजिस्टर seL4_Word r2 यंत्र("x2") = smc_args.arg[2];
    रेजिस्टर seL4_Word r3 यंत्र("x3") = smc_args.arg[3];
    रेजिस्टर seL4_Word r4 यंत्र("x4") = smc_args.arg[4];
    रेजिस्टर seL4_Word r5 यंत्र("x5") = smc_args.arg[5];
    रेजिस्टर seL4_Word r6 यंत्र("x6") = smc_args.arg[6];
    रेजिस्टर seL4_Word r7 यंत्र("x7") = smc_args.arg[7];

    यंत्र अस्थिर("smc #0\n"
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
    वापस smc_args;
}

अटल exception_t invokeSMCCall(smc_args_t smc_args, bool_t call)
{
    smc_args = doSMC(smc_args);

    अगर (call) {
        tcb_t *thread = गाँठ_STATE(ksCurThread);
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);
        क्रम (word_t i = 0; i < अंक_SMC_REGS; i++) {
            setMR(thread, ipc_बफर, i, smc_args.arg[i]);
        }

        word_t लंबाई = ipc_बफर ? अंक_SMC_REGS : n_संदेश_Registers;
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t लंबाई, cap_t cap, bool_t call, word_t *बफर)
{
    अगर (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (लंबाई < अंक_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, बफर);

    अगर (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    smc_args_t smc_args;
    क्रम (word_t i = 0; i < अंक_SMC_REGS; i++) {
        smc_args.arg[i] = getSyscallArg(i, बफर);
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSMCCall(smc_args, call);
}

#पूर्ण_अगर
