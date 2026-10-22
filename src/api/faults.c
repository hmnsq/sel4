/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/faults.h>
#ضمن <api/syscall.h>
#ضمن <kernel/thread.h>
#ضمن <arch/kernel/thread.h>
#ضمن <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#ضمن <mode/api/ipc_buffer.h>
#ضمن <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_جذر, lookup_fault_invalid_جذر + 1 == se_L4_Invalid_جذر)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_رقم, (word_t) n_exceptionMessage == se_L4_User_Exception_رقم)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

ساكن مضمن unsigned صحيح
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned صحيح offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    إذا (offset == seL4_CapFault_LookupFailureType) {
        أكد(offset + 1 == se_L4_Cap_Fault_Bits_يسار);
        أكد(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        أكد(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        أكد(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } وإلا {
        أكد(offset == 1);
    }

    بدل (lufType) {
    حالة lookup_fault_invalid_جذر:
        أرجع i;

    حالة lookup_fault_missing_capability:
        أرجع setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_يسار(luf));

    حالة lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_يسار(luf));
        أرجع setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    حالة lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_يسار(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        أرجع setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    افتراضي:
        fail("Invalid lookup failure");
    }
}

ساكن مضمن فراغ copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    لكل (i = 0; i < MIN(length, n_رسالة_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, رسالة_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    إذا (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        إذا (sendBuf) {
            لكل (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

ساكن مضمن فراغ copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    لكل (i = 0; i < MIN(length, n_رسالة_Registers); i++) {
        setRegister(receiver, رسالة_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    إذا (receiveIPCBuffer) {
        لكل (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, رسالة_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    بدل (seL4_Fault_get_seL4_FaultType(fault)) {
    حالة seL4_Fault_CapFault:
        أرجع true;

    حالة seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        أرجع (label == 0);

    حالة seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        أرجع (label == 0);

#ifdef CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        أرجع (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        word_t n_instrs;

        إذا (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            أرجع (label == 0);
        }

        إذا (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } وإلا {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, رسالة_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        إذا (res.type != seL4_NoError) {
            أرجع false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        أرجع (label == 0);
    }
#endif

    افتراضي:
        أرجع Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    بدل (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالة seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        أرجع setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالة seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        أرجع setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_رقم(sender->tcbFault));
    }

    حالة seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_رقم(sender->tcbFault));
        أرجع setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout: {
        إذا (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            أرجع mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } وإلا {
            أرجع setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned صحيح ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        إذا (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_رقم,
                        se_L4_Fault_Debug_Exception_get_breakpoint_رقم(sender->tcbFault));
        }
        أرجع ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    افتراضي:
        أرجع Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
