/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#অন্তর্ভাব <config.h>

#ifdef CONFIG_ALLOW_SMC_CALLS
#অন্তর্ভাব <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
প্রকারলেখ কাঠামো smc_args_t_ {
    word_t arg[অঙ্ক_SMC_REGS];
} smc_args_t;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
স্থির অন্তঃরেখ smc_args_t doSMC(smc_args_t smc_args)
{
    নিবন্ধ seL4_Word r0 asm("x0") = smc_args.arg[0];
    নিবন্ধ seL4_Word r1 asm("x1") = smc_args.arg[1];
    নিবন্ধ seL4_Word r2 asm("x2") = smc_args.arg[2];
    নিবন্ধ seL4_Word r3 asm("x3") = smc_args.arg[3];
    নিবন্ধ seL4_Word r4 asm("x4") = smc_args.arg[4];
    নিবন্ধ seL4_Word r5 asm("x5") = smc_args.arg[5];
    নিবন্ধ seL4_Word r6 asm("x6") = smc_args.arg[6];
    নিবন্ধ seL4_Word r7 asm("x7") = smc_args.arg[7];

    asm অস্থির("smc #0\n"
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
    ফেরত smc_args;
}

স্থির exception_t invokeSMCCall(smc_args_t smc_args, bool_t call)
{
    smc_args = doSMC(smc_args);

    যদি (call) {
        tcb_t *thread = গ্রন্থি_STATE(ksCurThread);
        word_t *ipc_বাফার = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);
        জন্য (word_t i = 0; i < অঙ্ক_SMC_REGS; i++) {
            setMR(thread, ipc_বাফার, i, smc_args.arg[i]);
        }

        word_t দৈর্ঘ্য = ipc_বাফার ? অঙ্ক_SMC_REGS : n_বার্তা_Registers;
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, দৈর্ঘ্য)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    ফেরত EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t দৈর্ঘ্য, cap_t cap, bool_t call, word_t *বাফার)
{
    যদি (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (দৈর্ঘ্য < অঙ্ক_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, বাফার);

    যদি (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    smc_args_t smc_args;
    জন্য (word_t i = 0; i < অঙ্ক_SMC_REGS; i++) {
        smc_args.arg[i] = getSyscallArg(i, বাফার);
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeSMCCall(smc_args, call);
}

#endif
