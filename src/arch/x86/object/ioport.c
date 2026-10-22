/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/thread.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <arch/object/ioport.h>
#ضمن <arch/api/invocation.h>
#ضمن <plat/machine/io.h>

ساكن مضمن فراغ apply_نمونو(لفظ_t_may_alias *w, لفظ_t نمونو, bool_t سيٽ)
{
    إذا (سيٽ) {
        *w |= نمونو;
    } وإلا {
        *w &= ~نمونو;
    }
}

ساكن مضمن لفظ_t make_نمونو(صحيح شروعات, صحيح پڇاڙي)
{
    // number of bits we want to have set
    صحيح انگ_بٽ = پڇاڙي - شروعات;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    أرجع (~(لفظ_t)0) >> (CONFIG_لفظ_سائيز - انگ_بٽ) << شروعات;
}

ساكن exception_t ensure_پورٽ_Operation_Allowed(cap_t cap, uint32_t شروعات_پورٽ, uint32_t سائيز)
{
    uint32_t پهريون_allowed = cap_io_پورٽ_cap_get_cap_IOPort_پهريون_پورٽ(cap);
    uint32_t آخري_allowed = cap_io_پورٽ_cap_get_cap_IOPort_آخري_پورٽ(cap);
    uint32_t پڇاڙي_پورٽ = شروعات_پورٽ + سائيز - 1;
    أكد(پهريون_allowed <= آخري_allowed);
    أكد(شروعات_پورٽ <= پڇاڙي_پورٽ);

    إذا ((شروعات_پورٽ < پهريون_allowed) || (پڇاڙي_پورٽ > آخري_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحيح)شروعات_پورٽ, (صحيح)پڇاڙي_پورٽ,
                  (صحيح)پهريون_allowed, (صحيح)آخري_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

فراغ freeIOPortRange(uint16_t پهريون_پورٽ, uint16_t آخري_پورٽ)
{
    سيٽ_IOPort_نقاب(x86KSAllocatedIOPorts, پهريون_پورٽ, آخري_پورٽ, false);
}

ساكن bool_t isIOPortRangeFree(uint16_t پهريون_پورٽ, uint16_t آخري_پورٽ)
{
    صحيح هيٺيون_لفظ = پهريون_پورٽ >> لفظ_Radix;
    صحيح مٿاهون_لفظ = آخري_پورٽ >> لفظ_Radix;
    صحيح هيٺيون_اشاريو = پهريون_پورٽ & نقاب(لفظ_Radix);
    صحيح مٿاهون_اشاريو = آخري_پورٽ & نقاب(لفظ_Radix);

    // check if we are operating on a partial word
    إذا (هيٺيون_لفظ == مٿاهون_لفظ) {
        إذا ((x86KSAllocatedIOPorts[هيٺيون_لفظ] & make_نمونو(هيٺيون_اشاريو, مٿاهون_اشاريو + 1)) != 0) {
            أرجع false;
        }
        أرجع true;
    }
    // check the starting word
    إذا ((x86KSAllocatedIOPorts[هيٺيون_لفظ] & make_نمونو(هيٺيون_اشاريو, CONFIG_لفظ_سائيز)) != 0) {
        أرجع false;
    }
    هيٺيون_لفظ++;
    // check the rest of the whole words
    طالما (هيٺيون_لفظ < مٿاهون_لفظ) {
        إذا (x86KSAllocatedIOPorts[هيٺيون_لفظ] != 0) {
            أرجع false;
        }
        هيٺيون_لفظ++;
    }
    // check any trailing bits
    إذا ((x86KSAllocatedIOPorts[هيٺيون_لفظ] & make_نمونو(0, مٿاهون_اشاريو + 1)) != 0) {
        أرجع false;
    }
    أرجع true;
}

ساكن exception_t invoke_X86_پورٽ_Control(uint16_t پهريون_پورٽ, uint16_t آخري_پورٽ, cte_t *ioport_جڳهه, cte_t *control_جڳهه)
{
    سيٽ_IOPort_نقاب(x86KSAllocatedIOPorts, پهريون_پورٽ, آخري_پورٽ, true);
    cte_وجهو(cap_io_پورٽ_cap_new(پهريون_پورٽ, آخري_پورٽ
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
                                  , VPID_INVALID
#جيڪڏهن_پورو
                                 ),
              control_جڳهه, ioport_جڳهه);

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورٽ_Control_Invocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *جڳهه,
    cap_t cap,
    لفظ_t *بفر
)
{
    uint16_t پهريون_پورٽ;
    uint16_t آخري_پورٽ;
    لفظ_t اشاريو, اونهائي;
    cap_t cnodeCap;
    cte_t *dest_جڳهه;
    lookup_جڳهه_ret_t lu_ret;
    exception_t حالت;

    إذا (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < 4 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پهريون_پورٽ = getSyscallArg(0, بفر) & 0xffff;
    آخري_پورٽ = getSyscallArg(1, بفر) & 0xffff;
    اشاريو = getSyscallArg(2, بفر);
    اونهائي = getSyscallArg(3, بفر);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (آخري_پورٽ < پهريون_پورٽ) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPortRangeFree(پهريون_پورٽ, آخري_پورٽ)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_هدف_جڳهه(cnodeCap, اشاريو, اونهائي);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(بفر, 0));
        أرجع lu_ret.حالت;
    }
    dest_جڳهه = lu_ret.جڳهه;

    حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(بفر, 0));
        أرجع حالت;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_X86_پورٽ_Control(پهريون_پورٽ, آخري_پورٽ, dest_جڳهه, جڳهه);
}

ساكن exception_t invoke_X86_پورٽ_اندريون(لفظ_t invLabel, uint16_t پورٽ, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);

    بدل (invLabel) {
    حالة X86IOPortIn8:
        res = in8(پورٽ);
        اكسر;
    حالة X86IOPortIn16:
        res = in16(پورٽ);
        اكسر;
    حالة X86IOPortIn32:
        res = in32(پورٽ);
        اكسر;
    }

    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, ipc_بفر, 0, res);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t invoke_X86_پورٽ_ٻاهريون(لفظ_t invLabel, uint16_t پورٽ, uint32_t معلومات)
{
    بدل (invLabel) {
    حالة X86IOPortOut8:
        out8(پورٽ, معلومات);
        اكسر;
    حالة X86IOPortOut16:
        out16(پورٽ, معلومات);
        اكسر;
    حالة X86IOPortOut32:
        out32(پورٽ, معلومات);
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورٽ_Invocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *جڳهه,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    exception_t ret = EXCEPTION_NONE;

    إذا (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        إذا (ڊيگهائي < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t پورٽ = getSyscallArg(0, بفر) & 0xffff;
        بدل (invLabel) {
        حالة X86IOPortIn8:
            ret = ensure_پورٽ_Operation_Allowed(cap, پورٽ, 1);
            اكسر;
        حالة X86IOPortIn16:
            ret = ensure_پورٽ_Operation_Allowed(cap, پورٽ, 2);
            اكسر;
        حالة X86IOPortIn32:
            ret = ensure_پورٽ_Operation_Allowed(cap, پورٽ, 4);
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورٽ_اندريون(invLabel, پورٽ, call);
    } وإلا إذا (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        إذا (ڊيگهائي < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t پورٽ = getSyscallArg(0, بفر) & 0xffff;
        se_L4_لفظ raw_معلومات = getSyscallArg(1, بفر);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t معلومات = 0;

        بدل (invLabel) {
        حالة X86IOPortOut8:
            ret = ensure_پورٽ_Operation_Allowed(cap, پورٽ, 1);
            معلومات = raw_معلومات & 0xff;
            اكسر;
        حالة X86IOPortOut16:
            ret = ensure_پورٽ_Operation_Allowed(cap, پورٽ, 2);
            معلومات = raw_معلومات & 0xffff;
            اكسر;
        حالة X86IOPortOut32:
            ret = ensure_پورٽ_Operation_Allowed(cap, پورٽ, 4);
            معلومات = raw_معلومات & 0xffffffff;
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورٽ_ٻاهريون(invLabel, پورٽ, معلومات);
    } وإلا {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ سيٽ_IOPort_نقاب(فراغ *ioport_bitmap, uint16_t هيٺيون, uint16_t مٿاهون, bool_t سيٽ)
{
    //get an aliasing pointer
    لفظ_t_may_alias *bitmap = ioport_bitmap;

    صحيح هيٺيون_لفظ = هيٺيون >> لفظ_Radix;
    صحيح مٿاهون_لفظ = مٿاهون >> لفظ_Radix;
    صحيح هيٺيون_اشاريو = هيٺيون & نقاب(لفظ_Radix);
    صحيح مٿاهون_اشاريو = مٿاهون & نقاب(لفظ_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    إذا (هيٺيون_لفظ == مٿاهون_لفظ) {
        apply_نمونو(bitmap + هيٺيون_لفظ, make_نمونو(هيٺيون_اشاريو, مٿاهون_اشاريو + 1), سيٽ);
    } وإلا {
        // operate on the potentially partial first word
        apply_نمونو(bitmap + هيٺيون_لفظ, make_نمونو(هيٺيون_اشاريو, CONFIG_لفظ_سائيز), سيٽ);
        هيٺيون_لفظ++;
        // iterate over the whole words
        طالما (هيٺيون_لفظ < مٿاهون_لفظ) {
            apply_نمونو(bitmap + هيٺيون_لفظ, ~(لفظ_t)0, سيٽ);
            هيٺيون_لفظ++;
        }
        // apply to any remaining bits
        apply_نمونو(bitmap + هيٺيون_لفظ, make_نمونو(0, مٿاهون_اشاريو + 1), سيٽ);
    }
}
