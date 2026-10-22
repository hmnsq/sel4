/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/faults.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <arch/kernel/thread.h>
#અન્તર્ભાવ <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#અન્તર્ભાવ <mode/api/ipc_buffer.h>
#અન્તર્ભાવ <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_મૂળ, lookup_fault_invalid_મૂળ + 1 == se_L4_Invalid_મૂળ)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_સંખ્યા, (word_t) n_exceptionMessage == se_L4_User_Exception_સંખ્યા)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

સ્થિર અંતઃરેખ unsigned પૂર્ણાંક
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned પૂર્ણાંક offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    જો (offset == seL4_CapFault_LookupFailureType) {
        પ્રતિજ્ઞા(offset + 1 == se_L4_Cap_Fault_Bits_ડાબું);
        પ્રતિજ્ઞા(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        પ્રતિજ્ઞા(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        પ્રતિજ્ઞા(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } નહીંતર {
        પ્રતિજ્ઞા(offset == 1);
    }

    પસંદગી (lufType) {
    સ્થિતિ lookup_fault_invalid_મૂળ:
        પાછા i;

    સ્થિતિ lookup_fault_missing_capability:
        પાછા setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_ડાબું(luf));

    સ્થિતિ lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_ડાબું(luf));
        પાછા setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    સ્થિતિ lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_ડાબું(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        પાછા setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    મૂળભૂત:
        fail("Invalid lookup failure");
    }
}

સ્થિર અંતઃરેખ શૂન્ય copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    માટે (i = 0; i < MIN(length, n_સંદેશ_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, સંદેશ_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    જો (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        જો (sendBuf) {
            માટે (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

સ્થિર અંતઃરેખ શૂન્ય copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    માટે (i = 0; i < MIN(length, n_સંદેશ_Registers); i++) {
        setRegister(receiver, સંદેશ_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    જો (receiveIPCBuffer) {
        માટે (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, સંદેશ_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    પસંદગી (seL4_Fault_get_seL4_FaultType(fault)) {
    સ્થિતિ seL4_Fault_CapFault:
        પાછા true;

    સ્થિતિ seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        પાછા (label == 0);

    સ્થિતિ seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        પાછા (label == 0);

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        પાછા (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    સ્થિતિ seL4_Fault_DebugException: {
        word_t n_instrs;

        જો (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            પાછા (label == 0);
        }

        જો (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } નહીંતર {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, સંદેશ_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        જો (res.type != seL4_NoError) {
            પાછા false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        પાછા (label == 0);
    }
#endif

    મૂળભૂત:
        પાછા Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    પસંદગી (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    સ્થિતિ seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        પાછા setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    સ્થિતિ seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        પાછા setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_સંખ્યા(sender->tcbFault));
    }

    સ્થિતિ seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_સંખ્યા(sender->tcbFault));
        પાછા setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ seL4_Fault_Timeout: {
        જો (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            પાછા mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } નહીંતર {
            પાછા setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    સ્થિતિ seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned પૂર્ણાંક ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        જો (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_સંખ્યા,
                        se_L4_Fault_Debug_Exception_get_breakpoint_સંખ્યા(sender->tcbFault));
        }
        પાછા ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    મૂળભૂત:
        પાછા Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
