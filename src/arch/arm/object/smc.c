/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
#ضمن <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
تعريفنوع بنية smc_دليلون_t {
    لفظ_t arg[انگ_SMC_REGS];
} smc_دليلون_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ساكن مضمن smc_دليلون_t_2 doSMC(smc_دليلون_t_2 smc_دليلون)
{
    سجل se_L4_لفظ r0 asm("x0") = smc_دليلون.arg[0];
    سجل se_L4_لفظ r1 asm("x1") = smc_دليلون.arg[1];
    سجل se_L4_لفظ r2 asm("x2") = smc_دليلون.arg[2];
    سجل se_L4_لفظ r3 asm("x3") = smc_دليلون.arg[3];
    سجل se_L4_لفظ r4 asm("x4") = smc_دليلون.arg[4];
    سجل se_L4_لفظ r5 asm("x5") = smc_دليلون.arg[5];
    سجل se_L4_لفظ r6 asm("x6") = smc_دليلون.arg[6];
    سجل se_L4_لفظ r7 asm("x7") = smc_دليلون.arg[7];

    asm متطاير("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_دليلون.arg[0] = r0;
    smc_دليلون.arg[1] = r1;
    smc_دليلون.arg[2] = r2;
    smc_دليلون.arg[3] = r3;
    smc_دليلون.arg[4] = r4;
    smc_دليلون.arg[5] = r5;
    smc_دليلون.arg[6] = r6;
    smc_دليلون.arg[7] = r7;
    أرجع smc_دليلون;
}

ساكن exception_t invokeSMCCall(smc_دليلون_t_2 smc_دليلون, bool_t call)
{
    smc_دليلون = doSMC(smc_دليلون);

    إذا (call) {
        tcb_t *thread = ڳنڍ_STATE(ksCurThread);
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);

        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        لكل (لفظ_t i = 0; i < انگ_SMC_REGS; i++) {
            سيٽ_MR(thread, ipc_بفر, i, smc_دليلون.arg[i]);
        }

        لفظ_t ڊيگهائي = ipc_بفر ? انگ_SMC_REGS : n_نياپو_Registers;
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(لفظ_t label, لفظ_t ڊيگهائي, cap_t cap, bool_t call, لفظ_t *بفر)
{
    إذا (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < انگ_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t badge = cap_smc_cap_get_capSMCBadge(cap);
    لفظ_t smc_func_id = getSyscallArg(0, بفر);

    إذا (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    smc_دليلون_t_2 smc_دليلون;
    لكل (لفظ_t i = 0; i < انگ_SMC_REGS; i++) {
        smc_دليلون.arg[i] = getSyscallArg(i, بفر);
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSMCCall(smc_دليلون, call);
}

#جيڪڏهن_پورو
