/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
#ಅನ್ತರ್ಭಾವ <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ smc_ವಾದಗಳು_t {
    ಶಬ್ದ_t arg[ಅಂಕೆ_SMC_REGS];
} smc_ವಾದಗಳು_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ smc_ವಾದಗಳು_t_2 doSMC(smc_ವಾದಗಳು_t_2 smc_ವಾದಗಳು)
{
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r0 asm("x0") = smc_ವಾದಗಳು.arg[0];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r1 asm("x1") = smc_ವಾದಗಳು.arg[1];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r2 asm("x2") = smc_ವಾದಗಳು.arg[2];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r3 asm("x3") = smc_ವಾದಗಳು.arg[3];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r4 asm("x4") = smc_ವಾದಗಳು.arg[4];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r5 asm("x5") = smc_ವಾದಗಳು.arg[5];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r6 asm("x6") = smc_ವಾದಗಳು.arg[6];
    ನೋಂದಣಿ se_L4_ಶಬ್ದ r7 asm("x7") = smc_ವಾದಗಳು.arg[7];

    asm ಅಸ್ಥಿರ("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_ವಾದಗಳು.arg[0] = r0;
    smc_ವಾದಗಳು.arg[1] = r1;
    smc_ವಾದಗಳು.arg[2] = r2;
    smc_ವಾದಗಳು.arg[3] = r3;
    smc_ವಾದಗಳು.arg[4] = r4;
    smc_ವಾದಗಳು.arg[5] = r5;
    smc_ವಾದಗಳು.arg[6] = r6;
    smc_ವಾದಗಳು.arg[7] = r7;
    ಹಿಂತಿರುಗಿ smc_ವಾದಗಳು;
}

ಸ್ಥಿರ exception_t invokeSMCCall(smc_ವಾದಗಳು_t_2 smc_ವಾದಗಳು, bool_t call)
{
    smc_ವಾದಗಳು = doSMC(smc_ವಾದಗಳು);

    ಆದರೆ (call) {
        tcb_t *thread = ಗಂಟು_STATE(ksCurThread);
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);

        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಅಂಕೆ_SMC_REGS; i++) {
            ಗಣ_MR(thread, ipc_ಬಫರ್, i, smc_ವಾದಗಳು.arg[i]);
        }

        ಶಬ್ದ_t ಉದ್ದಳತೆ = ipc_ಬಫರ್ ? ಅಂಕೆ_SMC_REGS : n_ಸಂದೇಶ_Registers;
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < ಅಂಕೆ_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಶಬ್ದ_t badge = cap_smc_cap_get_capSMCBadge(cap);
    ಶಬ್ದ_t smc_func_id = getSyscallArg(0, ಬಫರ್);

    ಆದರೆ (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    smc_ವಾದಗಳು_t_2 smc_ವಾದಗಳು;
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಅಂಕೆ_SMC_REGS; i++) {
        smc_ವಾದಗಳು.arg[i] = getSyscallArg(i, ಬಫರ್);
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeSMCCall(smc_ವಾದಗಳು, call);
}

#ಆದರೆ_ಅಂತ್ಯ
