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
#که_تعریف_شوی CONFIG_KERNEL_MCS
#ضمن <mode/api/ipc_buffer.h>
#ضمن <object/schedcontext.h>
#د_که_پای

/* consistency with libsel4 */
compile_assert(Invalid_ریښه, lookup_fault_invalid_ریښه + 1 == se_L4_Invalid_ریښه)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(ژوروالی_Mismatch, lookup_fault_ژوروالی_mismatch + 1 == se_L4_ژوروالی_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (کلمه_t) n_syscall_پیغام == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_شمېره, (کلمه_t) n_exception_پیغام == se_L4_User_Exception_شمېره)
compile_assert(se_L4_User_Exception_کوډ, (کلمه_t) n_exception_پیغام + 1 == se_L4_User_Exception_کوډ)

ساكن مضمن بینښه صحيح
سېټ_MRs_lookup_failure(tcb_t *receiver, کلمه_t *receiveIPCBuffer,
                      lookup_fault_t luf, بینښه صحيح دننه_واټن)
{
    کلمه_t lufType = lookup_fault_get_lufType(luf);
    کلمه_t i;

    i = سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن, lufType + 1);

    /* check constants match libsel4 */
    إذا (دننه_واټن == seL4_CapFault_LookupFailureType) {
        أكد(دننه_واټن + 1 == se_L4_Cap_Fault_بټونه_کیڼ);
        أكد(دننه_واټن + 2 == se_L4_Cap_Fault_ژوروالی_Mismatch_بټونه_وموندل);
        أكد(دننه_واټن + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_وموندل);
        أكد(دننه_واټن + 3 == se_L4_Cap_Fault_Guard_Mismatch_بټونه_وموندل);
    } وإلا {
        أكد(دننه_واټن == 1);
    }

    بدل (lufType) {
    حالة lookup_fault_invalid_ریښه:
        أرجع i;

    حالة lookup_fault_missing_capability:
        أرجع سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن + 1,
                     lookup_fault_missing_capability_get_بټونه_کیڼ(luf));

    حالة lookup_fault_ژوروالی_mismatch:
        سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن + 1,
              lookup_fault_ژوروالی_mismatch_get_بټونه_کیڼ(luf));
        أرجع سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن + 2,
                     lookup_fault_ژوروالی_mismatch_get_بټونه_وموندل(luf));

    حالة lookup_fault_guard_mismatch:
        سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن + 1,
              lookup_fault_guard_mismatch_get_بټونه_کیڼ(luf));
        سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن + 2,
              lookup_fault_guard_mismatch_get_guard_وموندل(luf));
        أرجع سېټ_MR(receiver, receiveIPCBuffer, دننه_واټن + 3,
                     lookup_fault_guard_mismatch_get_بټونه_وموندل(luf));

    افتراضي:
        fail("Invalid lookup failure");
    }
}

ساكن مضمن فراغ copy_MRs_Fault_ځواب(tcb_t *sender, tcb_t *receiver, پیغام_ID_t id, کلمه_t اوږدتیا)
{
    کلمه_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_ثبتوونکی_Info(receiver);

    لكل (i = 0; i < MIN(اوږدتیا, n_پیغام_Registers); i++) {
        ثبتوونکی_t r = fault_messages[id][i];
        کلمه_t v = get_ثبتوونکی(sender, پیغام_Registers[i]);
        سېټ_ثبتوونکی(receiver, r, sanitise_ثبتوونکی(r, v, archInfo));
    }

    إذا (i < اوږدتیا) {
        کلمه_t *sendBuf = lookupIPCBuffer(false, sender);
        إذا (sendBuf) {
            لكل (; i < اوږدتیا; i++) {
                ثبتوونکی_t r = fault_messages[id][i];
                کلمه_t v = sendBuf[i + 1];
                سېټ_ثبتوونکی(receiver, r, sanitise_ثبتوونکی(r, v, archInfo));
            }
        }
    }
}

ساكن مضمن فراغ copyMRsFault(tcb_t *sender, tcb_t *receiver, پیغام_ID_t id,
                                کلمه_t اوږدتیا, کلمه_t *receiveIPCBuffer)
{
    کلمه_t i;
    لكل (i = 0; i < MIN(اوږدتیا, n_پیغام_Registers); i++) {
        سېټ_ثبتوونکی(receiver, پیغام_Registers[i], get_ثبتوونکی(sender, fault_messages[id][i]));
    }

    إذا (receiveIPCBuffer) {
        لكل (; i < اوږدتیا; i++) {
            receiveIPCBuffer[i + 1] = get_ثبتوونکی(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_ځواب(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_پیغام_Info_t نښان = پیغام_Info_From_کلمه(get_ثبتوونکی(sender, پیغام_Info_ثبتوونکی));
    کلمه_t label = se_L4_پیغام_Info_get_label(نښان);
    کلمه_t اوږدتیا = se_L4_پیغام_Info_get_اوږدتیا(نښان);
    seL4_Fault_t fault = receiver->tcbFault;

    بدل (seL4_Fault_get_seL4_FaultType(fault)) {
    حالة seL4_Fault_CapFault:
        أرجع true;

    حالة seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_ځواب(sender, receiver, پیغام_ID_Syscall, MIN(اوږدتیا, n_syscall_پیغام));
        أرجع (label == 0);

    حالة seL4_Fault_UserException:
        copy_MRs_Fault_ځواب(sender, receiver, پیغام_ID_Exception, MIN(اوږدتیا, n_exception_پیغام));
        أرجع (label == 0);

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        copy_MRs_Fault_ځواب(sender, receiver, پیغام_ID_Timeout_ځواب, MIN(اوږدتیا, n_timeout_پیغام));
        أرجع (label == 0);
#د_که_پای
#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        کلمه_t n_instrs;

        إذا (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_ګام) {
            /* Only single-step replies are required to set message registers.
             */
            أرجع (label == 0);
        }

        إذا (اوږدتیا < DEBUG_ځواب_N_EXPECTED_REGISTERS) {
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
            n_instrs = get_ثبتوونکی(sender, پیغام_Registers[0]);
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
#د_که_پای

    افتراضي:
        أرجع Arch_handle_Fault_ځواب(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

کلمه_t سېټ_MRs_fault(tcb_t *sender, tcb_t *receiver, کلمه_t *receiveIPCBuffer)
{
    بدل (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالة seL4_Fault_CapFault:
        سېټ_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        سېټ_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_پته(sender->tcbFault));
        سېټ_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_دننه_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_دننه_Receive_Phase(sender->tcbFault));
        أرجع سېټ_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالة seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, پیغام_ID_Syscall, n_syscall_پیغام,
                     receiveIPCBuffer);

        أرجع سېټ_MR(receiver, receiveIPCBuffer, n_syscall_پیغام,
                     se_L4_Fault_Unknown_Syscall_get_syscall_شمېره(sender->tcbFault));
    }

    حالة seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, پیغام_ID_Exception,
                     n_exception_پیغام, receiveIPCBuffer);
        سېټ_MR(receiver, receiveIPCBuffer, n_exception_پیغام,
              se_L4_Fault_User_Exception_get_شمېره(sender->tcbFault));
        أرجع سېټ_MR(receiver, receiveIPCBuffer, n_exception_پیغام + 1u,
                     se_L4_Fault_User_Exception_get_کوډ(sender->tcbFault));
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout: {
        إذا (sender->tcbSchedContext) {
            کلمه_t len;
            وخت_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = سېټ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_معلومات,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            أرجع ډېرپېښ_سېټ_وخت_Arg(len, consumed, receiveIPCBuffer, receiver);
        } وإلا {
            أرجع سېټ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_معلومات,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#د_که_پای
#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        کلمه_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        سېټ_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        بینښه صحيح ret = سېټ_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        إذا (reason != se_L4_Single_ګام && reason != seL4_SoftwareBreakRequest) {
            ret = سېټ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_پته,
                        se_L4_Fault_Debug_Exception_get_breakpoint_پته(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = سېټ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_شمېره,
                        se_L4_Fault_Debug_Exception_get_breakpoint_شمېره(sender->tcbFault));
        }
        أرجع ret;
    }
#د_که_پای /* CONFIG_HARDWARE_DEBUG_API */

    افتراضي:
        أرجع Arch_سېټ_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
