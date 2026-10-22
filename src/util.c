/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <stdint.h>
#ضمن <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
تعريفنوع بنانشانی لما __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
فراغ memzero(فراغ *s, بنانشانی لما n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    أكد((بنانشانی لما)s % حجم(بنانشانی لما) == 0);
    أكد(n % حجم(بنانشانی لما) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    طالما (n != 0) {
        *(ulong_alias *)p = 0;
        p += حجم(ulong_alias);
        n -= حجم(ulong_alias);
    }
}

فراغ *VISIBLE memset(فراغ *s, بنانشانی لما c, بنانشانی لما n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    إذا (likely(c == 0 && ((بنانشانی لما)s % حجم(بنانشانی لما)) == 0 && (n % حجم(بنانشانی لما)) == 0)) {
        memzero(s, n);
    } وإلا {
        /* Otherwise, we use a slower, simple memset. */
        لكل (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    أرجع s;
}

فراغ *VISIBLE حافظہ_نقل(فراغ *ptr_dst, ثابت فراغ *ptr_src, بنانشانی لما n)
{
    uint8_t *p;
    ثابت uint8_t *q;

    لكل (p = (uint8_t *)ptr_dst, q = (ثابت uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    أرجع ptr_dst;
}

صحيح خالص لڑی_مقابلہ(ثابت حرف *s1, ثابت حرف *s2, صحيح n)
{
    لفظ_t i;
    صحيح diff;

    لكل (i = 0; i < n; i++) {
        diff = ((بنانشانی حرف *)s1)[i] - ((بنانشانی حرف *)s2)[i];
        إذا (diff != 0 || s1[i] == '\0') {
            أرجع diff;
        }
    }

    أرجع 0;
}

لما CONST char_منزل_long(حرف c)
{
    إذا (c >= '0' && c <= '9') {
        أرجع c - '0';
    } وإلا إذا (c >= 'A' && c <= 'F') {
        أرجع c - 'A' + 10;
    } وإلا إذا (c >= 'a' && c <= 'f') {
        أرجع c - 'a' + 10;
    }
    أرجع -1;
}

لما خالص str_منزل_long(ثابت حرف *str)
{
    بنانشانی صحيح ادھار;
    لما res;
    لما val = 0;
    حرف c;

    /* NULL ptr and empty str */
    إذا (str == عدم || *str == 0) {
        أرجع -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    إذا (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        ادھار = 16;
        str += 2;
        /* '0x' on its own is malformed */
        إذا (*str == 0) {
            أرجع -1;
        }
    } وإلا {
        ادھار = 10;
    }

    c = *str;
    طالما (c != '\0') {
        res = char_منزل_long(c);
        إذا (res == -1 || res >= ادھار) {
            أرجع -1;
        }
        val = val * ادھار + res;
        str++;
        c = *str;
    }

    أرجع val;
}

// The following implementations of CLZ (count leading zeros) and CTZ (count
// trailing zeros) perform a binary search for the first 1 bit from the
// beginning (resp. end) of the input. Initially, the focus is the whole input.
// Then, each iteration determines whether there are any 1 bits set in the
// upper (resp. lower) half of the current focus. If there are (resp. are not),
// then the upper half is shifted into the lower half. Either way, the lower
// half of the current focus becomes the new focus for the next iteration.
// After enough iterations (6 for 64-bit inputs, 5 for 32-bit inputs), the
// focus is reduced to a single bit, and the total number of bits shifted can
// be used to determine the number of zeros before (resp. after) the first 1
// bit.
//
// Although the details vary, the general approach is used in several library
// implementations, including in LLVM and GCC. Wikipedia has some references:
// https://en.wikipedia.org/wiki/Find_first_set
//
// The current implementation avoids branching. The test that determines
// whether the upper (resp. lower) half contains any ones directly produces a
// number which can be used for an unconditional shift. If the upper (resp.
// lower) half is all zeros, the test produces a zero, and the shift is a
// no-op. A branchless implementation has the disadvantage that it requires
// more instructions to execute than one which branches, but the advantage is
// that none will be mispredicted branches. Whether this is a good tradeoff
// depends on the branch predictability and the architecture's pipeline depth.
// The most critical use of clzl in the kernel is in the scheduler priority
// queue. In the absence of a concrete application and hardware implementation
// to evaluate the tradeoff, we somewhat arbitrarily choose a branchless
// implementation. In any case, the compiler might convert this to a branching
// binary.

// Check some assumptions made by the clzl, clzll, ctzl functions:
compile_assert(clz_ulong_32_or_64, حجم(بنانشانی لما) == 4 || حجم(بنانشانی لما) == 8);
compile_assert(clz_ullong_64, حجم(بنانشانی لما لما) == 8);
compile_assert(clz_لفظ_ناپ, حجم(بنانشانی لما) * 8 == CONFIG_لفظ_ناپ);

// Count leading zeros.
// This implementation contains no branches. If the architecture provides an
// instruction to set a register to a boolean value on a comparison, then the
// binary might also avoid branching. A branchless implementation might be
// preferable on architectures with deep pipelines, or when the maximum
// priority of runnable threads frequently varies. However, note that the
// compiler may choose to convert this to a branching implementation.
//
// These functions are potentially `UNUSED` because we want to always expose
// them to verification without necessarily linking them into the kernel
// binary.
ساكن بن_ورتیا CONST مضمن بنانشانی clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    بنانشانی گنتی = 32;
    uint32_t نقاب_2 = UINT32_MAX;

    // Each iteration i (counting backwards) considers the least significant
    // 2^(i+1) bits of x as the current focus. At the first iteration, the
    // focus is the whole input. Each iteration assumes x contains no 1 bits
    // outside its focus. The iteration contains a test which determines
    // whether there are any 1 bits in the upper half (2^i bits) of the focus,
    // setting `bits` to 2^i if there are, or zero if not. Shifting by `bits`
    // then narrows the focus to the lower 2^i bits and satisfies the
    // assumption for the next iteration. After the final iteration, the focus
    // is just the least significant bit, and the most significsnt 1 bit of the
    // original input (if any) has been shifted into this position. The leading
    // zero count can be determined from the total shift.
    //
    // The iterations are given a very regular structure to facilitate proofs,
    // while also generating reasonably efficient binary code.

    // The `if (1)` blocks make it easier to reason by chunks in the proofs.
    إذا (1) {
        // iteration 4
        نقاب_2 >>= (1 << 4); // 0x0000ffff
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 4; // [0, 16]
        x >>= بٹ; // <= 0x0000ffff
        گنتی -= بٹ; // [16, 32]
    }
    إذا (1) {
        // iteration 3
        نقاب_2 >>= (1 << 3); // 0x000000ff
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 3; // [0, 8]
        x >>= بٹ; // <= 0x000000ff
        گنتی -= بٹ; // [8, 16, 24, 32]
    }
    إذا (1) {
        // iteration 2
        نقاب_2 >>= (1 << 2); // 0x0000000f
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 2; // [0, 4]
        x >>= بٹ; // <= 0x0000000f
        گنتی -= بٹ; // [4, 8, 12, ..., 32]
    }
    إذا (1) {
        // iteration 1
        نقاب_2 >>= (1 << 1); // 0x00000003
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 1; // [0, 2]
        x >>= بٹ; // <= 0x00000003
        گنتی -= بٹ; // [2, 4, 6, ..., 32]
    }
    إذا (1) {
        // iteration 0
        نقاب_2 >>= (1 << 0); // 0x00000001
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 0; // [0, 1]
        x >>= بٹ; // <= 0x00000001
        گنتی -= بٹ; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    أرجع گنتی - x;
}

ساكن بن_ورتیا CONST مضمن بنانشانی clz64(uint64_t x)
{
    بنانشانی گنتی = 64;
    uint64_t نقاب_2 = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    إذا (1) {
        // iteration 5
        نقاب_2 >>= (1 << 5); // 0x00000000ffffffff
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 5; // [0, 32]
        x >>= بٹ; // <= 0x00000000ffffffff
        گنتی -= بٹ; // [32, 64]
    }
    إذا (1) {
        // iteration 4
        نقاب_2 >>= (1 << 4); // 0x000000000000ffff
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 4; // [0, 16]
        x >>= بٹ; // <= 0x000000000000ffff
        گنتی -= بٹ; // [16, 32, 48, 64]
    }
    إذا (1) {
        // iteration 3
        نقاب_2 >>= (1 << 3); // 0x00000000000000ff
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 3; // [0, 8]
        x >>= بٹ; // <= 0x00000000000000ff
        گنتی -= بٹ; // [8, 16, 24, ..., 64]
    }
    إذا (1) {
        // iteration 2
        نقاب_2 >>= (1 << 2); // 0x000000000000000f
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 2; // [0, 4]
        x >>= بٹ; // <= 0x000000000000000f
        گنتی -= بٹ; // [4, 8, 12, ..., 64]
    }
    إذا (1) {
        // iteration 1
        نقاب_2 >>= (1 << 1); // 0x0000000000000003
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 1; // [0, 2]
        x >>= بٹ; // <= 0x0000000000000003
        گنتی -= بٹ; // [2, 4, 6, ..., 64]
    }
    إذا (1) {
        // iteration 0
        نقاب_2 >>= (1 << 0); // 0x0000000000000001
        بنانشانی بٹ = ((بنانشانی)(نقاب_2 < x)) << 0; // [0, 1]
        x >>= بٹ; // <= 0x0000000000000001
        گنتی -= بٹ; // [1, 2, 3, ..., 64]
    }

    أرجع گنتی - x;
}

// Count trailing zeros.
// See comments on clz32.
ساكن بن_ورتیا CONST مضمن بنانشانی ctz32(uint32_t x)
{
    بنانشانی گنتی = (x == 0);
    uint32_t نقاب_2 = UINT32_MAX;

    // Each iteration i (counting backwards) considers the least significant
    // 2^(i+1) bits of x as the current focus. At the first iteration, the
    // focus is the whole input. The iteration contains a test which determines
    // whether there are any 1 bits in the lower half (2^i bits) of the focus,
    // setting `bits` to zero if there are, or 2^i if not. Shifting by `bits`
    // then narrows the focus to the lower 2^i bits for the next iteration.
    // After the final iteration, the focus is just the least significant bit,
    // and the least significsnt 1 bit of the original input (if any) has been
    // shifted into this position. The trailing zero count can be determined
    // from the total shift.
    //
    // If the initial input is zero, every iteration causes a shift, for a
    // total shift count of 31, so in that case, we add one for a total count
    // of 32. In the comments, xi is the initial value of x.
    //
    // The iterations are given a very regular structure to facilitate proofs,
    // while also generating reasonably efficient binary code.

    إذا (1) {
        // iteration 4
        نقاب_2 >>= (1 << 4); // 0x0000ffff
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 4; // [0, 16]
        x >>= بٹ; // xi != 0 --> x & 0x0000ffff != 0
        گنتی += بٹ; // if xi != 0 then [0, 16] else 17
    }
    إذا (1) {
        // iteration 3
        نقاب_2 >>= (1 << 3); // 0x000000ff
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 3; // [0, 8]
        x >>= بٹ; // xi != 0 --> x & 0x000000ff != 0
        گنتی += بٹ; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    إذا (1) {
        // iteration 2
        نقاب_2 >>= (1 << 2); // 0x0000000f
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 2; // [0, 4]
        x >>= بٹ; // xi != 0 --> x & 0x0000000f != 0
        گنتی += بٹ; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    إذا (1) {
        // iteration 1
        نقاب_2 >>= (1 << 1); // 0x00000003
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 1; // [0, 2]
        x >>= بٹ; // xi != 0 --> x & 0x00000003 != 0
        گنتی += بٹ; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    إذا (1) {
        // iteration 0
        نقاب_2 >>= (1 << 0); // 0x00000001
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 0; // [0, 1]
        x >>= بٹ; // xi != 0 --> x & 0x00000001 != 0
        گنتی += بٹ; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    أرجع گنتی;
}

ساكن بن_ورتیا CONST مضمن بنانشانی ctz64(uint64_t x)
{
    بنانشانی گنتی = (x == 0);
    uint64_t نقاب_2 = UINT64_MAX;

    إذا (1) {
        // iteration 5
        نقاب_2 >>= (1 << 5); // 0x00000000ffffffff
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 5; // [0, 32]
        x >>= بٹ; // xi != 0 --> x & 0x00000000ffffffff != 0
        گنتی += بٹ; // if xi != 0 then [0, 32] else 33
    }
    إذا (1) {
        // iteration 4
        نقاب_2 >>= (1 << 4); // 0x000000000000ffff
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 4; // [0, 16]
        x >>= بٹ; // xi != 0 --> x & 0x000000000000ffff != 0
        گنتی += بٹ; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    إذا (1) {
        // iteration 3
        نقاب_2 >>= (1 << 3); // 0x00000000000000ff
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 3; // [0, 8]
        x >>= بٹ; // xi != 0 --> x & 0x00000000000000ff != 0
        گنتی += بٹ; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    إذا (1) {
        // iteration 2
        نقاب_2 >>= (1 << 2); // 0x000000000000000f
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 2; // [0, 4]
        x >>= بٹ; // xi != 0 --> x & 0x000000000000000f != 0
        گنتی += بٹ; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    إذا (1) {
        // iteration 1
        نقاب_2 >>= (1 << 1); // 0x0000000000000003
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 1; // [0, 2]
        x >>= بٹ; // xi != 0 --> x & 0x0000000000000003 != 0
        گنتی += بٹ; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    إذا (1) {
        // iteration 0
        نقاب_2 >>= (1 << 0); // 0x0000000000000001
        بنانشانی بٹ = ((بنانشانی)((x & نقاب_2) == 0)) << 0; // [0, 1]
        x >>= بٹ; // xi != 0 --> x & 0x0000000000000001 != 0
        گنتی += بٹ; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    أرجع گنتی;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#جے_تعریف_شدہ CONFIG_CLZ_32
CONST صحيح __clzsi2(uint32_t x)
{
    أرجع clz32(x);
}
#جے_انت

#جے_تعریف_شدہ CONFIG_CLZ_64
CONST صحيح __clzdi2(uint64_t x)
{
    أرجع clz64(x);
}
#جے_انت

#جے_تعریف_شدہ CONFIG_CTZ_32
CONST صحيح __ctzsi2(uint32_t x)
{
    أرجع ctz32(x);
}
#جے_انت

#جے_تعریف_شدہ CONFIG_CTZ_64
CONST صحيح __ctzdi2(uint64_t x)
{
    أرجع ctz64(x);
}
#جے_انت
