/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <arch/kernel/thread.h>
#అన్తర్భావ <machine/debug.h>
#నిర్వచితమైతే CONFIG_KERNEL_MCS
#అన్తర్భావ <mode/api/ipc_buffer.h>
#అన్తర్భావ <object/schedcontext.h>
#అయితే_ముగింపు

/* consistency with libsel4 */
compile_assert(Invalid_మూలం, lookup_fault_invalid_మూలం + 1 == se_L4_Invalid_మూలం)
compile_assert(MissingCapability, lookup_fault_missing_capability + 1 == seL4_MissingCapability)
compile_assert(లోతు_Mismatch, lookup_fault_లోతు_mismatch + 1 == se_L4_లోతు_Mismatch)
compile_assert(GuardMismatch, lookup_fault_guard_mismatch + 1 == seL4_GuardMismatch)
compile_assert(seL4_UnknownSyscall_Syscall, (మాట_t) n_syscall_సందేశం == seL4_UnknownSyscall_Syscall)
compile_assert(se_L4_User_Exception_సంఖ్య, (మాట_t) n_exception_సందేశం == se_L4_User_Exception_సంఖ్య)
compile_assert(se_L4_User_Exception_సంకేతం, (మాట_t) n_exception_సందేశం + 1 == se_L4_User_Exception_సంకేతం)

స్థిర అంతఃవరుస గుర్తులేని పూర్ణాంకం
సమితి_MRs_lookup_failure(tcb_t *receiver, మాట_t *receiveIPCBuffer,
                      lookup_fault_t luf, గుర్తులేని పూర్ణాంకం స్థానభ్రంశం)
{
    మాట_t lufType = lookup_fault_get_lufType(luf);
    మాట_t i;

    i = సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం, lufType + 1);

    /* check constants match libsel4 */
    ఒకవేళ (స్థానభ్రంశం == seL4_CapFault_LookupFailureType) {
        ప్రతిజ్ఞా(స్థానభ్రంశం + 1 == se_L4_Cap_Fault_బిట్లు_ఎడమ);
        ప్రతిజ్ఞా(స్థానభ్రంశం + 2 == se_L4_Cap_Fault_లోతు_Mismatch_బిట్లు_దొరికింది);
        ప్రతిజ్ఞా(స్థానభ్రంశం + 2 == se_L4_Cap_Fault_Guard_Mismatch_Guard_దొరికింది);
        ప్రతిజ్ఞా(స్థానభ్రంశం + 3 == se_L4_Cap_Fault_Guard_Mismatch_బిట్లు_దొరికింది);
    } లేకపోతే {
        ప్రతిజ్ఞా(స్థానభ్రంశం == 1);
    }

    ఎంపిక (lufType) {
    స్థితి lookup_fault_invalid_మూలం:
        తిరిగి i;

    స్థితి lookup_fault_missing_capability:
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం + 1,
                     lookup_fault_missing_capability_get_బిట్లు_ఎడమ(luf));

    స్థితి lookup_fault_లోతు_mismatch:
        సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం + 1,
              lookup_fault_లోతు_mismatch_get_బిట్లు_ఎడమ(luf));
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం + 2,
                     lookup_fault_లోతు_mismatch_get_బిట్లు_దొరికింది(luf));

    స్థితి lookup_fault_guard_mismatch:
        సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం + 1,
              lookup_fault_guard_mismatch_get_బిట్లు_ఎడమ(luf));
        సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం + 2,
              lookup_fault_guard_mismatch_get_guard_దొరికింది(luf));
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, స్థానభ్రంశం + 3,
                     lookup_fault_guard_mismatch_get_బిట్లు_దొరికింది(luf));

    అప్రమేయ:
        fail("Invalid lookup failure");
    }
}

స్థిర అంతఃవరుస శూన్య copy_MRs_Fault_జవాబు(tcb_t *sender, tcb_t *receiver, సందేశం_ID_t id, మాట_t నిడివి)
{
    మాట_t i;
    bool_t archInfo;

    archInfo = Arch_get_Sanitise_రిజిస్టరు_Info(receiver);

    కొరకు (i = 0; i < MIN(నిడివి, n_సందేశం_Registers); i++) {
        రిజిస్టరు_t r = fault_messages[id][i];
        మాట_t v = get_రిజిస్టరు(sender, సందేశం_Registers[i]);
        సమితి_రిజిస్టరు(receiver, r, sanitise_రిజిస్టరు(r, v, archInfo));
    }

    ఒకవేళ (i < నిడివి) {
        మాట_t *sendBuf = lookupIPCBuffer(false, sender);
        ఒకవేళ (sendBuf) {
            కొరకు (; i < నిడివి; i++) {
                రిజిస్టరు_t r = fault_messages[id][i];
                మాట_t v = sendBuf[i + 1];
                సమితి_రిజిస్టరు(receiver, r, sanitise_రిజిస్టరు(r, v, archInfo));
            }
        }
    }
}

స్థిర అంతఃవరుస శూన్య copyMRsFault(tcb_t *sender, tcb_t *receiver, సందేశం_ID_t id,
                                మాట_t నిడివి, మాట_t *receiveIPCBuffer)
{
    మాట_t i;
    కొరకు (i = 0; i < MIN(నిడివి, n_సందేశం_Registers); i++) {
        సమితి_రిజిస్టరు(receiver, సందేశం_Registers[i], get_రిజిస్టరు(sender, fault_messages[id][i]));
    }

    ఒకవేళ (receiveIPCBuffer) {
        కొరకు (; i < నిడివి; i++) {
            receiveIPCBuffer[i + 1] = get_రిజిస్టరు(sender, fault_messages[id][i]);
        }
    }
}

bool_t handle_Fault_జవాబు(tcb_t *receiver, tcb_t *sender)
{
    /* These lookups are moved inward from doReplyTransfer */
    se_L4_సందేశం_Info_t ట్యాగ్ = సందేశం_Info_From_మాట(get_రిజిస్టరు(sender, సందేశం_Info_రిజిస్టరు));
    మాట_t label = se_L4_సందేశం_Info_get_label(ట్యాగ్);
    మాట_t నిడివి = se_L4_సందేశం_Info_get_నిడివి(ట్యాగ్);
    seL4_Fault_t fault = receiver->tcbFault;

    ఎంపిక (seL4_Fault_get_seL4_FaultType(fault)) {
    స్థితి seL4_Fault_CapFault:
        తిరిగి true;

    స్థితి seL4_Fault_UnknownSyscall:
        copy_MRs_Fault_జవాబు(sender, receiver, సందేశం_ID_Syscall, MIN(నిడివి, n_syscall_సందేశం));
        తిరిగి (label == 0);

    స్థితి seL4_Fault_UserException:
        copy_MRs_Fault_జవాబు(sender, receiver, సందేశం_ID_Exception, MIN(నిడివి, n_exception_సందేశం));
        తిరిగి (label == 0);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి seL4_Fault_Timeout:
        copy_MRs_Fault_జవాబు(sender, receiver, సందేశం_ID_Timeout_జవాబు, MIN(నిడివి, n_timeout_సందేశం));
        తిరిగి (label == 0);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    స్థితి seL4_Fault_DebugException: {
        మాట_t n_instrs;

        ఒకవేళ (seL4_Fault_DebugException_get_exceptionReason(fault) != se_L4_Single_అడుగు) {
            /* Only single-step replies are required to set message registers.
             */
            తిరిగి (label == 0);
        }

        ఒకవేళ (నిడివి < DEBUG_జవాబు_N_EXPECTED_REGISTERS) {
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
        } లేకపోతే {
            /* If the reply had all expected registers set, proceed as normal */
            n_instrs = get_రిజిస్టరు(sender, సందేశం_Registers[0]);
        }

        syscall_error_t res;

        res = Arch_decodeConfigureSingleStepping(receiver, 0, n_instrs, true);
        ఒకవేళ (res.type != seL4_NoError) {
            తిరిగి false;
        };

        configureSingleStepping(receiver, 0, n_instrs, true);

        /* Replying will always resume the thread: the only variant behaviour
         * is whether or not the thread will be resumed with stepping still
         * enabled.
         */
        తిరిగి (label == 0);
    }
#అయితే_ముగింపు

    అప్రమేయ:
        తిరిగి Arch_handle_Fault_జవాబు(receiver, sender, seL4_Fault_get_seL4_FaultType(fault));
    }
}

మాట_t సమితి_MRs_fault(tcb_t *sender, tcb_t *receiver, మాట_t *receiveIPCBuffer)
{
    ఎంపిక (seL4_Fault_get_seL4_FaultType(sender->tcbFault)) {
    స్థితి seL4_Fault_CapFault:
        సమితి_MR(receiver, receiveIPCBuffer, seL4_CapFault_IP, getRestartPC(sender));
        సమితి_MR(receiver, receiveIPCBuffer, seL4_CapFault_Addr,
              se_L4_Fault_Cap_Fault_get_చిరునామా(sender->tcbFault));
        సమితి_MR(receiver, receiveIPCBuffer, se_L4_Cap_Fault_లోపల_Recv_Phase,
              se_L4_Fault_Cap_Fault_get_లోపల_Receive_Phase(sender->tcbFault));
        తిరిగి సమితి_MRs_lookup_failure(receiver, receiveIPCBuffer,
                                     sender->tcbLookupFailure, seL4_CapFault_LookupFailureType);

    స్థితి seL4_Fault_UnknownSyscall: {
        copyMRsFault(sender, receiver, సందేశం_ID_Syscall, n_syscall_సందేశం,
                     receiveIPCBuffer);

        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, n_syscall_సందేశం,
                     se_L4_Fault_Unknown_Syscall_get_syscall_సంఖ్య(sender->tcbFault));
    }

    స్థితి seL4_Fault_UserException: {
        copyMRsFault(sender, receiver, సందేశం_ID_Exception,
                     n_exception_సందేశం, receiveIPCBuffer);
        సమితి_MR(receiver, receiveIPCBuffer, n_exception_సందేశం,
              se_L4_Fault_User_Exception_get_సంఖ్య(sender->tcbFault));
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, n_exception_సందేశం + 1u,
                     se_L4_Fault_User_Exception_get_సంకేతం(sender->tcbFault));
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి seL4_Fault_Timeout: {
        ఒకవేళ (sender->tcbSchedContext) {
            మాట_t len;
            సమయం_t consumed = schedContext_updateConsumed(sender->tcbSchedContext);
            len = సమితి_MR(receiver, receiveIPCBuffer, se_L4_Timeout_దత్తాంశం,
                        seL4_Fault_Timeout_get_badge(sender->tcbFault));
            తిరిగి బహుళకం_సమితి_సమయం_Arg(len, consumed, receiveIPCBuffer, receiver);
        } లేకపోతే {
            తిరిగి సమితి_MR(receiver, receiveIPCBuffer, se_L4_Timeout_దత్తాంశం,
                         seL4_Fault_Timeout_get_badge(sender->tcbFault));
        }
    }
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    స్థితి seL4_Fault_DebugException: {
        మాట_t reason = seL4_Fault_DebugException_get_exceptionReason(sender->tcbFault);

        సమితి_MR(receiver, receiveIPCBuffer,
              seL4_DebugException_FaultIP, getRestartPC(sender));
        గుర్తులేని పూర్ణాంకం ret = సమితి_MR(receiver, receiveIPCBuffer,
                                 seL4_DebugException_ExceptionReason, reason);

        ఒకవేళ (reason != se_L4_Single_అడుగు && reason != seL4_SoftwareBreakRequest) {
            ret = సమితి_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Trigger_చిరునామా,
                        se_L4_Fault_Debug_Exception_get_breakpoint_చిరునామా(sender->tcbFault));

            /* Breakpoint messages also set a "breakpoint number" register. */
            ret = సమితి_MR(receiver, receiveIPCBuffer,
                        se_L4_Debug_Exception_Breakpoint_సంఖ్య,
                        se_L4_Fault_Debug_Exception_get_breakpoint_సంఖ్య(sender->tcbFault));
        }
        తిరిగి ret;
    }
#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */

    అప్రమేయ:
        తిరిగి Arch_సమితి_MRs_fault(sender, receiver, receiveIPCBuffer,
                                 seL4_Fault_get_seL4_FaultType(sender->tcbFault));
    }
}
