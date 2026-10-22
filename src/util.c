/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <stdint.h>
#شامل <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
تعریفنوع unsigned long __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
تهی memzero(تهی *s, unsigned long n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    أكد((unsigned long)s % اندازه(unsigned long) == 0);
    أكد(n % اندازه(unsigned long) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    وقتی (n != 0) {
        *(ulong_alias *)p = 0;
        p += اندازه(ulong_alias);
        n -= اندازه(ulong_alias);
    }
}

تهی *VISIBLE memset(تهی *s, unsigned long c, unsigned long n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    اگر (likely(c == 0 && ((unsigned long)s % اندازه(unsigned long)) == 0 && (n % اندازه(unsigned long)) == 0)) {
        memzero(s, n);
    } وگرنه {
        /* Otherwise, we use a slower, simple memset. */
        برای (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    بازگشت s;
}

تهی *VISIBLE memcpy(تهی *ptr_dst, ثابت تهی *ptr_src, unsigned long n)
{
    uint8_t *p;
    ثابت uint8_t *q;

    برای (p = (uint8_t *)ptr_dst, q = (ثابت uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    بازگشت ptr_dst;
}

صحیح PURE strncmp(ثابت نویسه *s1, ثابت نویسه *s2, صحیح n)
{
    word_t i;
    صحیح diff;

    برای (i = 0; i < n; i++) {
        diff = ((unsigned نویسه *)s1)[i] - ((unsigned نویسه *)s2)[i];
        اگر (diff != 0 || s1[i] == '\0') {
            بازگشت diff;
        }
    }

    بازگشت 0;
}

long CONST char_to_long(نویسه c)
{
    اگر (c >= '0' && c <= '9') {
        بازگشت c - '0';
    } وگرنه اگر (c >= 'A' && c <= 'F') {
        بازگشت c - 'A' + 10;
    } وگرنه اگر (c >= 'a' && c <= 'f') {
        بازگشت c - 'a' + 10;
    }
    بازگشت -1;
}

long PURE str_to_long(ثابت نویسه *str)
{
    unsigned صحیح base;
    long res;
    long val = 0;
    نویسه c;

    /* NULL ptr and empty str */
    اگر (str == پوچ || *str == 0) {
        بازگشت -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    اگر (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        base = 16;
        str += 2;
        /* '0x' on its own is malformed */
        اگر (*str == 0) {
            بازگشت -1;
        }
    } وگرنه {
        base = 10;
    }

    c = *str;
    وقتی (c != '\0') {
        res = char_to_long(c);
        اگر (res == -1 || res >= base) {
            بازگشت -1;
        }
        val = val * base + res;
        str++;
        c = *str;
    }

    بازگشت val;
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
compile_assert(clz_ulong_32_or_64, اندازه(unsigned long) == 4 || اندازه(unsigned long) == 8);
compile_assert(clz_ullong_64, اندازه(unsigned long long) == 8);
compile_assert(clz_word_size, اندازه(unsigned long) * 8 == CONFIG_WORD_SIZE);

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
ایستا UNUSED CONST درونخطی unsigned clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    unsigned شمار = 32;
    uint32_t mask = UINT32_MAX;

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
    اگر (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        unsigned bits = ((unsigned)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x0000ffff
        شمار -= bits; // [16, 32]
    }
    اگر (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        unsigned bits = ((unsigned)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x000000ff
        شمار -= bits; // [8, 16, 24, 32]
    }
    اگر (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        unsigned bits = ((unsigned)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x0000000f
        شمار -= bits; // [4, 8, 12, ..., 32]
    }
    اگر (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        unsigned bits = ((unsigned)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x00000003
        شمار -= bits; // [2, 4, 6, ..., 32]
    }
    اگر (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        unsigned bits = ((unsigned)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x00000001
        شمار -= bits; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    بازگشت شمار - x;
}

ایستا UNUSED CONST درونخطی unsigned clz64(uint64_t x)
{
    unsigned شمار = 64;
    uint64_t mask = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    اگر (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        unsigned bits = ((unsigned)(mask < x)) << 5; // [0, 32]
        x >>= bits; // <= 0x00000000ffffffff
        شمار -= bits; // [32, 64]
    }
    اگر (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        unsigned bits = ((unsigned)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x000000000000ffff
        شمار -= bits; // [16, 32, 48, 64]
    }
    اگر (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        unsigned bits = ((unsigned)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x00000000000000ff
        شمار -= bits; // [8, 16, 24, ..., 64]
    }
    اگر (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        unsigned bits = ((unsigned)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x000000000000000f
        شمار -= bits; // [4, 8, 12, ..., 64]
    }
    اگر (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        unsigned bits = ((unsigned)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x0000000000000003
        شمار -= bits; // [2, 4, 6, ..., 64]
    }
    اگر (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        unsigned bits = ((unsigned)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x0000000000000001
        شمار -= bits; // [1, 2, 3, ..., 64]
    }

    بازگشت شمار - x;
}

// Count trailing zeros.
// See comments on clz32.
ایستا UNUSED CONST درونخطی unsigned ctz32(uint32_t x)
{
    unsigned شمار = (x == 0);
    uint32_t mask = UINT32_MAX;

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

    اگر (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x0000ffff != 0
        شمار += bits; // if xi != 0 then [0, 16] else 17
    }
    اگر (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x000000ff != 0
        شمار += bits; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    اگر (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        unsigned bits = ((unsigned)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x0000000f != 0
        شمار += bits; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    اگر (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        unsigned bits = ((unsigned)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x00000003 != 0
        شمار += bits; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    اگر (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        unsigned bits = ((unsigned)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x00000001 != 0
        شمار += bits; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    بازگشت شمار;
}

ایستا UNUSED CONST درونخطی unsigned ctz64(uint64_t x)
{
    unsigned شمار = (x == 0);
    uint64_t mask = UINT64_MAX;

    اگر (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 5; // [0, 32]
        x >>= bits; // xi != 0 --> x & 0x00000000ffffffff != 0
        شمار += bits; // if xi != 0 then [0, 32] else 33
    }
    اگر (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x000000000000ffff != 0
        شمار += bits; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    اگر (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x00000000000000ff != 0
        شمار += bits; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    اگر (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        unsigned bits = ((unsigned)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x000000000000000f != 0
        شمار += bits; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    اگر (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        unsigned bits = ((unsigned)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x0000000000000003 != 0
        شمار += bits; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    اگر (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        unsigned bits = ((unsigned)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x0000000000000001 != 0
        شمار += bits; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    بازگشت شمار;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#ifdef CONFIG_CLZ_32
CONST صحیح __clzsi2(uint32_t x)
{
    بازگشت clz32(x);
}
#endif

#ifdef CONFIG_CLZ_64
CONST صحیح __clzdi2(uint64_t x)
{
    بازگشت clz64(x);
}
#endif

#ifdef CONFIG_CTZ_32
CONST صحیح __ctzsi2(uint32_t x)
{
    بازگشت ctz32(x);
}
#endif

#ifdef CONFIG_CTZ_64
CONST صحیح __ctzdi2(uint64_t x)
{
    بازگشت ctz64(x);
}
#endif
