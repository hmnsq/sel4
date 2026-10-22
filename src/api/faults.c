/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/faults.h>
#شامل <api/syscall.h>
#شامل <kernel/thread.h>
#شامل <arch/kernel/thread.h>
#شامل <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#شامل <mode/api/ipc_buffer.h>
#شامل <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_ریشه, lookup_fault_invalid_ریشه + 1 == se_L4_Invalid_ریشه)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_عدد, (word_t) n_exceptionMessage == se_L4_User_Exception_عدد)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

ایستا درونخطی unsigned صحیح
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned صحیح offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    اگر (offset == seL4_CapFault_LookupFailureType) {
        أكد(offset + 1 == se_L4_Cap_Fault_Bits_چپ);
        أكد(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        أكد(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        أكد(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } وگرنه {
        أكد(offset == 1);
    }

    انتخاب (lufType) {
    حالت lookup_fault_invalid_ریشه:
        بازگشت i;

    حالت lookup_fault_missing_capability:
        بازگشت setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_چپ(luf));

    حالت lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_چپ(luf));
        بازگشت setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    حالت lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_چپ(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        بازگشت setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    پیشفرض:
        fail("Invalid lookup failure");
    }
}

ایستا درونخطی تهی copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    برای (i = 0; i < MIN(length, n_پیام_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, پیام_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    اگر (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        اگر (sendBuf) {
            برای (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

ایستا درونخطی تهی copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    برای (i = 0; i < MIN(length, n_پیام_Registers); i++) {
        setRegister(receiver, پیام_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    اگر (receiveIPCBuffer) {
        برای (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, پیام_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    انتخاب (seL4_Fault_get_seL4_FaultType(fault)) {
    حالت seL4_Fault_CapFault:
        بازگشت true;

    حالت seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        بازگشت (label == 0);

    حالت seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        بازگشت (label == 0);

#ifdef CONFIG_KERNEL_MCS
    حالت seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        بازگشت (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    حالت seL4_Fault_DebugException: {
        word_t n_instrs;

        اگر (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            بازگشت (label == 0);
        }

        اگر (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } وگرنه {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, پیام_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        اگر (res.type != seL4_NoError) {
            بازگشت false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        بازگشت (label == 0);
    }
#endif

    پیشفرض:
        بازگشت Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    انتخاب (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالت seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        بازگشت setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالت seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        بازگشت setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_عدد(sender->tcbFault));
    }

    حالت seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_عدد(sender->tcbFault));
        بازگشت setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    حالت seL4_Fault_Timeout: {
        اگر (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            بازگشت mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } وگرنه {
            بازگشت setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    حالت seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned صحیح ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        اگر (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_عدد,
                        se_L4_Fault_Debug_Exception_get_breakpoint_عدد(sender->tcbFault));
        }
        بازگشت ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    پیشفرض:
        بازگشت Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
