/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
#అన్తర్భావ <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
రకనిర్వచనం నిర్మాణం smc_వాదనలు_t {
    మాట_t arg[అంకె_SMC_REGS];
} smc_వాదనలు_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
స్థిర అంతఃవరుస smc_వాదనలు_t_2 doSMC(smc_వాదనలు_t_2 smc_వాదనలు)
{
    నమోదు se_L4_మాట r0 asm("x0") = smc_వాదనలు.arg[0];
    నమోదు se_L4_మాట r1 asm("x1") = smc_వాదనలు.arg[1];
    నమోదు se_L4_మాట r2 asm("x2") = smc_వాదనలు.arg[2];
    నమోదు se_L4_మాట r3 asm("x3") = smc_వాదనలు.arg[3];
    నమోదు se_L4_మాట r4 asm("x4") = smc_వాదనలు.arg[4];
    నమోదు se_L4_మాట r5 asm("x5") = smc_వాదనలు.arg[5];
    నమోదు se_L4_మాట r6 asm("x6") = smc_వాదనలు.arg[6];
    నమోదు se_L4_మాట r7 asm("x7") = smc_వాదనలు.arg[7];

    asm అస్థిర("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_వాదనలు.arg[0] = r0;
    smc_వాదనలు.arg[1] = r1;
    smc_వాదనలు.arg[2] = r2;
    smc_వాదనలు.arg[3] = r3;
    smc_వాదనలు.arg[4] = r4;
    smc_వాదనలు.arg[5] = r5;
    smc_వాదనలు.arg[6] = r6;
    smc_వాదనలు.arg[7] = r7;
    తిరిగి smc_వాదనలు;
}

స్థిర exception_t invokeSMCCall(smc_వాదనలు_t_2 smc_వాదనలు, bool_t call)
{
    smc_వాదనలు = doSMC(smc_వాదనలు);

    ఒకవేళ (call) {
        tcb_t *thread = కణుపు_STATE(ksCurThread);
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);

        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        కొరకు (మాట_t i = 0; i < అంకె_SMC_REGS; i++) {
            సమితి_MR(thread, ipc_బఫరు, i, smc_వాదనలు.arg[i]);
        }

        మాట_t నిడివి = ipc_బఫరు ? అంకె_SMC_REGS : n_సందేశం_Registers;
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(మాట_t label, మాట_t నిడివి, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఒకవేళ (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < అంకె_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మాట_t badge = cap_smc_cap_get_capSMCBadge(cap);
    మాట_t smc_func_id = getSyscallArg(0, బఫరు);

    ఒకవేళ (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    smc_వాదనలు_t_2 smc_వాదనలు;
    కొరకు (మాట_t i = 0; i < అంకె_SMC_REGS; i++) {
        smc_వాదనలు.arg[i] = getSyscallArg(i, బఫరు);
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeSMCCall(smc_వాదనలు, call);
}

#అయితే_ముగింపు
