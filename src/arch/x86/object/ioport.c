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

ساكن مضمن فراغ apply_نمونہ(لفظ_t_may_alias *w, لفظ_t نمونہ, bool_t سیٹ)
{
    إذا (سیٹ) {
        *w |= نمونہ;
    } وإلا {
        *w &= ~نمونہ;
    }
}

ساكن مضمن لفظ_t make_نمونہ(صحيح شروع, صحيح سرا)
{
    // number of bits we want to have set
    صحيح عدد_بٹ = سرا - شروع;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    أرجع (~(لفظ_t)0) >> (CONFIG_لفظ_ناپ - عدد_بٹ) << شروع;
}

ساكن exception_t ensure_پورٹ_Operation_Allowed(cap_t cap, uint32_t شروع_پورٹ, uint32_t ناپ)
{
    uint32_t پہلا_allowed = cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap);
    uint32_t آخری_allowed = cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap);
    uint32_t سرا_پورٹ = شروع_پورٹ + ناپ - 1;
    أكد(پہلا_allowed <= آخری_allowed);
    أكد(شروع_پورٹ <= سرا_پورٹ);

    إذا ((شروع_پورٹ < پہلا_allowed) || (سرا_پورٹ > آخری_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحيح)شروع_پورٹ, (صحيح)سرا_پورٹ,
                  (صحيح)پہلا_allowed, (صحيح)آخری_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

فراغ freeIOPortRange(uint16_t پہلا_پورٹ, uint16_t آخری_پورٹ)
{
    سیٹ_IOPort_نقاب(x86KSAllocatedIOPorts, پہلا_پورٹ, آخری_پورٹ, false);
}

ساكن bool_t isIOPortRangeFree(uint16_t پہلا_پورٹ, uint16_t آخری_پورٹ)
{
    صحيح کم_لفظ = پہلا_پورٹ >> لفظ_Radix;
    صحيح بلند_لفظ = آخری_پورٹ >> لفظ_Radix;
    صحيح کم_اشاریہ = پہلا_پورٹ & نقاب(لفظ_Radix);
    صحيح بلند_اشاریہ = آخری_پورٹ & نقاب(لفظ_Radix);

    // check if we are operating on a partial word
    إذا (کم_لفظ == بلند_لفظ) {
        إذا ((x86KSAllocatedIOPorts[کم_لفظ] & make_نمونہ(کم_اشاریہ, بلند_اشاریہ + 1)) != 0) {
            أرجع false;
        }
        أرجع true;
    }
    // check the starting word
    إذا ((x86KSAllocatedIOPorts[کم_لفظ] & make_نمونہ(کم_اشاریہ, CONFIG_لفظ_ناپ)) != 0) {
        أرجع false;
    }
    کم_لفظ++;
    // check the rest of the whole words
    طالما (کم_لفظ < بلند_لفظ) {
        إذا (x86KSAllocatedIOPorts[کم_لفظ] != 0) {
            أرجع false;
        }
        کم_لفظ++;
    }
    // check any trailing bits
    إذا ((x86KSAllocatedIOPorts[کم_لفظ] & make_نمونہ(0, بلند_اشاریہ + 1)) != 0) {
        أرجع false;
    }
    أرجع true;
}

ساكن exception_t invoke_X86_پورٹ_Control(uint16_t پہلا_پورٹ, uint16_t آخری_پورٹ, cte_t *ioport_تھاں, cte_t *control_تھاں)
{
    سیٹ_IOPort_نقاب(x86KSAllocatedIOPorts, پہلا_پورٹ, آخری_پورٹ, true);
    cte_داخل_کرو(cap_io_پورٹ_cap_new(پہلا_پورٹ, آخری_پورٹ
#جے_تعریف_شدہ CONFIG_VTX
                                  , VPID_INVALID
#جے_انت
                                 ),
              control_تھاں, ioport_تھاں);

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورٹ_Control_Invocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cptr_t cptr,
    cte_t *تھاں,
    cap_t cap,
    لفظ_t *بفر
)
{
    uint16_t پہلا_پورٹ;
    uint16_t آخری_پورٹ;
    لفظ_t اشاریہ, ڈونگھائی;
    cap_t cnodeCap;
    cte_t *dest_تھاں;
    lookup_تھاں_ret_t lu_ret;
    exception_t حالت;

    إذا (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (لمائی < 4 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پہلا_پورٹ = getSyscallArg(0, بفر) & 0xffff;
    آخری_پورٹ = getSyscallArg(1, بفر) & 0xffff;
    اشاریہ = getSyscallArg(2, بفر);
    ڈونگھائی = getSyscallArg(3, بفر);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (آخری_پورٹ < پہلا_پورٹ) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPortRangeFree(پہلا_پورٹ, آخری_پورٹ)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(بفر, 0));
        أرجع lu_ret.حالت;
    }
    dest_تھاں = lu_ret.تھاں;

    حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(بفر, 0));
        أرجع حالت;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_X86_پورٹ_Control(پہلا_پورٹ, آخری_پورٹ, dest_تھاں, تھاں);
}

ساكن exception_t invoke_X86_پورٹ_وچ(لفظ_t invLabel, uint16_t پورٹ, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);

    بدل (invLabel) {
    حالة X86IOPortIn8:
        res = in8(پورٹ);
        اكسر;
    حالة X86IOPortIn16:
        res = in16(پورٹ);
        اكسر;
    حالة X86IOPortIn32:
        res = in32(پورٹ);
        اكسر;
    }

    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        بنانشانی صحيح لمائی = سیٹ_MR(thread, ipc_بفر, 0, res);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, لمائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t invoke_X86_پورٹ_باہرلا(لفظ_t invLabel, uint16_t پورٹ, uint32_t انکڑے)
{
    بدل (invLabel) {
    حالة X86IOPortOut8:
        out8(پورٹ, انکڑے);
        اكسر;
    حالة X86IOPortOut16:
        out16(پورٹ, انکڑے);
        اكسر;
    حالة X86IOPortOut32:
        out32(پورٹ, انکڑے);
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورٹ_Invocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cptr_t cptr,
    cte_t *تھاں,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    exception_t ret = EXCEPTION_NONE;

    إذا (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        إذا (لمائی < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t پورٹ = getSyscallArg(0, بفر) & 0xffff;
        بدل (invLabel) {
        حالة X86IOPortIn8:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 1);
            اكسر;
        حالة X86IOPortIn16:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 2);
            اكسر;
        حالة X86IOPortIn32:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 4);
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورٹ_وچ(invLabel, پورٹ, call);
    } وإلا إذا (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        إذا (لمائی < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t پورٹ = getSyscallArg(0, بفر) & 0xffff;
        se_L4_لفظ raw_انکڑے = getSyscallArg(1, بفر);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t انکڑے = 0;

        بدل (invLabel) {
        حالة X86IOPortOut8:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 1);
            انکڑے = raw_انکڑے & 0xff;
            اكسر;
        حالة X86IOPortOut16:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 2);
            انکڑے = raw_انکڑے & 0xffff;
            اكسر;
        حالة X86IOPortOut32:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 4);
            انکڑے = raw_انکڑے & 0xffffffff;
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورٹ_باہرلا(invLabel, پورٹ, انکڑے);
    } وإلا {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ سیٹ_IOPort_نقاب(فراغ *ioport_bitmap, uint16_t کم, uint16_t بلند, bool_t سیٹ)
{
    //get an aliasing pointer
    لفظ_t_may_alias *bitmap = ioport_bitmap;

    صحيح کم_لفظ = کم >> لفظ_Radix;
    صحيح بلند_لفظ = بلند >> لفظ_Radix;
    صحيح کم_اشاریہ = کم & نقاب(لفظ_Radix);
    صحيح بلند_اشاریہ = بلند & نقاب(لفظ_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    إذا (کم_لفظ == بلند_لفظ) {
        apply_نمونہ(bitmap + کم_لفظ, make_نمونہ(کم_اشاریہ, بلند_اشاریہ + 1), سیٹ);
    } وإلا {
        // operate on the potentially partial first word
        apply_نمونہ(bitmap + کم_لفظ, make_نمونہ(کم_اشاریہ, CONFIG_لفظ_ناپ), سیٹ);
        کم_لفظ++;
        // iterate over the whole words
        طالما (کم_لفظ < بلند_لفظ) {
            apply_نمونہ(bitmap + کم_لفظ, ~(لفظ_t)0, سیٹ);
            کم_لفظ++;
        }
        // apply to any remaining bits
        apply_نمونہ(bitmap + کم_لفظ, make_نمونہ(0, بلند_اشاریہ + 1), سیٹ);
    }
}
