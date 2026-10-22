/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/object/ioport.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <plat/machine/io.h>

స్థిర అంతఃవరుస శూన్య apply_నమూనా(మాట_t_may_alias *w, మాట_t నమూనా, bool_t సమితి)
{
    ఒకవేళ (సమితి) {
        *w |= నమూనా;
    } లేకపోతే {
        *w &= ~నమూనా;
    }
}

స్థిర అంతఃవరుస మాట_t make_నమూనా(పూర్ణాంకం ప్రారంభం, పూర్ణాంకం చివర)
{
    // number of bits we want to have set
    పూర్ణాంకం అంకె_బిట్లు = చివర - ప్రారంభం;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    తిరిగి (~(మాట_t)0) >> (CONFIG_మాట_కొలత - అంకె_బిట్లు) << ప్రారంభం;
}

స్థిర exception_t ensure_పోర్టు_Operation_Allowed(cap_t cap, uint32_t ప్రారంభం_పోర్టు, uint32_t కొలత)
{
    uint32_t మొదటి_allowed = cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap);
    uint32_t చివరి_allowed = cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap);
    uint32_t చివర_పోర్టు = ప్రారంభం_పోర్టు + కొలత - 1;
    ప్రతిజ్ఞా(మొదటి_allowed <= చివరి_allowed);
    ప్రతిజ్ఞా(ప్రారంభం_పోర్టు <= చివర_పోర్టు);

    ఒకవేళ ((ప్రారంభం_పోర్టు < మొదటి_allowed) || (చివర_పోర్టు > చివరి_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (పూర్ణాంకం)ప్రారంభం_పోర్టు, (పూర్ణాంకం)చివర_పోర్టు,
                  (పూర్ణాంకం)మొదటి_allowed, (పూర్ణాంకం)చివరి_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

శూన్య freeIOPortRange(uint16_t మొదటి_పోర్టు, uint16_t చివరి_పోర్టు)
{
    సమితి_IOPort_ముసుగు(x86KSAllocatedIOPorts, మొదటి_పోర్టు, చివరి_పోర్టు, false);
}

స్థిర bool_t isIOPortRangeFree(uint16_t మొదటి_పోర్టు, uint16_t చివరి_పోర్టు)
{
    పూర్ణాంకం దిగువ_మాట = మొదటి_పోర్టు >> మాట_Radix;
    పూర్ణాంకం ఎగువ_మాట = చివరి_పోర్టు >> మాట_Radix;
    పూర్ణాంకం దిగువ_సూచీ = మొదటి_పోర్టు & ముసుగు(మాట_Radix);
    పూర్ణాంకం ఎగువ_సూచీ = చివరి_పోర్టు & ముసుగు(మాట_Radix);

    // check if we are operating on a partial word
    ఒకవేళ (దిగువ_మాట == ఎగువ_మాట) {
        ఒకవేళ ((x86KSAllocatedIOPorts[దిగువ_మాట] & make_నమూనా(దిగువ_సూచీ, ఎగువ_సూచీ + 1)) != 0) {
            తిరిగి false;
        }
        తిరిగి true;
    }
    // check the starting word
    ఒకవేళ ((x86KSAllocatedIOPorts[దిగువ_మాట] & make_నమూనా(దిగువ_సూచీ, CONFIG_మాట_కొలత)) != 0) {
        తిరిగి false;
    }
    దిగువ_మాట++;
    // check the rest of the whole words
    అయితే (దిగువ_మాట < ఎగువ_మాట) {
        ఒకవేళ (x86KSAllocatedIOPorts[దిగువ_మాట] != 0) {
            తిరిగి false;
        }
        దిగువ_మాట++;
    }
    // check any trailing bits
    ఒకవేళ ((x86KSAllocatedIOPorts[దిగువ_మాట] & make_నమూనా(0, ఎగువ_సూచీ + 1)) != 0) {
        తిరిగి false;
    }
    తిరిగి true;
}

స్థిర exception_t invoke_X86_పోర్టు_Control(uint16_t మొదటి_పోర్టు, uint16_t చివరి_పోర్టు, cte_t *ioport_స్థానం, cte_t *control_స్థానం)
{
    సమితి_IOPort_ముసుగు(x86KSAllocatedIOPorts, మొదటి_పోర్టు, చివరి_పోర్టు, true);
    cte_చొప్పించు(cap_io_పోర్టు_cap_new(మొదటి_పోర్టు, చివరి_పోర్టు
#నిర్వచితమైతే CONFIG_VTX
                                  , VPID_INVALID
#అయితే_ముగింపు
                                 ),
              control_స్థానం, ioport_స్థానం);

    తిరిగి EXCEPTION_NONE;
}

exception_t decode_X86_పోర్టు_Control_Invocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    మాట_t *బఫరు
)
{
    uint16_t మొదటి_పోర్టు;
    uint16_t చివరి_పోర్టు;
    మాట_t సూచీ, లోతు;
    cap_t cnodeCap;
    cte_t *dest_స్థానం;
    lookup_స్థానం_ret_t lu_ret;
    exception_t స్థితి_2;

    ఒకవేళ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 4 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మొదటి_పోర్టు = getSyscallArg(0, బఫరు) & 0xffff;
    చివరి_పోర్టు = getSyscallArg(1, బఫరు) & 0xffff;
    సూచీ = getSyscallArg(2, బఫరు);
    లోతు = getSyscallArg(3, బఫరు);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (చివరి_పోర్టు < మొదటి_పోర్టు) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (!isIOPortRangeFree(మొదటి_పోర్టు, చివరి_పోర్టు)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(బఫరు, 0));
        తిరిగి lu_ret.స్థితి_2;
    }
    dest_స్థానం = lu_ret.స్థానం;

    స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(బఫరు, 0));
        తిరిగి స్థితి_2;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invoke_X86_పోర్టు_Control(మొదటి_పోర్టు, చివరి_పోర్టు, dest_స్థానం, స్థానం);
}

స్థిర exception_t invoke_X86_పోర్టు_లోపల(మాట_t invLabel, uint16_t పోర్టు, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);

    ఎంపిక (invLabel) {
    స్థితి X86IOPortIn8:
        res = in8(పోర్టు);
        ఆపు;
    స్థితి X86IOPortIn16:
        res = in16(పోర్టు);
        ఆపు;
    స్థితి X86IOPortIn32:
        res = in32(పోర్టు);
        ఆపు;
    }

    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, ipc_బఫరు, 0, res);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t invoke_X86_పోర్టు_బహిర్గతం(మాట_t invLabel, uint16_t పోర్టు, uint32_t దత్తాంశం)
{
    ఎంపిక (invLabel) {
    స్థితి X86IOPortOut8:
        out8(పోర్టు, దత్తాంశం);
        ఆపు;
    స్థితి X86IOPortOut16:
        out16(పోర్టు, దత్తాంశం);
        ఆపు;
    స్థితి X86IOPortOut32:
        out32(పోర్టు, దత్తాంశం);
        ఆపు;
    }

    తిరిగి EXCEPTION_NONE;
}

exception_t decode_X86_పోర్టు_Invocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    exception_t ret = EXCEPTION_NONE;

    ఒకవేళ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ఒకవేళ (నిడివి < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t పోర్టు = getSyscallArg(0, బఫరు) & 0xffff;
        ఎంపిక (invLabel) {
        స్థితి X86IOPortIn8:
            ret = ensure_పోర్టు_Operation_Allowed(cap, పోర్టు, 1);
            ఆపు;
        స్థితి X86IOPortIn16:
            ret = ensure_పోర్టు_Operation_Allowed(cap, పోర్టు, 2);
            ఆపు;
        స్థితి X86IOPortIn32:
            ret = ensure_పోర్టు_Operation_Allowed(cap, పోర్టు, 4);
            ఆపు;
        }
        ఒకవేళ (ret != EXCEPTION_NONE) {
            తిరిగి ret;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invoke_X86_పోర్టు_లోపల(invLabel, పోర్టు, call);
    } లేకపోతే ఒకవేళ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ఒకవేళ (నిడివి < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t పోర్టు = getSyscallArg(0, బఫరు) & 0xffff;
        se_L4_మాట raw_దత్తాంశం = getSyscallArg(1, బఫరు);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t దత్తాంశం = 0;

        ఎంపిక (invLabel) {
        స్థితి X86IOPortOut8:
            ret = ensure_పోర్టు_Operation_Allowed(cap, పోర్టు, 1);
            దత్తాంశం = raw_దత్తాంశం & 0xff;
            ఆపు;
        స్థితి X86IOPortOut16:
            ret = ensure_పోర్టు_Operation_Allowed(cap, పోర్టు, 2);
            దత్తాంశం = raw_దత్తాంశం & 0xffff;
            ఆపు;
        స్థితి X86IOPortOut32:
            ret = ensure_పోర్టు_Operation_Allowed(cap, పోర్టు, 4);
            దత్తాంశం = raw_దత్తాంశం & 0xffffffff;
            ఆపు;
        }
        ఒకవేళ (ret != EXCEPTION_NONE) {
            తిరిగి ret;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invoke_X86_పోర్టు_బహిర్గతం(invLabel, పోర్టు, దత్తాంశం);
    } లేకపోతే {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

శూన్య సమితి_IOPort_ముసుగు(శూన్య *ioport_bitmap, uint16_t దిగువ, uint16_t ఎగువ, bool_t సమితి)
{
    //get an aliasing pointer
    మాట_t_may_alias *bitmap = ioport_bitmap;

    పూర్ణాంకం దిగువ_మాట = దిగువ >> మాట_Radix;
    పూర్ణాంకం ఎగువ_మాట = ఎగువ >> మాట_Radix;
    పూర్ణాంకం దిగువ_సూచీ = దిగువ & ముసుగు(మాట_Radix);
    పూర్ణాంకం ఎగువ_సూచీ = ఎగువ & ముసుగు(మాట_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ఒకవేళ (దిగువ_మాట == ఎగువ_మాట) {
        apply_నమూనా(bitmap + దిగువ_మాట, make_నమూనా(దిగువ_సూచీ, ఎగువ_సూచీ + 1), సమితి);
    } లేకపోతే {
        // operate on the potentially partial first word
        apply_నమూనా(bitmap + దిగువ_మాట, make_నమూనా(దిగువ_సూచీ, CONFIG_మాట_కొలత), సమితి);
        దిగువ_మాట++;
        // iterate over the whole words
        అయితే (దిగువ_మాట < ఎగువ_మాట) {
            apply_నమూనా(bitmap + దిగువ_మాట, ~(మాట_t)0, సమితి);
            దిగువ_మాట++;
        }
        // apply to any remaining bits
        apply_నమూనా(bitmap + దిగువ_మాట, make_నమూనా(0, ఎగువ_సూచీ + 1), సమితి);
    }
}
