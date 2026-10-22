/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/faults.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <arch/kernel/thread.h>
#অন্তর্ভাব <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#অন্তর্ভাব <mode/api/ipc_buffer.h>
#অন্তর্ভাব <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_মূল, lookup_fault_invalid_মূল + 1 == se_L4_Invalid_মূল)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_সংখ্যা, (word_t) n_exceptionMessage == se_L4_User_Exception_সংখ্যা)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

স্থির অন্তঃরেখ unsigned পূর্ণসংখ্যা
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned পূর্ণসংখ্যা offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    যদি (offset == seL4_CapFault_LookupFailureType) {
        প্রতিজ্ঞা(offset + 1 == se_L4_Cap_Fault_Bits_বাম);
        প্রতিজ্ঞা(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        প্রতিজ্ঞা(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        প্রতিজ্ঞা(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } অন্যথা {
        প্রতিজ্ঞা(offset == 1);
    }

    নির্বাচন (lufType) {
    ক্ষেত্র lookup_fault_invalid_মূল:
        ফেরত i;

    ক্ষেত্র lookup_fault_missing_capability:
        ফেরত setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_বাম(luf));

    ক্ষেত্র lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_বাম(luf));
        ফেরত setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    ক্ষেত্র lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_বাম(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        ফেরত setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    পূর্বনির্ধারিত:
        fail("Invalid lookup failure");
    }
}

স্থির অন্তঃরেখ শূন্য copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t দৈর্ঘ্য)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    জন্য (i = 0; i < MIN(দৈর্ঘ্য, n_বার্তা_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, বার্তা_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    যদি (i < দৈর্ঘ্য) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        যদি (sendBuf) {
            জন্য (; i < দৈর্ঘ্য; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

স্থির অন্তঃরেখ শূন্য copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t দৈর্ঘ্য, word_t *receiveIPCBuffer)
{
    word_t i;
    জন্য (i = 0; i < MIN(দৈর্ঘ্য, n_বার্তা_Registers); i++) {
        setRegister(receiver, বার্তা_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    যদি (receiveIPCBuffer) {
        জন্য (; i < দৈর্ঘ্য; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, বার্তা_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t দৈর্ঘ্য = se_L4_Message_Info_get_দৈর্ঘ্য(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    নির্বাচন (seL4_Fault_get_seL4_FaultType(fault)) {
    ক্ষেত্র seL4_Fault_CapFault:
        ফেরত true;

    ক্ষেত্র seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(দৈর্ঘ্য, n_syscallMessage));
        ফেরত (label == 0);

    ক্ষেত্র seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(দৈর্ঘ্য, n_exceptionMessage));
        ফেরত (label == 0);

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(দৈর্ঘ্য, n_timeoutMessage));
        ফেরত (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    ক্ষেত্র seL4_Fault_DebugException: {
        word_t n_instrs;

        যদি (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            ফেরত (label == 0);
        }

        যদি (দৈর্ঘ্য < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } অন্যথা {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, বার্তা_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        যদি (res.type != seL4_NoError) {
            ফেরত false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        ফেরত (label == 0);
    }
#endif

    পূর্বনির্ধারিত:
        ফেরত Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    নির্বাচন (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    ক্ষেত্র seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        ফেরত setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    ক্ষেত্র seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        ফেরত setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_সংখ্যা(sender->tcbFault));
    }

    ক্ষেত্র seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_সংখ্যা(sender->tcbFault));
        ফেরত setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র seL4_Fault_Timeout: {
        যদি (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, se_L4_Timeout_তথ্য,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            ফেরত mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } অন্যথা {
            ফেরত setMR(receiver, receiveIPCBuffer, se_L4_Timeout_তথ্য,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    ক্ষেত্র seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned পূর্ণসংখ্যা ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        যদি (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_সংখ্যা,
                        se_L4_Fault_Debug_Exception_get_breakpoint_সংখ্যা(sender->tcbFault));
        }
        ফেরত ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    পূর্বনির্ধারিত:
        ফেরত Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
