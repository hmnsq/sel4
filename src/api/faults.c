/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <mode/api/ipc_buffer.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#ಆದರೆ_ಅಂತ್ಯ

/* consistency with libsel4 */
compile_assert(Invalid_ಬೇರು, lookup_fault_invalid_ಬೇರು + 1 == se_L4_Invalid_ಬೇರು)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(ಆಳ_Mismatch, lookup_fault_ಆಳ_mismatch + 1 == se_L4_ಆಳ_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (ಶಬ್ದ_t) n_syscall_ಸಂದೇಶ == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_ಸಂಖ್ಯೆ, (ಶಬ್ದ_t) n_exception_ಸಂದೇಶ == se_L4_User_Exception_ಸಂಖ್ಯೆ)
compile_assert(se_L4_User_Exception_ಸಂಕೇತ, (ಶಬ್ದ_t) n_exception_ಸಂದೇಶ + 1 == se_L4_User_Exception_ಸಂಕೇತ)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
ಗಣ_MRs_lookup_failure(tcb_t *receiver, ಶಬ್ದ_t *receiveIPCBuffer,
                      lookup_fault_t luf, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    ಶಬ್ದ_t lufType = lookup_fault_get_lufType(luf);
    ಶಬ್ದ_t i;

    i = ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ, lufType + 1);

    /* check constants match libsel4 */
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == seL4_CapFault_LookupFailureType) {
        ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ + 1 == se_L4_Cap_Fault_ಬಿಟ್ಟುಗಳು_ಎಡ);
        ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ + 2 == se_L4_Cap_Fault_ಆಳ_Mismatch_ಬಿಟ್ಟುಗಳು_ಸಿಕ್ಕಿತು);
        ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_ಸಿಕ್ಕಿತು);
        ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ + 3 == se_L4_Cap_Fault_Guard_Mismatch_ಬಿಟ್ಟುಗಳು_ಸಿಕ್ಕಿತು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಪ್ರತಿಜ್ಞಾ(ಸ್ಥಾನಾಂತರ == 1);
    }

    ಆಯ್ಕೆ (lufType) {
    ಸ್ಥಿತಿ lookup_fault_invalid_ಬೇರು:
        ಹಿಂತಿರುಗಿ i;

    ಸ್ಥಿತಿ lookup_fault_missing_capability:
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ + 1,
                     lookup_fault_missing_capability_get_ಬಿಟ್ಟುಗಳು_ಎಡ(luf));

    ಸ್ಥಿತಿ lookup_fault_ಆಳ_mismatch:
        ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ + 1,
              lookup_fault_ಆಳ_mismatch_get_ಬಿಟ್ಟುಗಳು_ಎಡ(luf));
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ + 2,
                     lookup_fault_ಆಳ_mismatch_get_ಬಿಟ್ಟುಗಳು_ಸಿಕ್ಕಿತು(luf));

    ಸ್ಥಿತಿ lookup_fault_guard_mismatch:
        ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ + 1,
              lookup_fault_guard_mismatch_get_ಬಿಟ್ಟುಗಳು_ಎಡ(luf));
        ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ + 2,
              lookup_fault_guard_mismatch_get_guard_ಸಿಕ್ಕಿತು(luf));
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, ಸ್ಥಾನಾಂತರ + 3,
                     lookup_fault_guard_mismatch_get_ಬಿಟ್ಟುಗಳು_ಸಿಕ್ಕಿತು(luf));

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid lookup failure");
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ copy_MRs_Fault_ಉತ್ತರ(tcb_t *sender, tcb_t *receiver, ಸಂದೇಶ_ID_t id, ಶಬ್ದ_t ಉದ್ದಳತೆ)
{
    ಶಬ್ದ_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_ರಿಜಿಸ್ಟರ್_Info(receiver);

    ಗಾಗಿ (i = 0; i < MIN(ಉದ್ದಳತೆ, n_ಸಂದೇಶ_Registers); i++) {
        ರಿಜಿಸ್ಟರ್_t r = fault_messages[id][i];
        ಶಬ್ದ_t v = get_ರಿಜಿಸ್ಟರ್(sender, ಸಂದೇಶ_Registers[i]);
        ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, r, sanitise_ರಿಜಿಸ್ಟರ್(r, v, archInfo));
    }

    ಆದರೆ (i < ಉದ್ದಳತೆ) {
        ಶಬ್ದ_t *sendBuf = lookupIPCBuffer(false, sender);
        ಆದರೆ (sendBuf) {
            ಗಾಗಿ (; i < ಉದ್ದಳತೆ; i++) {
                ರಿಜಿಸ್ಟರ್_t r = fault_messages[id][i];
                ಶಬ್ದ_t v = sendBuf[i + 1];
                ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, r, sanitise_ರಿಜಿಸ್ಟರ್(r, v, archInfo));
            }
        }
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ copyMRsFault(tcb_t *sender, tcb_t *receiver, ಸಂದೇಶ_ID_t id,
                                ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *receiveIPCBuffer)
{
    ಶಬ್ದ_t i;
    ಗಾಗಿ (i = 0; i < MIN(ಉದ್ದಳತೆ, n_ಸಂದೇಶ_Registers); i++) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, ಸಂದೇಶ_Registers[i], get_ರಿಜಿಸ್ಟರ್(sender, fault_messages[id][i]));
    }

    ಆದರೆ (receiveIPCBuffer) {
        ಗಾಗಿ (; i < ಉದ್ದಳತೆ; i++) {
            receiveIPCBuffer[i + 1] = get_ರಿಜಿಸ್ಟರ್(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_ಉತ್ತರ(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_ಸಂದೇಶ_Info_t ಟ್ಯಾಗ್ = ಸಂದೇಶ_Info_From_ಶಬ್ದ(get_ರಿಜಿಸ್ಟರ್(sender, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್));
    ಶಬ್ದ_t label = se_L4_ಸಂದೇಶ_Info_get_label(ಟ್ಯಾಗ್);
    ಶಬ್ದ_t ಉದ್ದಳತೆ = se_L4_ಸಂದೇಶ_Info_get_ಉದ್ದಳತೆ(ಟ್ಯಾಗ್);
    seL4_Fault_t fault = receiver->tcbFault;

    ಆಯ್ಕೆ (seL4_Fault_get_seL4_FaultType(fault)) {
    ಸ್ಥಿತಿ seL4_Fault_CapFault:
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_ಉತ್ತರ(sender, receiver, ಸಂದೇಶ_ID_Syscall, MIN(ಉದ್ದಳತೆ, n_syscall_ಸಂದೇಶ));
        ಹಿಂತಿರುಗಿ (label == 0);

    ಸ್ಥಿತಿ seL4_Fault_UserException:
        copy_MRs_Fault_ಉತ್ತರ(sender, receiver, ಸಂದೇಶ_ID_Exception, MIN(ಉದ್ದಳತೆ, n_exception_ಸಂದೇಶ));
        ಹಿಂತಿರುಗಿ (label == 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ seL4_Fault_Timeout:
        copy_MRs_Fault_ಉತ್ತರ(sender, receiver, ಸಂದೇಶ_ID_Timeout_ಉತ್ತರ, MIN(ಉದ್ದಳತೆ, n_timeout_ಸಂದೇಶ));
        ಹಿಂತಿರುಗಿ (label == 0);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    ಸ್ಥಿತಿ seL4_Fault_DebugException: {
        ಶಬ್ದ_t n_instrs;

        ಆದರೆ (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_ಹೆಜ್ಜೆ) {
            /* Only single-step replies are required to set message registers.
             */
            ಹಿಂತಿರುಗಿ (label == 0);
        }

        ಆದರೆ (ಉದ್ದಳತೆ < DEBUG_ಉತ್ತರ_N_EXPECTED_REGISTERS) {
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
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = get_ರಿಜಿಸ್ಟರ್(sender, ಸಂದೇಶ_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        ಆದರೆ (res.type != seL4_NoError) {
            ಹಿಂತಿರುಗಿ false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        ಹಿಂತಿರುಗಿ (label == 0);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ Arch_handle_Fault_ಉತ್ತರ(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

ಶಬ್ದ_t ಗಣ_MRs_fault(tcb_t *sender, tcb_t *receiver, ಶಬ್ದ_t *receiveIPCBuffer)
{
    ಆಯ್ಕೆ (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    ಸ್ಥಿತಿ seL4_Fault_CapFault:
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_ವಿಳಾಸ(sender->tcbFault));
        ಗಣ_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_ಒಳ_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_ಒಳ_Receive_Phase(sender->tcbFault));
        ಹಿಂತಿರುಗಿ ಗಣ_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    ಸ್ಥಿತಿ seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, ಸಂದೇಶ_ID_Syscall, n_syscall_ಸಂದೇಶ,
                     receiveIPCBuffer);

        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, n_syscall_ಸಂದೇಶ,
                     se_L4_Fault_Unknown_Syscall_get_syscall_ಸಂಖ್ಯೆ(sender->tcbFault));
    }

    ಸ್ಥಿತಿ seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, ಸಂದೇಶ_ID_Exception,
                     n_exception_ಸಂದೇಶ, receiveIPCBuffer);
        ಗಣ_MR(receiver, receiveIPCBuffer, n_exception_ಸಂದೇಶ,
              se_L4_Fault_User_Exception_get_ಸಂಖ್ಯೆ(sender->tcbFault));
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, n_exception_ಸಂದೇಶ + 1u,
                     se_L4_Fault_User_Exception_get_ಸಂಕೇತ(sender->tcbFault));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ seL4_Fault_Timeout: {
        ಆದರೆ (sender->tcbSchedContext) {
            ಶಬ್ದ_t len;
            ಸಮಯ_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = ಗಣ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_ದತ್ತಾಂಶ,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            ಹಿಂತಿರುಗಿ ಬಹುಲಕ_ಗಣ_ಸಮಯ_Arg(len, consumed, receiveIPCBuffer, receiver);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, se_L4_Timeout_ದತ್ತಾಂಶ,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    ಸ್ಥಿತಿ seL4_Fault_DebugException: {
        ಶಬ್ದ_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        ಗಣ_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ret = ಗಣ_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        ಆದರೆ (reason != se_L4_Single_ಹೆಜ್ಜೆ && reason != seL4_SoftwareBreakRequest) {
            ret = ಗಣ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_ವಿಳಾಸ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_ವಿಳಾಸ(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = ಗಣ_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_ಸಂಖ್ಯೆ,
                        se_L4_Fault_Debug_Exception_get_breakpoint_ಸಂಖ್ಯೆ(sender->tcbFault));
        }
        ಹಿಂತಿರುಗಿ ret;
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ Arch_ಗಣ_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
