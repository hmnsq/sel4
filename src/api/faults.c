/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/faults.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#ଅନ୍ତର୍ଭାଵ <mode/api/ipc_buffer.h>
#ଅନ୍ତର୍ଭାଵ <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_ମୂଳ, lookup_fault_invalid_ମୂଳ + 1 == se_L4_Invalid_ମୂଳ)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_ସଂଖ୍ୟା, (word_t) n_exceptionMessage == se_L4_User_Exception_ସଂଖ୍ୟା)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    ଯଦି (offset == seL4_CapFault_LookupFailureType) {
        ପ୍ରତିଜ୍ଞା(offset + 1 == se_L4_Cap_Fault_Bits_ବାମ);
        ପ୍ରତିଜ୍ଞା(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        ପ୍ରତିଜ୍ଞା(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        ପ୍ରତିଜ୍ଞା(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } ଅନ୍ୟଥା {
        ପ୍ରତିଜ୍ଞା(offset == 1);
    }

    ବାଛ (lufType) {
    ସ୍ଥିତି lookup_fault_invalid_ମୂଳ:
        ଫେରନ୍ତୁ i;

    ସ୍ଥିତି lookup_fault_missing_capability:
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_ବାମ(luf));

    ସ୍ଥିତି lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_ବାମ(luf));
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    ସ୍ଥିତି lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_ବାମ(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid lookup failure");
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    ପାଇଁ (i = 0; i < MIN(length, n_ସନ୍ଦେଶ_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, ସନ୍ଦେଶ_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    ଯଦି (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        ଯଦି (sendBuf) {
            ପାଇଁ (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    ପାଇଁ (i = 0; i < MIN(length, n_ସନ୍ଦେଶ_Registers); i++) {
        setRegister(receiver, ସନ୍ଦେଶ_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    ଯଦି (receiveIPCBuffer) {
        ପାଇଁ (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, ସନ୍ଦେଶ_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    ବାଛ (seL4_Fault_get_seL4_FaultType(fault)) {
    ସ୍ଥିତି seL4_Fault_CapFault:
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        ଫେରନ୍ତୁ (label == 0);

    ସ୍ଥିତି seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        ଫେରନ୍ତୁ (label == 0);

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        ଫେରନ୍ତୁ (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    ସ୍ଥିତି seL4_Fault_DebugException: {
        word_t n_instrs;

        ଯଦି (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            ଫେରନ୍ତୁ (label == 0);
        }

        ଯଦି (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } ଅନ୍ୟଥା {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, ସନ୍ଦେଶ_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        ଯଦି (res.type != seL4_NoError) {
            ଫେରନ୍ତୁ false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        ଫେରନ୍ତୁ (label == 0);
    }
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    ବାଛ (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    ସ୍ଥିତି seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        ଫେରନ୍ତୁ setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    ସ୍ଥିତି seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_ସଂଖ୍ୟା(sender->tcbFault));
    }

    ସ୍ଥିତି seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_ସଂଖ୍ୟା(sender->tcbFault));
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି seL4_Fault_Timeout: {
        ଯଦି (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            ଫେରନ୍ତୁ mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    ସ୍ଥିତି seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        ଯଦି (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_ସଂଖ୍ୟା,
                        se_L4_Fault_Debug_Exception_get_breakpoint_ସଂଖ୍ୟା(sender->tcbFault));
        }
        ଫେରନ୍ତୁ ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
