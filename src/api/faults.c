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
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#ضمن <mode/api/ipc_buffer.h>
#ضمن <object/schedcontext.h>
#جيڪڏهن_پورو

/* consistency with libsel4 */
compile_assert(Invalid_پاڙ, lookup_fault_invalid_پاڙ + 1 == se_L4_Invalid_پاڙ)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(اونهائي_Mismatch, lookup_fault_اونهائي_mismatch + 1 == se_L4_اونهائي_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (لفظ_t) n_syscall_پيغام == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_عدد, (لفظ_t) n_exception_پيغام == se_L4_User_Exception_عدد)
compile_assert(se_L4_User_Exception_ڪوڊ, (لفظ_t) n_exception_پيغام + 1 == se_L4_User_Exception_ڪوڊ)

ساكن مضمن بنانشان صحيح
سيٽ_MRs_lookup_failure(tcb_t *receiver, لفظ_t *receiveIPCBuffer,
                      lookup_fault_t luf, بنانشان صحيح وٿي)
{
    لفظ_t lufType = lookup_fault_get_lufType(luf);
    لفظ_t i;

    i = سيٽ_MR(receiver, receiveIPCBuffer, وٿي, lufType + 1);

    /* check constants match libsel4 */
    إذا (وٿي == seL4_CapFault_LookupFailureType) {
        أكد(وٿي + 1 == se_L4_Cap_Fault_بٽ_کاٻو);
        أكد(وٿي + 2 == se_L4_Cap_Fault_اونهائي_Mismatch_بٽ_مليو);
        أكد(وٿي + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_مليو);
        أكد(وٿي + 3 == se_L4_Cap_Fault_Guard_Mismatch_بٽ_مليو);
    } وإلا {
        أكد(وٿي == 1);
    }

    بدل (lufType) {
    حالة lookup_fault_invalid_پاڙ:
        أرجع i;

    حالة lookup_fault_missing_capability:
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, وٿي + 1,
                     lookup_fault_missing_capability_get_بٽ_کاٻو(luf));

    حالة lookup_fault_اونهائي_mismatch:
        سيٽ_MR(receiver, receiveIPCBuffer, وٿي + 1,
              lookup_fault_اونهائي_mismatch_get_بٽ_کاٻو(luf));
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, وٿي + 2,
                     lookup_fault_اونهائي_mismatch_get_بٽ_مليو(luf));

    حالة lookup_fault_guard_mismatch:
        سيٽ_MR(receiver, receiveIPCBuffer, وٿي + 1,
              lookup_fault_guard_mismatch_get_بٽ_کاٻو(luf));
        سيٽ_MR(receiver, receiveIPCBuffer, وٿي + 2,
              lookup_fault_guard_mismatch_get_guard_مليو(luf));
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, وٿي + 3,
                     lookup_fault_guard_mismatch_get_بٽ_مليو(luf));

    افتراضي:
        fail("Invalid lookup failure");
    }
}

ساكن مضمن فراغ copy_MRs_Fault_جواب(tcb_t *sender, tcb_t *receiver, پيغام_ID_t id, لفظ_t ڊيگهائي)
{
    لفظ_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_رجسٽر_Info(receiver);

    لكل (i = 0; i < MIN(ڊيگهائي, n_نياپو_Registers); i++) {
        رجسٽر_t r = fault_messages[id][i];
        لفظ_t v = get_رجسٽر(sender, نياپو_Registers[i]);
        سيٽ_رجسٽر(receiver, r, sanitise_رجسٽر(r, v, archInfo));
    }

    إذا (i < ڊيگهائي) {
        لفظ_t *sendBuf = lookupIPCBuffer(false, sender);
        إذا (sendBuf) {
            لكل (; i < ڊيگهائي; i++) {
                رجسٽر_t r = fault_messages[id][i];
                لفظ_t v = sendBuf[i + 1];
                سيٽ_رجسٽر(receiver, r, sanitise_رجسٽر(r, v, archInfo));
            }
        }
    }
}

ساكن مضمن فراغ copyMRsFault(tcb_t *sender, tcb_t *receiver, پيغام_ID_t id,
                                لفظ_t ڊيگهائي, لفظ_t *receiveIPCBuffer)
{
    لفظ_t i;
    لكل (i = 0; i < MIN(ڊيگهائي, n_نياپو_Registers); i++) {
        سيٽ_رجسٽر(receiver, نياپو_Registers[i], get_رجسٽر(sender, fault_messages[id][i]));
    }

    إذا (receiveIPCBuffer) {
        لكل (; i < ڊيگهائي; i++) {
            receiveIPCBuffer[i + 1] = get_رجسٽر(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_جواب(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_پيغام_Info_t ٽيگ = پيغام_Info_From_لفظ(get_رجسٽر(sender, نياپو_Info_رجسٽر));
    لفظ_t label = se_L4_پيغام_Info_get_label(ٽيگ);
    لفظ_t ڊيگهائي = se_L4_پيغام_Info_get_ڊيگهائي(ٽيگ);
    seL4_Fault_t fault = receiver->tcbFault;

    بدل (seL4_Fault_get_seL4_FaultType(fault)) {
    حالة seL4_Fault_CapFault:
        أرجع true;

    حالة seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_جواب(sender, receiver, پيغام_ID_Syscall, MIN(ڊيگهائي, n_syscall_پيغام));
        أرجع (label == 0);

    حالة seL4_Fault_UserException:
        copy_MRs_Fault_جواب(sender, receiver, پيغام_ID_Exception, MIN(ڊيگهائي, n_exception_پيغام));
        أرجع (label == 0);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        copy_MRs_Fault_جواب(sender, receiver, پيغام_ID_Timeout_جواب, MIN(ڊيگهائي, n_timeout_پيغام));
        أرجع (label == 0);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        لفظ_t n_instrs;

        إذا (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_قدم) {
            /* Only single-step replies are required to set message registers.
             */
            أرجع (label == 0);
        }

        إذا (ڊيگهائي < DEBUG_جواب_N_EXPECTED_REGISTERS) {
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
            n_instrs = get_رجسٽر(sender, نياپو_Registers[0]);
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
#جيڪڏهن_پورو

    افتراضي:
        أرجع Arch_handle_Fault_جواب(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

لفظ_t سيٽ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer)
{
    بدل (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالة seL4_Fault_CapFault:
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_پتو(sender->tcbFault));
        سيٽ_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_اندريون_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_اندريون_Receive_Phase(sender->tcbFault));
        أرجع سيٽ_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالة seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, پيغام_ID_Syscall, n_syscall_پيغام,
                     receiveIPCBuffer);

        أرجع سيٽ_MR(receiver, receiveIPCBuffer, n_syscall_پيغام,
                     se_L4_Fault_Unknown_Syscall_get_syscall_عدد(sender->tcbFault));
    }

    حالة seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, پيغام_ID_Exception,
                     n_exception_پيغام, receiveIPCBuffer);
        سيٽ_MR(receiver, receiveIPCBuffer, n_exception_پيغام,
              se_L4_Fault_User_Exception_get_عدد(sender->tcbFault));
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, n_exception_پيغام + 1u,
                     se_L4_Fault_User_Exception_get_ڪوڊ(sender->tcbFault));
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout: {
        إذا (sender->tcbSchedContext) {
            لفظ_t len;
            وقت_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = سيٽ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_معلومات,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            أرجع گهڻائي_سيٽ_وقت_Arg(len, consumed, receiveIPCBuffer, receiver);
        } وإلا {
            أرجع سيٽ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_معلومات,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        لفظ_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        سيٽ_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        بنانشان صحيح ret = سيٽ_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        إذا (reason != se_L4_Single_قدم && reason != seL4_SoftwareBreakRequest) {
            ret = سيٽ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_پتو,
                        se_L4_Fault_Debug_Exception_get_breakpoint_پتو(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = سيٽ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_عدد,
                        se_L4_Fault_Debug_Exception_get_breakpoint_عدد(sender->tcbFault));
        }
        أرجع ret;
    }
#جيڪڏهن_پورو /* CONFIG_HARDWARE_DEBUG_API */

    افتراضي:
        أرجع Arch_سيٽ_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
