/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/api/ipc_buffer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontext.h>
#ਜੇ_ਅੰਤ

/* consistency with libsel4 */
compile_assert(Invalid_ਜੜ੍ਹ, lookup_fault_invalid_ਜੜ੍ਹ + 1 == se_L4_Invalid_ਜੜ੍ਹ)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(ਡੂੰਘਾਈ_Mismatch, lookup_fault_ਡੂੰਘਾਈ_mismatch + 1 == se_L4_ਡੂੰਘਾਈ_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (ਸ਼ਬਦ_t) n_syscall_ਸੁਨੇਹਾ == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_ਸੰਖਿਆ, (ਸ਼ਬਦ_t) n_exception_ਸੁਨੇਹਾ == se_L4_User_Exception_ਸੰਖਿਆ)
compile_assert(se_L4_User_Exception_ਕੋਡ, (ਸ਼ਬਦ_t) n_exception_ਸੁਨੇਹਾ + 1 == se_L4_User_Exception_ਕੋਡ)

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ
ਸੈੱਟ_MRs_lookup_failure(tcb_t *receiver, ਸ਼ਬਦ_t *receiveIPCBuffer,
                      lookup_fault_t luf, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਥ)
{
    ਸ਼ਬਦ_t lufType = lookup_fault_get_lufType(luf);
    ਸ਼ਬਦ_t i;

    i = ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ, lufType + 1);

    /* check constants match libsel4 */
    ਜੇ (ਵਿੱਥ == seL4_CapFault_LookupFailureType) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ + 1 == se_L4_Cap_Fault_ਬਿੱਟ_ਖੱਬਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ + 2 == se_L4_Cap_Fault_ਡੂੰਘਾਈ_Mismatch_ਬਿੱਟ_ਮਿਲਿਆ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_ਮਿਲਿਆ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ + 3 == se_L4_Cap_Fault_Guard_Mismatch_ਬਿੱਟ_ਮਿਲਿਆ);
    } ਨਹੀਂਤਾਂ {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਥ == 1);
    }

    ਚੋਣ (lufType) {
    ਹਾਲਤ lookup_fault_invalid_ਜੜ੍ਹ:
        ਵਾਪਸ i;

    ਹਾਲਤ lookup_fault_missing_capability:
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ + 1,
                     lookup_fault_missing_capability_get_ਬਿੱਟ_ਖੱਬਾ(luf));

    ਹਾਲਤ lookup_fault_ਡੂੰਘਾਈ_mismatch:
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ + 1,
              lookup_fault_ਡੂੰਘਾਈ_mismatch_get_ਬਿੱਟ_ਖੱਬਾ(luf));
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ + 2,
                     lookup_fault_ਡੂੰਘਾਈ_mismatch_get_ਬਿੱਟ_ਮਿਲਿਆ(luf));

    ਹਾਲਤ lookup_fault_guard_mismatch:
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ + 1,
              lookup_fault_guard_mismatch_get_ਬਿੱਟ_ਖੱਬਾ(luf));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ + 2,
              lookup_fault_guard_mismatch_get_guard_ਮਿਲਿਆ(luf));
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, ਵਿੱਥ + 3,
                     lookup_fault_guard_mismatch_get_ਬਿੱਟ_ਮਿਲਿਆ(luf));

    ਮੂਲ:
        fail("Invalid lookup failure");
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ copy_MRs_Fault_ਜਵਾਬ(tcb_t *sender, tcb_t *receiver, ਸੁਨੇਹਾ_ID_t id, ਸ਼ਬਦ_t ਲੰਮਾਈ)
{
    ਸ਼ਬਦ_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_ਰਜਿਸਟਰ_Info(receiver);

    ਲਈ (i = 0; i < MIN(ਲੰਮਾਈ, n_ਸੁਨੇਹਾ_Registers); i++) {
        ਰਜਿਸਟਰ_t r = fault_messages[id][i];
        ਸ਼ਬਦ_t v = get_ਰਜਿਸਟਰ(sender, ਸੁਨੇਹਾ_Registers[i]);
        ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, r, sanitise_ਰਜਿਸਟਰ(r, v, archInfo));
    }

    ਜੇ (i < ਲੰਮਾਈ) {
        ਸ਼ਬਦ_t *sendBuf = lookupIPCBuffer(false, sender);
        ਜੇ (sendBuf) {
            ਲਈ (; i < ਲੰਮਾਈ; i++) {
                ਰਜਿਸਟਰ_t r = fault_messages[id][i];
                ਸ਼ਬਦ_t v = sendBuf[i + 1];
                ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, r, sanitise_ਰਜਿਸਟਰ(r, v, archInfo));
            }
        }
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ copyMRsFault(tcb_t *sender, tcb_t *receiver, ਸੁਨੇਹਾ_ID_t id,
                                ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *receiveIPCBuffer)
{
    ਸ਼ਬਦ_t i;
    ਲਈ (i = 0; i < MIN(ਲੰਮਾਈ, n_ਸੁਨੇਹਾ_Registers); i++) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, ਸੁਨੇਹਾ_Registers[i], get_ਰਜਿਸਟਰ(sender, fault_messages[id][i]));
    }

    ਜੇ (receiveIPCBuffer) {
        ਲਈ (; i < ਲੰਮਾਈ; i++) {
            receiveIPCBuffer[i + 1] = get_ਰਜਿਸਟਰ(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_ਜਵਾਬ(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_ਸੁਨੇਹਾ_Info_t ਟੈਗ = ਸੁਨੇਹਾ_Info_From_ਸ਼ਬਦ(get_ਰਜਿਸਟਰ(sender, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ));
    ਸ਼ਬਦ_t label = se_L4_ਸੁਨੇਹਾ_Info_get_label(ਟੈਗ);
    ਸ਼ਬਦ_t ਲੰਮਾਈ = se_L4_ਸੁਨੇਹਾ_Info_get_ਲੰਮਾਈ(ਟੈਗ);
    seL4_Fault_t fault = receiver->tcbFault;

    ਚੋਣ (seL4_Fault_get_seL4_FaultType(fault)) {
    ਹਾਲਤ seL4_Fault_CapFault:
        ਵਾਪਸ true;

    ਹਾਲਤ seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_ਜਵਾਬ(sender, receiver, ਸੁਨੇਹਾ_ID_Syscall, MIN(ਲੰਮਾਈ, n_syscall_ਸੁਨੇਹਾ));
        ਵਾਪਸ (label == 0);

    ਹਾਲਤ seL4_Fault_UserException:
        copy_MRs_Fault_ਜਵਾਬ(sender, receiver, ਸੁਨੇਹਾ_ID_Exception, MIN(ਲੰਮਾਈ, n_exception_ਸੁਨੇਹਾ));
        ਵਾਪਸ (label == 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ seL4_Fault_Timeout:
        copy_MRs_Fault_ਜਵਾਬ(sender, receiver, ਸੁਨੇਹਾ_ID_Timeout_ਜਵਾਬ, MIN(ਲੰਮਾਈ, n_timeout_ਸੁਨੇਹਾ));
        ਵਾਪਸ (label == 0);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    ਹਾਲਤ seL4_Fault_DebugException: {
        ਸ਼ਬਦ_t n_instrs;

        ਜੇ (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_ਕਦਮ) {
            /* Only single-step replies are required to set message registers.
             */
            ਵਾਪਸ (label == 0);
        }

        ਜੇ (ਲੰਮਾਈ < DEBUG_ਜਵਾਬ_N_EXPECTED_REGISTERS) {
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
        } ਨਹੀਂਤਾਂ {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = get_ਰਜਿਸਟਰ(sender, ਸੁਨੇਹਾ_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        ਜੇ (res.type != seL4_NoError) {
            ਵਾਪਸ false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        ਵਾਪਸ (label == 0);
    }
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        ਵਾਪਸ Arch_handle_Fault_ਜਵਾਬ(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

ਸ਼ਬਦ_t ਸੈੱਟ_MRs_fault(tcb_t *sender, tcb_t *receiver, ਸ਼ਬਦ_t *receiveIPCBuffer)
{
    ਚੋਣ (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    ਹਾਲਤ seL4_Fault_CapFault:
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_ਪਤਾ(sender->tcbFault));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_ਵਿੱਚ_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_ਵਿੱਚ_Receive_Phase(sender->tcbFault));
        ਵਾਪਸ ਸੈੱਟ_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    ਹਾਲਤ seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, ਸੁਨੇਹਾ_ID_Syscall, n_syscall_ਸੁਨੇਹਾ,
                     receiveIPCBuffer);

        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, n_syscall_ਸੁਨੇਹਾ,
                     se_L4_Fault_Unknown_Syscall_get_syscall_ਸੰਖਿਆ(sender->tcbFault));
    }

    ਹਾਲਤ seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, ਸੁਨੇਹਾ_ID_Exception,
                     n_exception_ਸੁਨੇਹਾ, receiveIPCBuffer);
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, n_exception_ਸੁਨੇਹਾ,
              se_L4_Fault_User_Exception_get_ਸੰਖਿਆ(sender->tcbFault));
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, n_exception_ਸੁਨੇਹਾ + 1u,
                     se_L4_Fault_User_Exception_get_ਕੋਡ(sender->tcbFault));
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ seL4_Fault_Timeout: {
        ਜੇ (sender->tcbSchedContext) {
            ਸ਼ਬਦ_t len;
            ਸਮਾਂ_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = ਸੈੱਟ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_ਅੰਕੜੇ,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            ਵਾਪਸ ਬਹੁਲਕ_ਸੈੱਟ_ਸਮਾਂ_Arg(len, consumed, receiveIPCBuffer, receiver);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_ਅੰਕੜੇ,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    ਹਾਲਤ seL4_Fault_DebugException: {
        ਸ਼ਬਦ_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        ਸੈੱਟ_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ret = ਸੈੱਟ_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        ਜੇ (reason != se_L4_Single_ਕਦਮ && reason != seL4_SoftwareBreakRequest) {
            ret = ਸੈੱਟ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_ਪਤਾ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_ਪਤਾ(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = ਸੈੱਟ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_ਸੰਖਿਆ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_ਸੰਖਿਆ(sender->tcbFault));
        }
        ਵਾਪਸ ret;
    }
#ਜੇ_ਅੰਤ /* CONFIG_HARDWARE_DEBUG_API */

    ਮੂਲ:
        ਵਾਪਸ Arch_ਸੈੱਟ_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
