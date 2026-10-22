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
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#ضمن <mode/api/ipc_buffer.h>
#ضمن <object/schedcontext.h>
#جے_انت

/* consistency with libsel4 */
compile_assert(Invalid_جڑ, lookup_fault_invalid_جڑ + 1 == se_L4_Invalid_جڑ)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(ڈونگھائی_Mismatch, lookup_fault_ڈونگھائی_mismatch + 1 == se_L4_ڈونگھائی_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (لفظ_t) n_syscall_سنیہا == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_نمبر, (لفظ_t) n_exception_سنیہا == se_L4_User_Exception_نمبر)
compile_assert(se_L4_User_Exception_کوڈ, (لفظ_t) n_exception_سنیہا + 1 == se_L4_User_Exception_کوڈ)

ساكن مضمن بنانشانی صحيح
سیٹ_MRs_lookup_failure(tcb_t *receiver, لفظ_t *receiveIPCBuffer,
                      lookup_fault_t luf, بنانشانی صحيح وتھ)
{
    لفظ_t lufType = lookup_fault_get_lufType(luf);
    لفظ_t i;

    i = سیٹ_MR(receiver, receiveIPCBuffer, وتھ, lufType + 1);

    /* check constants match libsel4 */
    إذا (وتھ == seL4_CapFault_LookupFailureType) {
        أكد(وتھ + 1 == se_L4_Cap_Fault_بٹ_بایاں);
        أكد(وتھ + 2 == se_L4_Cap_Fault_ڈونگھائی_Mismatch_بٹ_لبھیا);
        أكد(وتھ + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_لبھیا);
        أكد(وتھ + 3 == se_L4_Cap_Fault_Guard_Mismatch_بٹ_لبھیا);
    } وإلا {
        أكد(وتھ == 1);
    }

    بدل (lufType) {
    حالة lookup_fault_invalid_جڑ:
        أرجع i;

    حالة lookup_fault_missing_capability:
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, وتھ + 1,
                     lookup_fault_missing_capability_get_بٹ_بایاں(luf));

    حالة lookup_fault_ڈونگھائی_mismatch:
        سیٹ_MR(receiver, receiveIPCBuffer, وتھ + 1,
              lookup_fault_ڈونگھائی_mismatch_get_بٹ_بایاں(luf));
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, وتھ + 2,
                     lookup_fault_ڈونگھائی_mismatch_get_بٹ_لبھیا(luf));

    حالة lookup_fault_guard_mismatch:
        سیٹ_MR(receiver, receiveIPCBuffer, وتھ + 1,
              lookup_fault_guard_mismatch_get_بٹ_بایاں(luf));
        سیٹ_MR(receiver, receiveIPCBuffer, وتھ + 2,
              lookup_fault_guard_mismatch_get_guard_لبھیا(luf));
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, وتھ + 3,
                     lookup_fault_guard_mismatch_get_بٹ_لبھیا(luf));

    افتراضي:
        fail("Invalid lookup failure");
    }
}

ساكن مضمن فراغ copy_MRs_Fault_جواب(tcb_t *sender, tcb_t *receiver, سنیہا_ID_t id, لفظ_t لمائی)
{
    لفظ_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_رجسٹر_Info(receiver);

    لكل (i = 0; i < MIN(لمائی, n_پیغام_Registers); i++) {
        رجسٹر_t r = fault_messages[id][i];
        لفظ_t v = get_رجسٹر(sender, پیغام_Registers[i]);
        سیٹ_رجسٹر(receiver, r, sanitise_رجسٹر(r, v, archInfo));
    }

    إذا (i < لمائی) {
        لفظ_t *sendBuf = lookupIPCBuffer(false, sender);
        إذا (sendBuf) {
            لكل (; i < لمائی; i++) {
                رجسٹر_t r = fault_messages[id][i];
                لفظ_t v = sendBuf[i + 1];
                سیٹ_رجسٹر(receiver, r, sanitise_رجسٹر(r, v, archInfo));
            }
        }
    }
}

ساكن مضمن فراغ copyMRsFault(tcb_t *sender, tcb_t *receiver, سنیہا_ID_t id,
                                لفظ_t لمائی, لفظ_t *receiveIPCBuffer)
{
    لفظ_t i;
    لكل (i = 0; i < MIN(لمائی, n_پیغام_Registers); i++) {
        سیٹ_رجسٹر(receiver, پیغام_Registers[i], get_رجسٹر(sender, fault_messages[id][i]));
    }

    إذا (receiveIPCBuffer) {
        لكل (; i < لمائی; i++) {
            receiveIPCBuffer[i + 1] = get_رجسٹر(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_جواب(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_سنیہا_Info_t ٹیگ = سنیہا_Info_From_لفظ(get_رجسٹر(sender, پیغام_Info_رجسٹر));
    لفظ_t label = se_L4_سنیہا_Info_get_label(ٹیگ);
    لفظ_t لمائی = se_L4_سنیہا_Info_get_لمائی(ٹیگ);
    seL4_Fault_t fault = receiver->tcbFault;

    بدل (seL4_Fault_get_seL4_FaultType(fault)) {
    حالة seL4_Fault_CapFault:
        أرجع true;

    حالة seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_جواب(sender, receiver, سنیہا_ID_Syscall, MIN(لمائی, n_syscall_سنیہا));
        أرجع (label == 0);

    حالة seL4_Fault_UserException:
        copy_MRs_Fault_جواب(sender, receiver, سنیہا_ID_Exception, MIN(لمائی, n_exception_سنیہا));
        أرجع (label == 0);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        copy_MRs_Fault_جواب(sender, receiver, سنیہا_ID_Timeout_جواب, MIN(لمائی, n_timeout_سنیہا));
        أرجع (label == 0);
#جے_انت
#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        لفظ_t n_instrs;

        إذا (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_قدم) {
            /* Only single-step replies are required to set message registers.
             */
            أرجع (label == 0);
        }

        إذا (لمائی < DEBUG_جواب_N_EXPECTED_REGISTERS) {
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
            n_instrs = get_رجسٹر(sender, پیغام_Registers[0]);
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
#جے_انت

    افتراضي:
        أرجع Arch_handle_Fault_جواب(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

لفظ_t سیٹ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer)
{
    بدل (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالة seL4_Fault_CapFault:
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_پتہ(sender->tcbFault));
        سیٹ_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_وچ_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_وچ_Receive_Phase(sender->tcbFault));
        أرجع سیٹ_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالة seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, سنیہا_ID_Syscall, n_syscall_سنیہا,
                     receiveIPCBuffer);

        أرجع سیٹ_MR(receiver, receiveIPCBuffer, n_syscall_سنیہا,
                     se_L4_Fault_Unknown_Syscall_get_syscall_نمبر(sender->tcbFault));
    }

    حالة seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, سنیہا_ID_Exception,
                     n_exception_سنیہا, receiveIPCBuffer);
        سیٹ_MR(receiver, receiveIPCBuffer, n_exception_سنیہا,
              se_L4_Fault_User_Exception_get_نمبر(sender->tcbFault));
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, n_exception_سنیہا + 1u,
                     se_L4_Fault_User_Exception_get_کوڈ(sender->tcbFault));
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout: {
        إذا (sender->tcbSchedContext) {
            لفظ_t len;
            ویلا_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = سیٹ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_انکڑے,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            أرجع منوال_سیٹ_ویلا_Arg(len, consumed, receiveIPCBuffer, receiver);
        } وإلا {
            أرجع سیٹ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_انکڑے,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#جے_انت
#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        لفظ_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        سیٹ_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        بنانشانی صحيح ret = سیٹ_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        إذا (reason != se_L4_Single_قدم && reason != seL4_SoftwareBreakRequest) {
            ret = سیٹ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_پتہ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_پتہ(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = سیٹ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_نمبر,
                        se_L4_Fault_Debug_Exception_get_breakpoint_نمبر(sender->tcbFault));
        }
        أرجع ret;
    }
#جے_انت /* CONFIG_HARDWARE_DEBUG_API */

    افتراضي:
        أرجع Arch_سیٹ_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
