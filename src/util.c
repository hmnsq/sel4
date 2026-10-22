/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
ਖਾਲੀ memzero(ਖਾਲੀ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    ਪ੍ਰਤਿਜ੍ਞਾ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)s % ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) == 0);
    ਪ੍ਰਤਿਜ੍ਞਾ(n % ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    ਜਦੋਂ (n != 0) {
        *(ulong_alias *)p = 0;
        p += ਆਕਾਰ(ulong_alias);
        n -= ਆਕਾਰ(ulong_alias);
    }
}

ਖਾਲੀ *VISIBLE memset(ਖਾਲੀ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ c, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    ਜੇ (likely(c == 0 && ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)s % ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) == 0 && (n % ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)) == 0)) {
        memzero(s, n);
    } ਨਹੀਂਤਾਂ {
        /* Otherwise, we use a slower, simple memset. */
        ਲਈ (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    ਵਾਪਸ s;
}

ਖਾਲੀ *VISIBLE ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਖਾਲੀ *ptr_dst, ਅਚਲ ਖਾਲੀ *ptr_src, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ n)
{
    uint8_t *p;
    ਅਚਲ uint8_t *q;

    ਲਈ (p = (uint8_t *)ptr_dst, q = (ਅਚਲ uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    ਵਾਪਸ ptr_dst;
}

ਪੂਰਨ ਸ਼ੁੱਧ ਲੜੀ_ਤੁਲਨਾ(ਅਚਲ ਅੱਖਰ *s1, ਅਚਲ ਅੱਖਰ *s2, ਪੂਰਨ n)
{
    ਸ਼ਬਦ_t i;
    ਪੂਰਨ diff;

    ਲਈ (i = 0; i < n; i++) {
        diff = ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)s1)[i] - ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)s2)[i];
        ਜੇ (diff != 0 || s1[i] == '\0') {
            ਵਾਪਸ diff;
        }
    }

    ਵਾਪਸ 0;
}

ਲੰਮਾ CONST char_ਮੰਜ਼ਿਲ_long(ਅੱਖਰ c)
{
    ਜੇ (c >= '0' && c <= '9') {
        ਵਾਪਸ c - '0';
    } ਨਹੀਂਤਾਂ ਜੇ (c >= 'A' && c <= 'F') {
        ਵਾਪਸ c - 'A' + 10;
    } ਨਹੀਂਤਾਂ ਜੇ (c >= 'a' && c <= 'f') {
        ਵਾਪਸ c - 'a' + 10;
    }
    ਵਾਪਸ -1;
}

ਲੰਮਾ ਸ਼ੁੱਧ str_ਮੰਜ਼ਿਲ_long(ਅਚਲ ਅੱਖਰ *str)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਅਧਾਰ;
    ਲੰਮਾ res;
    ਲੰਮਾ val = 0;
    ਅੱਖਰ c;

    /* NULL ptr and empty str */
    ਜੇ (str == ਨਿਰਾ || *str == 0) {
        ਵਾਪਸ -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    ਜੇ (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        ਅਧਾਰ = 16;
        str += 2;
        /* '0x' on its own is malformed */
        ਜੇ (*str == 0) {
            ਵਾਪਸ -1;
        }
    } ਨਹੀਂਤਾਂ {
        ਅਧਾਰ = 10;
    }

    c = *str;
    ਜਦੋਂ (c != '\0') {
        res = char_ਮੰਜ਼ਿਲ_long(c);
        ਜੇ (res == -1 || res >= ਅਧਾਰ) {
            ਵਾਪਸ -1;
        }
        val = val * ਅਧਾਰ + res;
        str++;
        c = *str;
    }

    ਵਾਪਸ val;
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
compile_assert(clz_ulong_32_or_64, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) == 4 || ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) == 8);
compile_assert(clz_ullong_64, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ) == 8);
compile_assert(clz_ਸ਼ਬਦ_ਮਾਪ, ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) * 8 == CONFIG_ਸ਼ਬਦ_ਮਾਪ);

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
ਸਥਿਰ ਅਣਵਰਤਿਆ CONST ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2 = 32;
    uint32_t ਨਕਾਬ_2 = UINT32_MAX;

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
    ਜੇ (1) {
        // iteration 4
        ਨਕਾਬ_2 >>= (1 << 4); // 0x0000ffff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 4; // [0, 16]
        x >>= ਬਿੱਟ; // <= 0x0000ffff
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [16, 32]
    }
    ਜੇ (1) {
        // iteration 3
        ਨਕਾਬ_2 >>= (1 << 3); // 0x000000ff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 3; // [0, 8]
        x >>= ਬਿੱਟ; // <= 0x000000ff
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [8, 16, 24, 32]
    }
    ਜੇ (1) {
        // iteration 2
        ਨਕਾਬ_2 >>= (1 << 2); // 0x0000000f
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 2; // [0, 4]
        x >>= ਬਿੱਟ; // <= 0x0000000f
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [4, 8, 12, ..., 32]
    }
    ਜੇ (1) {
        // iteration 1
        ਨਕਾਬ_2 >>= (1 << 1); // 0x00000003
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 1; // [0, 2]
        x >>= ਬਿੱਟ; // <= 0x00000003
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [2, 4, 6, ..., 32]
    }
    ਜੇ (1) {
        // iteration 0
        ਨਕਾਬ_2 >>= (1 << 0); // 0x00000001
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 0; // [0, 1]
        x >>= ਬਿੱਟ; // <= 0x00000001
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    ਵਾਪਸ ਗਿਣਤੀ_2 - x;
}

ਸਥਿਰ ਅਣਵਰਤਿਆ CONST ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ clz64(uint64_t x)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2 = 64;
    uint64_t ਨਕਾਬ_2 = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    ਜੇ (1) {
        // iteration 5
        ਨਕਾਬ_2 >>= (1 << 5); // 0x00000000ffffffff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 5; // [0, 32]
        x >>= ਬਿੱਟ; // <= 0x00000000ffffffff
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [32, 64]
    }
    ਜੇ (1) {
        // iteration 4
        ਨਕਾਬ_2 >>= (1 << 4); // 0x000000000000ffff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 4; // [0, 16]
        x >>= ਬਿੱਟ; // <= 0x000000000000ffff
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [16, 32, 48, 64]
    }
    ਜੇ (1) {
        // iteration 3
        ਨਕਾਬ_2 >>= (1 << 3); // 0x00000000000000ff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 3; // [0, 8]
        x >>= ਬਿੱਟ; // <= 0x00000000000000ff
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [8, 16, 24, ..., 64]
    }
    ਜੇ (1) {
        // iteration 2
        ਨਕਾਬ_2 >>= (1 << 2); // 0x000000000000000f
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 2; // [0, 4]
        x >>= ਬਿੱਟ; // <= 0x000000000000000f
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [4, 8, 12, ..., 64]
    }
    ਜੇ (1) {
        // iteration 1
        ਨਕਾਬ_2 >>= (1 << 1); // 0x0000000000000003
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 1; // [0, 2]
        x >>= ਬਿੱਟ; // <= 0x0000000000000003
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [2, 4, 6, ..., 64]
    }
    ਜੇ (1) {
        // iteration 0
        ਨਕਾਬ_2 >>= (1 << 0); // 0x0000000000000001
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(ਨਕਾਬ_2 < x)) << 0; // [0, 1]
        x >>= ਬਿੱਟ; // <= 0x0000000000000001
        ਗਿਣਤੀ_2 -= ਬਿੱਟ; // [1, 2, 3, ..., 64]
    }

    ਵਾਪਸ ਗਿਣਤੀ_2 - x;
}

// Count trailing zeros.
// See comments on clz32.
ਸਥਿਰ ਅਣਵਰਤਿਆ CONST ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ctz32(uint32_t x)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2 = (x == 0);
    uint32_t ਨਕਾਬ_2 = UINT32_MAX;

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

    ਜੇ (1) {
        // iteration 4
        ਨਕਾਬ_2 >>= (1 << 4); // 0x0000ffff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 4; // [0, 16]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x0000ffff != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 16] else 17
    }
    ਜੇ (1) {
        // iteration 3
        ਨਕਾਬ_2 >>= (1 << 3); // 0x000000ff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 3; // [0, 8]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x000000ff != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    ਜੇ (1) {
        // iteration 2
        ਨਕਾਬ_2 >>= (1 << 2); // 0x0000000f
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 2; // [0, 4]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x0000000f != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    ਜੇ (1) {
        // iteration 1
        ਨਕਾਬ_2 >>= (1 << 1); // 0x00000003
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 1; // [0, 2]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x00000003 != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    ਜੇ (1) {
        // iteration 0
        ਨਕਾਬ_2 >>= (1 << 0); // 0x00000001
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 0; // [0, 1]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x00000001 != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    ਵਾਪਸ ਗਿਣਤੀ_2;
}

ਸਥਿਰ ਅਣਵਰਤਿਆ CONST ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ctz64(uint64_t x)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਗਿਣਤੀ_2 = (x == 0);
    uint64_t ਨਕਾਬ_2 = UINT64_MAX;

    ਜੇ (1) {
        // iteration 5
        ਨਕਾਬ_2 >>= (1 << 5); // 0x00000000ffffffff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 5; // [0, 32]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x00000000ffffffff != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 32] else 33
    }
    ਜੇ (1) {
        // iteration 4
        ਨਕਾਬ_2 >>= (1 << 4); // 0x000000000000ffff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 4; // [0, 16]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x000000000000ffff != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    ਜੇ (1) {
        // iteration 3
        ਨਕਾਬ_2 >>= (1 << 3); // 0x00000000000000ff
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 3; // [0, 8]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x00000000000000ff != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    ਜੇ (1) {
        // iteration 2
        ਨਕਾਬ_2 >>= (1 << 2); // 0x000000000000000f
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 2; // [0, 4]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x000000000000000f != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    ਜੇ (1) {
        // iteration 1
        ਨਕਾਬ_2 >>= (1 << 1); // 0x0000000000000003
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 1; // [0, 2]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x0000000000000003 != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    ਜੇ (1) {
        // iteration 0
        ਨਕਾਬ_2 >>= (1 << 0); // 0x0000000000000001
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਿੱਟ = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)((x & ਨਕਾਬ_2) == 0)) << 0; // [0, 1]
        x >>= ਬਿੱਟ; // xi != 0 --> x & 0x0000000000000001 != 0
        ਗਿਣਤੀ_2 += ਬਿੱਟ; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    ਵਾਪਸ ਗਿਣਤੀ_2;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_CLZ_32
CONST ਪੂਰਨ __clzsi2(uint32_t x)
{
    ਵਾਪਸ clz32(x);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_CLZ_64
CONST ਪੂਰਨ __clzdi2(uint64_t x)
{
    ਵਾਪਸ clz64(x);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_CTZ_32
CONST ਪੂਰਨ __ctzsi2(uint32_t x)
{
    ਵਾਪਸ ctz32(x);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_CTZ_64
CONST ਪੂਰਨ __ctzdi2(uint64_t x)
{
    ਵਾਪਸ ctz64(x);
}
#ਜੇ_ਅੰਤ
