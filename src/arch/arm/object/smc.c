/*
 * Copyright 2021, DornerWorks Ltd.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
#अन्तर्भाव <arch/object/smc.h>

/** Wrapped in a struct for verification, so the array does not decay to a pointer. */
प्रकारनिर्देश संरचना smc_तर्काः_t {
    word_t arg[अङ्कः_SMC_REGS];
} smc_तर्काः_t_2;

/** We need the function result to be returned on the stack for verification.
  * In reality, this function is inlined and the compiler should optimise all
  * of this argument shuffling away. */
स्थिर अन्तःस्थ smc_तर्काः_t_2 doSMC(smc_तर्काः_t_2 smc_तर्काः)
{
    पञ्जिका seL4_Word r0 asm("x0") = smc_तर्काः.arg[0];
    पञ्जिका seL4_Word r1 asm("x1") = smc_तर्काः.arg[1];
    पञ्जिका seL4_Word r2 asm("x2") = smc_तर्काः.arg[2];
    पञ्जिका seL4_Word r3 asm("x3") = smc_तर्काः.arg[3];
    पञ्जिका seL4_Word r4 asm("x4") = smc_तर्काः.arg[4];
    पञ्जिका seL4_Word r5 asm("x5") = smc_तर्काः.arg[5];
    पञ्जिका seL4_Word r6 asm("x6") = smc_तर्काः.arg[6];
    पञ्जिका seL4_Word r7 asm("x7") = smc_तर्काः.arg[7];

    asm चञ्चल("smc #0\n"
                 : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3),
                 "+r"(r4), "+r"(r5), "+r"(r6), "+r"(r7)
                 :: "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "memory");

    smc_तर्काः.arg[0] = r0;
    smc_तर्काः.arg[1] = r1;
    smc_तर्काः.arg[2] = r2;
    smc_तर्काः.arg[3] = r3;
    smc_तर्काः.arg[4] = r4;
    smc_तर्काः.arg[5] = r5;
    smc_तर्काः.arg[6] = r6;
    smc_तर्काः.arg[7] = r7;
    प्रत्यागम smc_तर्काः;
}

स्थिर exception_t invokeSMCCall(smc_तर्काः_t_2 smc_तर्काः, bool_t call)
{
    smc_तर्काः = doSMC(smc_तर्काः);

    यदि (call) {
        tcb_t *thread = ग्रन्थिः_STATE(ksCurThread);
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);

        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        प्रति (word_t i = 0; i < अङ्कः_SMC_REGS; i++) {
            समुच्चयः_MR(thread, ipc_बफर, i, smc_तर्काः.arg[i]);
        }

        word_t दीर्घता = ipc_बफर ? अङ्कः_SMC_REGS : n_सन्देशः_Registers;
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeARMSMCInvocation(word_t label, word_t दीर्घता, cap_t cap, bool_t call, word_t *बफर)
{
    यदि (label != ARMSMCCall) {
        userError("ARMSMCInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < अङ्कः_SMC_REGS) {
        userError("ARMSMCCall: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = cap_smc_cap_get_capSMCBadge(cap);
    word_t smc_func_id = getSyscallArg(0, बफर);

    यदि (badge != 0 && badge != smc_func_id) {
        userError("ARMSMCCall: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    smc_तर्काः_t_2 smc_तर्काः;
    प्रति (word_t i = 0; i < अङ्कः_SMC_REGS; i++) {
        smc_तर्काः.arg[i] = getSyscallArg(i, बफर);
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeSMCCall(smc_तर्काः, call);
}

#यदि_अन्तः
