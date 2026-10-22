/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/faults.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <arch/kernel/thread.h>
#അന്തര്ഭാവ <machine/debug.h>
#ifdef CONFIG_KERNEL_MCS
#അന്തര്ഭാവ <mode/api/ipc_buffer.h>
#അന്തര്ഭാവ <object/schedcontext.h>
#endif

/* consistency with libsel4 */
compile_assert(Invalid_വേര്, lookup_fault_invalid_വേര് + 1 == se_L4_Invalid_വേര്)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(DepthMismatch, lookup_fault_depth_mismatch + 1 == seL4_DepthMismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscallMessage == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_സംഖ്യ, (word_t) n_exceptionMessage == se_L4_User_Exception_സംഖ്യ)
compile_assert(seL4_UserException_Code, (word_t) n_exceptionMessage + 1 == seL4_UserException_Code)

സ്ഥിതം വരിയിൽ unsigned പൂർണ്ണസംഖ്യ
setMRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, unsigned പൂർണ്ണസംഖ്യ offset)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = setMR(receiver, receiveIPCBuffer, offset, lufType + 1);

    /* check constants match libsel4 */
    എങ്കിൽ (offset == seL4_CapFault_LookupFailureType) {
        പ്രതിജ്ഞാ(offset + 1 == se_L4_Cap_Fault_Bits_ഇടത്);
        പ്രതിജ്ഞാ(offset + 2 == seL4_CapFault_DepthMismatch_BitsFound);
        പ്രതിജ്ഞാ(offset + 2 == seL4_CapFault_GuardMismatch_GuardFound);
        പ്രതിജ്ഞാ(offset + 3 == seL4_CapFault_GuardMismatch_BitsFound);
    } അല്ലെങ്കിൽ {
        പ്രതിജ്ഞാ(offset == 1);
    }

    തിരഞ്ഞെടുപ്പ് (lufType) {
    അവസ്ഥ lookup_fault_invalid_വേര്:
        തിരികെ i;

    അവസ്ഥ lookup_fault_missing_capability:
        തിരികെ setMR(receiver, receiveIPCBuffer, offset + 1,
                     lookup_fault_missing_capability_get_bits_ഇടത്(luf));

    അവസ്ഥ lookup_fault_depth_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_depth_mismatch_get_bits_ഇടത്(luf));
        തിരികെ setMR(receiver, receiveIPCBuffer, offset + 2,
                     lookup_fault_depth_mismatch_get_bitsFound(luf));

    അവസ്ഥ lookup_fault_guard_mismatch:
        setMR(receiver, receiveIPCBuffer, offset + 1,
              lookup_fault_guard_mismatch_get_bits_ഇടത്(luf));
        setMR(receiver, receiveIPCBuffer, offset + 2,
              lookup_fault_guard_mismatch_get_guardFound(luf));
        തിരികെ setMR(receiver, receiveIPCBuffer, offset + 3,
                     lookup_fault_guard_mismatch_get_bitsFound(luf));

    സ്ഥിരസ്ഥിതി:
        fail("Invalid lookup failure");
    }
}

സ്ഥിതം വരിയിൽ ശൂന്യം copyMRsFaultReply(tcb_t *sender, tcb_t *receiver, MessageID_t id, word_t length)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_getSanitiseRegisterInfo(receiver);

    വേണ്ടി (i = 0; i < MIN(length, n_സന്ദേശം_Registers); i++) {
        register_t r = fault_messages[id][i];
        word_t v = getRegister(sender, സന്ദേശം_Registers[i]);
        setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
    }

    എങ്കിൽ (i < length) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        എങ്കിൽ (sendBuf) {
            വേണ്ടി (; i < length; i++) {
                register_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                setRegister(receiver, r, sanitiseRegister(r, v, archInfo));
            }
        }
    }
}

സ്ഥിതം വരിയിൽ ശൂന്യം copyMRsFault(tcb_t *sender, tcb_t *receiver, MessageID_t id,
                                word_t length, word_t *receiveIPCBuffer)
{
    word_t i;
    വേണ്ടി (i = 0; i < MIN(length, n_സന്ദേശം_Registers); i++) {
        setRegister(receiver, സന്ദേശം_Registers[i], getRegister(sender, fault_messages[id][i]));
    }

    എങ്കിൽ (receiveIPCBuffer) {
        വേണ്ടി (; i < length; i++) {
            receiveIPCBuffer[i + 1] = getRegister(sender, fault_messages[id][i]);
        }
    }
}

bool_t handleFaultReply(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    seL4_MessageInfo_t tag = messageInfoFromWord(getRegister(sender, സന്ദേശം_Info_Register));
    word_t label = seL4_MessageInfo_get_label(tag);
    word_t length = seL4_MessageInfo_get_length(tag);
    seL4_Fault_t fault = receiver->tcbFault;

    തിരഞ്ഞെടുപ്പ് (seL4_Fault_get_seL4_FaultType(fault)) {
    അവസ്ഥ seL4_Fault_CapFault:
        തിരികെ true;

    അവസ്ഥ seL4_Fault_UnknownSyscall:
        copyMRsFaultReply(sender, receiver, MessageID_Syscall, MIN(length, n_syscallMessage));
        തിരികെ (label == 0);

    അവസ്ഥ seL4_Fault_UserException:
        copyMRsFaultReply(sender, receiver, MessageID_Exception, MIN(length, n_exceptionMessage));
        തിരികെ (label == 0);

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ seL4_Fault_Timeout:
        copyMRsFaultReply(sender, receiver, MessageID_TimeoutReply, MIN(length, n_timeoutMessage));
        തിരികെ (label == 0);
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    അവസ്ഥ seL4_Fault_DebugException: {
        word_t n_instrs;

        എങ്കിൽ (seL4_Fault_DebugException_get_exceptionReason(fault) != seL4_SingleStep) {
            /* Only single-step replies are required to set message registers.
             */
            തിരികെ (label == 0);
        }

        എങ്കിൽ (length < DEBUG_REPLY_N_EXPECTED_REGISTERS) {
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
        } അല്ലെങ്കിൽ {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = getRegister(sender, സന്ദേശം_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        എങ്കിൽ (res.type != seL4_NoError) {
            തിരികെ false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        തിരികെ (label == 0);
    }
#endif

    സ്ഥിരസ്ഥിതി:
        തിരികെ Arch_handleFaultReply(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    തിരഞ്ഞെടുപ്പ് (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    അവസ്ഥ seL4_Fault_CapFault:
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              seL4_Fault_CapFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_CapFault_InRecvPhase,
              seL4_Fault_CapFault_get_inReceivePhase(sender->tcbFault));
        തിരികെ setMRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    അവസ്ഥ seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, MessageID_Syscall, n_syscallMessage,
                     receiveIPCBuffer);

        തിരികെ setMR(receiver, receiveIPCBuffer, n_syscallMessage,
                     se_L4_Fault_Unknown_Syscall_get_syscall_സംഖ്യ(sender->tcbFault));
    }

    അവസ്ഥ seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, MessageID_Exception,
                     n_exceptionMessage, receiveIPCBuffer);
        setMR(receiver, receiveIPCBuffer, n_exceptionMessage,
              se_L4_Fault_User_Exception_get_സംഖ്യ(sender->tcbFault));
        തിരികെ setMR(receiver, receiveIPCBuffer, n_exceptionMessage + 1u,
                     seL4_Fault_UserException_get_code(sender->tcbFault));
    }

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ seL4_Fault_Timeout: {
        എങ്കിൽ (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            തിരികെ mode_setTimeArg(len, consumed, receiveIPCBuffer, receiver);
        } അല്ലെങ്കിൽ {
            തിരികെ setMR(receiver, receiveIPCBuffer, seL4_Timeout_Data,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#endif
#ifdef CONFIG_HARDWARE_DEBUG_API
    അവസ്ഥ seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        setMR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        unsigned പൂർണ്ണസംഖ്യ ret = setMR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        എങ്കിൽ (reason != seL4_SingleStep && reason != seL4_SoftwareBreakRequest) {
            ret = setMR(receiver, receiveIPCBuffer,
                        seL4_DebugException_TriggerAddress,
                        seL4_Fault_DebugException_get_breakpointAddress(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = setMR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_സംഖ്യ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_സംഖ്യ(sender->tcbFault));
        }
        തിരികെ ret;
    }
#endif /* CONFIG_HARDWARE_DEBUG_API */

    സ്ഥിരസ്ഥിതി:
        തിരികെ Arch_setMRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
