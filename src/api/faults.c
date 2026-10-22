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
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#अन्तर्भाव <mode/api/ipc_buffer.h>
#अन्तर्भाव <object/schedcontext.h>
#यदि_अन्तः

/* consistency with libsel4 */
compile_assert(Invalid_मूलम्, lookup_fault_invalid_मूलम् + 1 == se_L4_Invalid_मूलम्)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(गभीरता_Mismatch, lookup_fault_गभीरता_mismatch + 1 == se_L4_गभीरता_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (word_t) n_syscall_सन्देशः == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_सङ्ख्या, (word_t) n_exception_सन्देशः == se_L4_User_Exception_सङ्ख्या)
compile_assert(se_L4_User_Exception_संहिता, (word_t) n_exception_सन्देशः + 1 == se_L4_User_Exception_संहिता)

स्थिर अन्तःस्थ अचिह्नित पूर्णाङ्क
समुच्चयः_MRs_lookup_failure(tcb_t *receiver, word_t *receiveIPCBuffer,
                      lookup_fault_t luf, अचिह्नित पूर्णाङ्क विस्थापनम्)
{
    word_t lufType = lookup_fault_get_lufType(luf);
    word_t i;

    i = समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम्, lufType + 1);

    /* check constants match libsel4 */
    यदि (विस्थापनम् == seL4_CapFault_LookupFailureType) {
        प्रतिज्ञा(विस्थापनम् + 1 == se_L4_Cap_Fault_द्वयङ्काः_वामः);
        प्रतिज्ञा(विस्थापनम् + 2 == se_L4_Cap_Fault_गभीरता_Mismatch_द्वयङ्काः_प्राप्तम्);
        प्रतिज्ञा(विस्थापनम् + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_प्राप्तम्);
        प्रतिज्ञा(विस्थापनम् + 3 == se_L4_Cap_Fault_Guard_Mismatch_द्वयङ्काः_प्राप्तम्);
    } अन्यथा {
        प्रतिज्ञा(विस्थापनम् == 1);
    }

    विकल्प (lufType) {
    पक्ष lookup_fault_invalid_मूलम्:
        प्रत्यागम i;

    पक्ष lookup_fault_missing_capability:
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम् + 1,
                     lookup_fault_missing_capability_get_द्वयङ्काः_वामः(luf));

    पक्ष lookup_fault_गभीरता_mismatch:
        समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम् + 1,
              lookup_fault_गभीरता_mismatch_get_द्वयङ्काः_वामः(luf));
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम् + 2,
                     lookup_fault_गभीरता_mismatch_get_द्वयङ्काः_प्राप्तम्(luf));

    पक्ष lookup_fault_guard_mismatch:
        समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम् + 1,
              lookup_fault_guard_mismatch_get_द्वयङ्काः_वामः(luf));
        समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम् + 2,
              lookup_fault_guard_mismatch_get_guard_प्राप्तम्(luf));
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, विस्थापनम् + 3,
                     lookup_fault_guard_mismatch_get_द्वयङ्काः_प्राप्तम्(luf));

    स्वभाव:
        fail("Invalid lookup failure");
    }
}

स्थिर अन्तःस्थ शून्य copy_MRs_Fault_प्रत्युत्तरम्(tcb_t *sender, tcb_t *receiver, सन्देशः_ID_t id, word_t दीर्घता)
{
    word_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_पञ्जिकाकोशः_Info(receiver);

    प्रति (i = 0; i < MIN(दीर्घता, n_सन्देशः_Registers); i++) {
        पञ्जिकाकोशः_t r = fault_messages[id][i];
        word_t v = get_पञ्जिकाकोशः(sender, सन्देशः_Registers[i]);
        समुच्चयः_पञ्जिकाकोशः(receiver, r, sanitise_पञ्जिकाकोशः(r, v, archInfo));
    }

    यदि (i < दीर्घता) {
        word_t *sendBuf = lookupIPCBuffer(false, sender);
        यदि (sendBuf) {
            प्रति (; i < दीर्घता; i++) {
                पञ्जिकाकोशः_t r = fault_messages[id][i];
                word_t v = sendBuf[i + 1];
                समुच्चयः_पञ्जिकाकोशः(receiver, r, sanitise_पञ्जिकाकोशः(r, v, archInfo));
            }
        }
    }
}

स्थिर अन्तःस्थ शून्य copyMRsFault(tcb_t *sender, tcb_t *receiver, सन्देशः_ID_t id,
                                word_t दीर्घता, word_t *receiveIPCBuffer)
{
    word_t i;
    प्रति (i = 0; i < MIN(दीर्घता, n_सन्देशः_Registers); i++) {
        समुच्चयः_पञ्जिकाकोशः(receiver, सन्देशः_Registers[i], get_पञ्जिकाकोशः(sender, fault_messages[id][i]));
    }

    यदि (receiveIPCBuffer) {
        प्रति (; i < दीर्घता; i++) {
            receiveIPCBuffer[i + 1] = get_पञ्जिकाकोशः(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_प्रत्युत्तरम्(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_सन्देशः_Info_t चिह्नकम् = सन्देशः_Info_From_Word(get_पञ्जिकाकोशः(sender, सन्देशः_Info_पञ्जिकाकोशः));
    word_t label = se_L4_सन्देशः_Info_get_label(चिह्नकम्);
    word_t दीर्घता = se_L4_सन्देशः_Info_get_दीर्घता(चिह्नकम्);
    seL4_Fault_t fault = receiver->tcbFault;

    विकल्प (seL4_Fault_get_seL4_FaultType(fault)) {
    पक्ष seL4_Fault_CapFault:
        प्रत्यागम true;

    पक्ष seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_प्रत्युत्तरम्(sender, receiver, सन्देशः_ID_Syscall, MIN(दीर्घता, n_syscall_सन्देशः));
        प्रत्यागम (label == 0);

    पक्ष seL4_Fault_UserException:
        copy_MRs_Fault_प्रत्युत्तरम्(sender, receiver, सन्देशः_ID_Exception, MIN(दीर्घता, n_exception_सन्देशः));
        प्रत्यागम (label == 0);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष seL4_Fault_Timeout:
        copy_MRs_Fault_प्रत्युत्तरम्(sender, receiver, सन्देशः_ID_Timeout_प्रत्युत्तरम्, MIN(दीर्घता, n_timeout_सन्देशः));
        प्रत्यागम (label == 0);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    पक्ष seL4_Fault_DebugException: {
        word_t n_instrs;

        यदि (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_चरणः) {
            /* Only single-step replies are required to set message registers.
             */
            प्रत्यागम (label == 0);
        }

        यदि (दीर्घता < DEBUG_प्रत्युत्तरम्_N_EXPECTED_REGISTERS) {
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
            n_instrs = get_पञ्जिकाकोशः(sender, सन्देशः_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        यदि (res.type != seL4_NoError) {
            प्रत्यागम false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        प्रत्यागम (label == 0);
    }
#यदि_अन्तः

    स्वभाव:
        प्रत्यागम Arch_handle_Fault_प्रत्युत्तरम्(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

word_t समुच्चयः_MRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer)
{
    विकल्प (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    पक्ष seL4_Fault_CapFault:
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_स्थानाङ्कः(sender->tcbFault));
        समुच्चयः_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_अन्तर्_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_अन्तर्_Receive_Phase(sender->tcbFault));
        प्रत्यागम समुच्चयः_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    पक्ष seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, सन्देशः_ID_Syscall, n_syscall_सन्देशः,
                     receiveIPCBuffer);

        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, n_syscall_सन्देशः,
                     se_L4_Fault_Unknown_Syscall_get_syscall_सङ्ख्या(sender->tcbFault));
    }

    पक्ष seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, सन्देशः_ID_Exception,
                     n_exception_सन्देशः, receiveIPCBuffer);
        समुच्चयः_MR(receiver, receiveIPCBuffer, n_exception_सन्देशः,
              se_L4_Fault_User_Exception_get_सङ्ख्या(sender->tcbFault));
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, n_exception_सन्देशः + 1u,
                     se_L4_Fault_User_Exception_get_संहिता(sender->tcbFault));
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष seL4_Fault_Timeout: {
        यदि (sender->tcbSchedContext) {
            word_t len;
            time_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = समुच्चयः_MR(receiver, receiveIPCBuffer, se_L4_Timeout_दत्त,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            प्रत्यागम बहुलकम्_समुच्चयः_Time_Arg(len, consumed, receiveIPCBuffer, receiver);
        } अन्यथा {
            प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, se_L4_Timeout_दत्त,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    पक्ष seL4_Fault_DebugException: {
        word_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        समुच्चयः_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        अचिह्नित पूर्णाङ्क ret = समुच्चयः_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        यदि (reason != se_L4_Single_चरणः && reason != seL4_SoftwareBreakRequest) {
            ret = समुच्चयः_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_स्थानाङ्कः,
                        se_L4_Fault_Debug_Exception_get_breakpoint_स्थानाङ्कः(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = समुच्चयः_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_सङ्ख्या,
                        se_L4_Fault_Debug_Exception_get_breakpoint_सङ्ख्या(sender->tcbFault));
        }
        प्रत्यागम ret;
    }
#यदि_अन्तः /* CONFIG_HARDWARE_DEBUG_API */

    स्वभाव:
        प्रत्यागम Arch_समुच्चयः_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
