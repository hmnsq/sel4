/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/faults.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <arch/kernel/thread.h>
#அந்தர்பாவ <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#அந்தர்பாவ <mode/api/ipc_buffer.h>
#அந்தர்பாவ <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_வேர், lookup_fault_invalid_வேர் + 1 == se_L4_Invalid_வேர்)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_எண், (word_t) n_exceptionMessage == se_L4_User_Exception_எண்)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

நிலையான உள்வரி unsigned முழு
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned முழு offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    எனில் (offset == seL4_CapFault_LookupFailureType) {
        ப்ரதிஜ்ஞா(offset + 1 == se_L4_Cap_Fault_Bits_இடது);
        ப்ரதிஜ்ஞா(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        ப்ரதிஜ்ஞா(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        ப்ரதிஜ்ஞா(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } இல்லையெனில் {
        ப்ரதிஜ்ஞா(offset == 1);
    }

    தேர்வு (lufType) {
    நிலை lookup_fault_invalid_வேர்:
        திரும்பு i;

    நிலை lookup_fault_missing_capability:
        திரும்பு setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_இடது(luf));

    நிலை lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_இடது(luf));
        திரும்பு setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    நிலை lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_இடது(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        திரும்பு setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    இயல்பு:
        fail("Invalid lookup failure");
    }
}

நிலையான உள்வரி வெறுமை copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    க்கு (i = 0; i < MIN(length, n_செய்தி_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, செய்தி_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    எனில் (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        எனில் (sendBuf) {
            க்கு (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

நிலையான உள்வரி வெறுமை copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    க்கு (i = 0; i < MIN(length, n_செய்தி_Registers); i++) {
        setRegister(receiver, செய்தி_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    எனில் (receiveIPCBuffer) {
        க்கு (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, செய்தி_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    தேர்வு (seL4_Fault_get_seL4_FaultType(fault)) {
    நிலை seL4_Fault_CapFault:
        திரும்பு true;

    நிலை seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        திரும்பு (label == 0);

    நிலை seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        திரும்பு (label == 0);

#ifdef CONFIG_KERNEL_MCS
    நிலை seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        திரும்பு (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    நிலை seL4_Fault_DebugException: {
        word_t n_instrs;

        எனில் (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            திரும்பு (label == 0);
        }

        எனில் (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } இல்லையெனில் {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, செய்தி_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        எனில் (res.type != seL4_NoError) {
            திரும்பு false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        திரும்பு (label == 0);
    }
#endif

    இயல்பு:
        திரும்பு Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    தேர்வு (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    நிலை seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        திரும்பு setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    நிலை seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        திரும்பு setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_எண்(sender->tcbFault));
    }

    நிலை seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_எண்(sender->tcbFault));
        திரும்பு setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    நிலை seL4_Fault_Timeout: {
        எனில் (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            திரும்பு mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } இல்லையெனில் {
            திரும்பு setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    நிலை seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned முழு ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        எனில் (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_எண்,
                        se_L4_Fault_Debug_Exception_get_breakpoint_எண்(sender->tcbFault));
        }
        திரும்பு ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    இயல்பு:
        திரும்பு Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
