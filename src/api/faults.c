/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/faults.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#अन्तर्भाव <mode/api/ipc_buffer.h>
#अन्तर्भाव <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_मूलम्, lookup_fault_invalid_मूलम् + 1 == se_L4_Invalid_मूलम्)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_सङ्ख्या, (word_t) n_exceptionMessage == se_L4_User_Exception_सङ्ख्या)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

स्थिर अन्तःस्थ unsigned पूर्णाङ्क
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned पूर्णाङ्क offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    जइ (offset == seL4_CapFault_LookupFailureType) {
        प्रतिज्ञा(offset + 1 == se_L4_Cap_Fault_Bits_वामः);
        प्रतिज्ञा(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        प्रतिज्ञा(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        प्रतिज्ञा(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } अण्णहा {
        प्रतिज्ञा(offset == 1);
    }

    विकल्प (lufType) {
    पक्ष lookup_fault_invalid_मूलम्:
        प्रत्यागम i;

    पक्ष lookup_fault_missing_capability:
        प्रत्यागम setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_वामः(luf));

    पक्ष lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_वामः(luf));
        प्रत्यागम setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    पक्ष lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_वामः(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        प्रत्यागम setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    स्वभाव:
        fail("Invalid lookup failure");
    }
}

स्थिर अन्तःस्थ शून्य copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    प्रति (i = 0; i < MIN(length, n_सन्देशः_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, सन्देशः_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    जइ (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        जइ (sendBuf) {
            प्रति (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

स्थिर अन्तःस्थ शून्य copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    प्रति (i = 0; i < MIN(length, n_सन्देशः_Registers); i++) {
        setRegister(receiver, सन्देशः_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    जइ (receiveIPCBuffer) {
        प्रति (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, सन्देशः_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    विकल्प (seL4_Fault_get_seL4_FaultType(fault)) {
    पक्ष seL4_Fault_CapFault:
        प्रत्यागम true;

    पक्ष seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        प्रत्यागम (label == 0);

    पक्ष seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        प्रत्यागम (label == 0);

#ifdef CONFIG_KERNEL_MCS
    पक्ष seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        प्रत्यागम (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    पक्ष seL4_Fault_DebugException: {
        word_t n_instrs;

        जइ (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            प्रत्यागम (label == 0);
        }

        जइ (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } अण्णहा {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, सन्देशः_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        जइ (res.type != seL4_NoError) {
            प्रत्यागम false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        प्रत्यागम (label == 0);
    }
#endif

    स्वभाव:
        प्रत्यागम Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    विकल्प (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    पक्ष seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        प्रत्यागम setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    पक्ष seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        प्रत्यागम setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_सङ्ख्या(sender->tcbFault));
    }

    पक्ष seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_सङ्ख्या(sender->tcbFault));
        प्रत्यागम setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    पक्ष seL4_Fault_Timeout: {
        जइ (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            प्रत्यागम mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } अण्णहा {
            प्रत्यागम setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    पक्ष seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned पूर्णाङ्क ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        जइ (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_सङ्ख्या,
                        se_L4_Fault_Debug_Exception_get_breakpoint_सङ्ख्या(sender->tcbFault));
        }
        प्रत्यागम ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    स्वभाव:
        प्रत्यागम Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
