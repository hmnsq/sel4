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
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#شامل <mode/api/ipc_buffer.h>
#شامل <object/schedcontext.h>
#اگر_ختم

/* consistency with libsel4 */
compile_assert(Invalid_جڑ, lookup_fault_invalid_جڑ + 1 == se_L4_Invalid_جڑ)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(گہرائی_Mismatch, lookup_fault_گہرائی_mismatch + 1 == se_L4_گہرائی_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (لفظ_t) n_syscall_پیغام == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_نمبر, (لفظ_t) n_exception_پیغام == se_L4_User_Exception_نمبر)
compile_assert(se_L4_User_Exception_رمز, (لفظ_t) n_exception_پیغام + 1 == se_L4_User_Exception_رمز)

ساکن اندرونی غیرعلامتی صحیح
سیٹ_MRs_lookup_failure(tcb_t *receiver, لفظ_t *receiveIPCBuffer,
                      lookup_fault_t luf, غیرعلامتی صحیح انحراف)
{
    لفظ_t lufType = lookup_fault_get_lufType(luf);
    لفظ_t i;

    i = سیٹ_MR(receiver, receiveIPCBuffer, انحراف, lufType + 1);

    /* check constants match libsel4 */
    اگر (انحراف == seL4_CapFault_LookupFailureType) {
        أكد(انحراف + 1 == se_L4_Cap_Fault_بٹ_بایاں);
        أكد(انحراف + 2 == se_L4_Cap_Fault_گہرائی_Mismatch_بٹ_ملا);
        أكد(انحراف + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_ملا);
        أكد(انحراف + 3 == se_L4_Cap_Fault_Guard_Mismatch_بٹ_ملا);
    } ورنہ {
        أكد(انحراف == 1);
    }

    انتخاب (lufType) {
    حالت lookup_fault_invalid_جڑ:
        واپس i;

    حالت lookup_fault_missing_capability:
        واپس سیٹ_MR(receiver, receiveIPCBuffer, انحراف + 1,
                     lookup_fault_missing_capability_get_بٹ_بایاں(luf));

    حالت lookup_fault_گہرائی_mismatch:
        سیٹ_MR(receiver, receiveIPCBuffer, انحراف + 1,
              lookup_fault_گہرائی_mismatch_get_بٹ_بایاں(luf));
        واپس سیٹ_MR(receiver, receiveIPCBuffer, انحراف + 2,
                     lookup_fault_گہرائی_mismatch_get_بٹ_ملا(luf));

    حالت lookup_fault_guard_mismatch:
        سیٹ_MR(receiver, receiveIPCBuffer, انحراف + 1,
              lookup_fault_guard_mismatch_get_بٹ_بایاں(luf));
        سیٹ_MR(receiver, receiveIPCBuffer, انحراف + 2,
              lookup_fault_guard_mismatch_get_guard_ملا(luf));
        واپس سیٹ_MR(receiver, receiveIPCBuffer, انحراف + 3,
                     lookup_fault_guard_mismatch_get_بٹ_ملا(luf));

    طےشدہ:
        fail("Invalid lookup failure");
    }
}

ساکن اندرونی خالی copy_MRs_Fault_جواب(tcb_t *sender, tcb_t *receiver, پیغام_ID_t id, لفظ_t لمبائی)
{
    لفظ_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_رجسٹر_Info(receiver);

    برائے (i = 0; i < MIN(لمبائی, n_پیغام_Registers); i++) {
        رجسٹر_t r = fault_messages[id][i];
        لفظ_t v = get_رجسٹر(sender, پیغام_Registers[i]);
        سیٹ_رجسٹر(receiver, r, sanitise_رجسٹر(r, v, archInfo));
    }

    اگر (i < لمبائی) {
        لفظ_t *sendBuf = lookupIPCBuffer(false, sender);
        اگر (sendBuf) {
            برائے (; i < لمبائی; i++) {
                رجسٹر_t r = fault_messages[id][i];
                لفظ_t v = sendBuf[i + 1];
                سیٹ_رجسٹر(receiver, r, sanitise_رجسٹر(r, v, archInfo));
            }
        }
    }
}

ساکن اندرونی خالی copyMRsFault(tcb_t *sender, tcb_t *receiver, پیغام_ID_t id,
                                لفظ_t لمبائی, لفظ_t *receiveIPCBuffer)
{
    لفظ_t i;
    برائے (i = 0; i < MIN(لمبائی, n_پیغام_Registers); i++) {
        سیٹ_رجسٹر(receiver, پیغام_Registers[i], get_رجسٹر(sender, fault_messages[id][i]));
    }

    اگر (receiveIPCBuffer) {
        برائے (; i < لمبائی; i++) {
            receiveIPCBuffer[i + 1] = get_رجسٹر(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_جواب(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_پیغام_Info_t لیبل = پیغام_Info_From_لفظ(get_رجسٹر(sender, پیغام_Info_رجسٹر));
    لفظ_t label = se_L4_پیغام_Info_get_label(لیبل);
    لفظ_t لمبائی = se_L4_پیغام_Info_get_لمبائی(لیبل);
    seL4_Fault_t fault = receiver->tcbFault;

    انتخاب (seL4_Fault_get_seL4_FaultType(fault)) {
    حالت seL4_Fault_CapFault:
        واپس true;

    حالت seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_جواب(sender, receiver, پیغام_ID_Syscall, MIN(لمبائی, n_syscall_پیغام));
        واپس (label == 0);

    حالت seL4_Fault_UserException:
        copy_MRs_Fault_جواب(sender, receiver, پیغام_ID_Exception, MIN(لمبائی, n_exception_پیغام));
        واپس (label == 0);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت seL4_Fault_Timeout:
        copy_MRs_Fault_جواب(sender, receiver, پیغام_ID_Timeout_جواب, MIN(لمبائی, n_timeout_پیغام));
        واپس (label == 0);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    حالت seL4_Fault_DebugException: {
        لفظ_t n_instrs;

        اگر (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_قدم) {
            /* Only single-step replies are required to set message registers.
             */
            واپس (label == 0);
        }

        اگر (لمبائی < DEBUG_جواب_N_EXPECTED_REGISTERS) {
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
        } ورنہ {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = get_رجسٹر(sender, پیغام_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        اگر (res.type != seL4_NoError) {
            واپس false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        واپس (label == 0);
    }
#اگر_ختم

    طےشدہ:
        واپس Arch_handle_Fault_جواب(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

لفظ_t سیٹ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer)
{
    انتخاب (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالت seL4_Fault_CapFault:
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_پتہ(sender->tcbFault));
        سیٹ_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_اندر_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_اندر_Receive_Phase(sender->tcbFault));
        واپس سیٹ_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالت seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, پیغام_ID_Syscall, n_syscall_پیغام,
                     receiveIPCBuffer);

        واپس سیٹ_MR(receiver, receiveIPCBuffer, n_syscall_پیغام,
                     se_L4_Fault_Unknown_Syscall_get_syscall_نمبر(sender->tcbFault));
    }

    حالت seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, پیغام_ID_Exception,
                     n_exception_پیغام, receiveIPCBuffer);
        سیٹ_MR(receiver, receiveIPCBuffer, n_exception_پیغام,
              se_L4_Fault_User_Exception_get_نمبر(sender->tcbFault));
        واپس سیٹ_MR(receiver, receiveIPCBuffer, n_exception_پیغام + 1u,
                     se_L4_Fault_User_Exception_get_رمز(sender->tcbFault));
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت seL4_Fault_Timeout: {
        اگر (sender->tcbSchedContext) {
            لفظ_t len;
            وقت_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = سیٹ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_ڈیٹا,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            واپس منوال_سیٹ_وقت_Arg(len, consumed, receiveIPCBuffer, receiver);
        } ورنہ {
            واپس سیٹ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_ڈیٹا,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    حالت seL4_Fault_DebugException: {
        لفظ_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        سیٹ_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        غیرعلامتی صحیح ret = سیٹ_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        اگر (reason != se_L4_Single_قدم && reason != seL4_SoftwareBreakRequest) {
            ret = سیٹ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_پتہ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_پتہ(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = سیٹ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_نمبر,
                        se_L4_Fault_Debug_Exception_get_breakpoint_نمبر(sender->tcbFault));
        }
        واپس ret;
    }
#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */

    طےشدہ:
        واپس Arch_سیٹ_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
