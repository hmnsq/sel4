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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
#ضمن <mode/api/ipc_buffer.h>
#ضمن <object/schedcontext.h>
#پایان_اگر

/* consistency with libsel4 */
compile_assert(Invalid_ریشه, lookup_fault_invalid_ریشه + 1 == se_L4_Invalid_ریشه)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(عمق_Mismatch, lookup_fault_عمق_mismatch + 1 == se_L4_عمق_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (کلمه_t) n_syscall_پیام == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_عدد, (کلمه_t) n_exception_پیام == se_L4_User_Exception_عدد)
compile_assert(se_L4_User_Exception_کد, (کلمه_t) n_exception_پیام + 1 == se_L4_User_Exception_کد)

ساكن مضمن بیعلامت صحيح
مجموعه_MRs_lookup_failure(tcb_t *receiver, کلمه_t *receiveIPCBuffer,
                      lookup_fault_t luf, بیعلامت صحيح جابه_جایی)
{
    کلمه_t lufType = lookup_fault_get_lufType(luf);
    کلمه_t i;

    i = مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی, lufType + 1);

    /* check constants match libsel4 */
    إذا (جابه_جایی == seL4_CapFault_LookupFailureType) {
        أكد(جابه_جایی + 1 == se_L4_Cap_Fault_بیتها_چپ);
        أكد(جابه_جایی + 2 == se_L4_Cap_Fault_عمق_Mismatch_بیتها_یافت);
        أكد(جابه_جایی + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_یافت);
        أكد(جابه_جایی + 3 == se_L4_Cap_Fault_Guard_Mismatch_بیتها_یافت);
    } وإلا {
        أكد(جابه_جایی == 1);
    }

    بدل (lufType) {
    حالة lookup_fault_invalid_ریشه:
        أرجع i;

    حالة lookup_fault_missing_capability:
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی + 1,
                     lookup_fault_missing_capability_get_بیتها_چپ(luf));

    حالة lookup_fault_عمق_mismatch:
        مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی + 1,
              lookup_fault_عمق_mismatch_get_بیتها_چپ(luf));
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی + 2,
                     lookup_fault_عمق_mismatch_get_بیتها_یافت(luf));

    حالة lookup_fault_guard_mismatch:
        مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی + 1,
              lookup_fault_guard_mismatch_get_بیتها_چپ(luf));
        مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی + 2,
              lookup_fault_guard_mismatch_get_guard_یافت(luf));
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, جابه_جایی + 3,
                     lookup_fault_guard_mismatch_get_بیتها_یافت(luf));

    افتراضي:
        fail("Invalid lookup failure");
    }
}

ساكن مضمن فراغ copy_MRs_Fault_پاسخ(tcb_t *sender, tcb_t *receiver, پیام_ID_t id, کلمه_t درازی)
{
    کلمه_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_ثبات_Info(receiver);

    لكل (i = 0; i < MIN(درازی, n_پیام_Registers); i++) {
        ثبات_t r = fault_messages[id][i];
        کلمه_t v = get_ثبات(sender, پیام_Registers[i]);
        مجموعه_ثبات(receiver, r, sanitise_ثبات(r, v, archInfo));
    }

    إذا (i < درازی) {
        کلمه_t *sendBuf = lookupIPCBuffer(false, sender);
        إذا (sendBuf) {
            لكل (; i < درازی; i++) {
                ثبات_t r = fault_messages[id][i];
                کلمه_t v = sendBuf[i + 1];
                مجموعه_ثبات(receiver, r, sanitise_ثبات(r, v, archInfo));
            }
        }
    }
}

ساكن مضمن فراغ copyMRsFault(tcb_t *sender, tcb_t *receiver, پیام_ID_t id,
                                کلمه_t درازی, کلمه_t *receiveIPCBuffer)
{
    کلمه_t i;
    لكل (i = 0; i < MIN(درازی, n_پیام_Registers); i++) {
        مجموعه_ثبات(receiver, پیام_Registers[i], get_ثبات(sender, fault_messages[id][i]));
    }

    إذا (receiveIPCBuffer) {
        لكل (; i < درازی; i++) {
            receiveIPCBuffer[i + 1] = get_ثبات(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_پاسخ(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_پیام_Info_t برچسب = پیام_Info_From_کلمه(get_ثبات(sender, پیام_Info_ثبات));
    کلمه_t label = se_L4_پیام_Info_get_label(برچسب);
    کلمه_t درازی = se_L4_پیام_Info_get_درازی(برچسب);
    seL4_Fault_t fault = receiver->tcbFault;

    بدل (seL4_Fault_get_seL4_FaultType(fault)) {
    حالة seL4_Fault_CapFault:
        أرجع true;

    حالة seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_پاسخ(sender, receiver, پیام_ID_Syscall, MIN(درازی, n_syscall_پیام));
        أرجع (label == 0);

    حالة seL4_Fault_UserException:
        copy_MRs_Fault_پاسخ(sender, receiver, پیام_ID_Exception, MIN(درازی, n_exception_پیام));
        أرجع (label == 0);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        copy_MRs_Fault_پاسخ(sender, receiver, پیام_ID_Timeout_پاسخ, MIN(درازی, n_timeout_پیام));
        أرجع (label == 0);
#پایان_اگر
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        کلمه_t n_instrs;

        إذا (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_گام) {
            /* Only single-step replies are required to set message registers.
             */
            أرجع (label == 0);
        }

        إذا (درازی < DEBUG_پاسخ_N_EXPECTED_REGISTERS) {
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
            n_instrs = get_ثبات(sender, پیام_Registers[0]);
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
#پایان_اگر

    افتراضي:
        أرجع Arch_handle_Fault_پاسخ(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

کلمه_t مجموعه_MRs_fault(tcb_t *sender, tcb_t *receiver, کلمه_t *receiveIPCBuffer)
{
    بدل (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    حالة seL4_Fault_CapFault:
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_آدرس(sender->tcbFault));
        مجموعه_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_درون_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_درون_Receive_Phase(sender->tcbFault));
        أرجع مجموعه_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    حالة seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, پیام_ID_Syscall, n_syscall_پیام,
                     receiveIPCBuffer);

        أرجع مجموعه_MR(receiver, receiveIPCBuffer, n_syscall_پیام,
                     se_L4_Fault_Unknown_Syscall_get_syscall_عدد(sender->tcbFault));
    }

    حالة seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, پیام_ID_Exception,
                     n_exception_پیام, receiveIPCBuffer);
        مجموعه_MR(receiver, receiveIPCBuffer, n_exception_پیام,
              se_L4_Fault_User_Exception_get_عدد(sender->tcbFault));
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, n_exception_پیام + 1u,
                     se_L4_Fault_User_Exception_get_کد(sender->tcbFault));
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout: {
        إذا (sender->tcbSchedContext) {
            کلمه_t len;
            زمان_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = مجموعه_MR(receiver, receiveIPCBuffer, se_L4_Timeout_داده,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            أرجع نما_مجموعه_زمان_Arg(len, consumed, receiveIPCBuffer, receiver);
        } وإلا {
            أرجع مجموعه_MR(receiver, receiveIPCBuffer, se_L4_Timeout_داده,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#پایان_اگر
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    حالة seL4_Fault_DebugException: {
        کلمه_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        مجموعه_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        بیعلامت صحيح ret = مجموعه_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        إذا (reason != se_L4_Single_گام && reason != seL4_SoftwareBreakRequest) {
            ret = مجموعه_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_آدرس,
                        se_L4_Fault_Debug_Exception_get_breakpoint_آدرس(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = مجموعه_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_عدد,
                        se_L4_Fault_Debug_Exception_get_breakpoint_عدد(sender->tcbFault));
        }
        أرجع ret;
    }
#پایان_اگر /* CONFIG_HARDWARE_DEBUG_API */

    افتراضي:
        أرجع Arch_مجموعه_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
