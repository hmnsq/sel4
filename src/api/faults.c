/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/faults.h>
#समावेश <api/syscall.h>
#समावेश <kernel/thread.h>
#समावेश <arch/kernel/thread.h>
#समावेश <machine/debug.h>
#अगर_घोषित CONFIG_KERNEL_MCS
#समावेश <mode/api/ipc_buffer.h>
#समावेश <object/schedcontext.h>
#पूर्ण_अगर

/* consistency with libsel4 */
compile_assert(Invalid_मूल, lookup_fault_invalid_मूल + 1 == se_L4_Invalid_मूल)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_संख्या, (word_t) n_exceptionMessage == se_L4_User_Exception_संख्या)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

अटल अंतरभूत अचिन्हित पूर्णांक
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, अचिन्हित पूर्णांक offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    अगर (offset == seL4_CapFault_LookupFailureType) {
        निश्चित(offset + 1 == se_L4_Cap_Fault_Bits_बायाँ);
        निश्चित(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        निश्चित(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        निश्चित(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } अन्यथा {
        निश्चित(offset == 1);
    }

    चयन (lufType) {
    हाल lookup_fault_invalid_मूल:
        वापस i;

    हाल lookup_fault_missing_capability:
        वापस setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_बायाँ(luf));

    हाल lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_बायाँ(luf));
        वापस setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    हाल lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_बायाँ(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        वापस setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    शेष:
        fail("Invalid lookup failure");
    }
}

अटल अंतरभूत व्योम copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t लंबाई)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    क्रम (i = 0; i < MIN(लंबाई, n_संदेश_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, संदेश_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    अगर (i < लंबाई) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        अगर (sendBuf) {
            क्रम (; i < लंबाई; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

अटल अंतरभूत व्योम copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t लंबाई, word_t *receiveIPCBuffer)
{
    word_t i;
    क्रम (i = 0; i < MIN(लंबाई, n_संदेश_Registers); i++) {
        setRegister(receiver, संदेश_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    अगर (receiveIPCBuffer) {
        क्रम (; i < लंबाई; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, संदेश_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t लंबाई = se_L4_Message_Info_get_लंबाई(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    चयन (seL4_Fault_get_seL4_FaultType(fault)) {
    हाल seL4_Fault_CapFault:
        वापस true;

    हाल seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(लंबाई, n_syscallMessage));
        वापस (label == 0);

    हाल seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(लंबाई, n_exceptionMessage));
        वापस (label == 0);

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(लंबाई, n_timeoutMessage));
        वापस (label == 0);
#पूर्ण_अगर
#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    हाल seL4_Fault_DebugException: {
        word_t n_instrs;

        अगर (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            वापस (label == 0);
        }

        अगर (लंबाई < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
            /* A single-step reply doesn't mean much if it isn't composed of the bp
             * number and number of instructions to skip. But even if both aren't
             * set, we can still allow the thread to continue because replying
             * should uniformly resume thread execution, based on the general seL4
             * API model.
             *
             * If it was single-step, but no reply registers were set, just
             * default to skipping 1 and continuing.
             *
             * On x86, bp_num actually doesn't matter for single-stepping
             * because single-stepping doesn't use a hardware register -- it
             * uses EFLAGS.TF.
             */
            n_instrs = 1;
        } अन्यथा {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, संदेश_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        अगर (res.type != seL4_NoError) {
            वापस false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        वापस (label == 0);
    }
#पूर्ण_अगर

    शेष:
        वापस Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    चयन (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    हाल seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        वापस setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    हाल seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        वापस setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_संख्या(sender->tcbFault));
    }

    हाल seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_संख्या(sender->tcbFault));
        वापस setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल seL4_Fault_Timeout: {
        अगर (sender->tcbSchedContext) {
            word_t len;
            समय_प्रकार consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, se_L4_Timeout_तथ्य,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            वापस mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } अन्यथा {
            वापस setMR(receiver, receiveIPCBuffer, se_L4_Timeout_तथ्य,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#पूर्ण_अगर
#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    हाल seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        अचिन्हित पूर्णांक ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        अगर (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_संख्या,
                        se_L4_Fault_Debug_Exception_get_breakpoint_संख्या(sender->tcbFault));
        }
        वापस ret;
    }
#पूर्ण_अगर /* CONFIG_HARDWARE_DEBUG_API */

    शेष:
        वापस Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
