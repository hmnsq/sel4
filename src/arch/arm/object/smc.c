/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#அந்தர்பாவ <config.h>

#ifdef CONFIG_ALLOW_SMC_CALLS
#அந்தர்பாவ <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
வகைவரை கட்டமைப்பு smc_args_t_ {
    word_t arg[இலக்கம்_SMC_REGS];
} smc_args_t;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
நிலையான உள்வரி smc_args_t doSMC(smc_args_t smc_args)
{
    பதிவு seL4_Word r0 asm("x0") = smc_args.arg[0];
    பதிவு seL4_Word r1 asm("x1") = smc_args.arg[1];
    பதிவு seL4_Word r2 asm("x2") = smc_args.arg[2];
    பதிவு seL4_Word r3 asm("x3") = smc_args.arg[3];
    பதிவு seL4_Word r4 asm("x4") = smc_args.arg[4];
    பதிவு seL4_Word r5 asm("x5") = smc_args.arg[5];
    பதிவு seL4_Word r6 asm("x6") = smc_args.arg[6];
    பதிவு seL4_Word r7 asm("x7") = smc_args.arg[7];

    asm நிலையற்ற("smc #0\n"
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
    திரும்பு smc_args;
}

நிலையான exception_t invokeSMCCall(smc_args_t smc_args, bool_t call)
{
    smc_args = doSMC(smc_args);

    எனில் (call) {
        tcb_t *thread = முனை_STATE(ksCurThread);
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);
        க்கு (word_t i = 0; i < இலக்கம்_SMC_REGS; i++) {
            setMR(thread, ipcBuffer, i, smc_args.arg[i]);
        }

        word_t length = ipcBuffer ? இலக்கம்_SMC_REGS : n_செய்தி_Registers;
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t length, cap_t cap, bool_t call, word_t *buffer)
{
    எனில் (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (length < இலக்கம்_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, buffer);

    எனில் (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    smc_args_t smc_args;
    க்கு (word_t i = 0; i < இலக்கம்_SMC_REGS; i++) {
        smc_args.arg[i] = getSyscallArg(i, buffer);
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSMCCall(smc_args, call);
}

#endif
