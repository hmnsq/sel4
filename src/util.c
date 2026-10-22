/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
ଶୂନ୍ୟ memzero(ଶୂନ୍ୟ *s, unsigned long n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    ପ୍ରତିଜ୍ଞା((unsigned long)s % ଆକାର(unsigned long) == 0);
    ପ୍ରତିଜ୍ଞା(n % ଆକାର(unsigned long) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    ଯେତେବେଳେ (n != 0) {
        *(ulong_alias *)p = 0;
        p += ଆକାର(ulong_alias);
        n -= ଆକାର(ulong_alias);
    }
}

ଶୂନ୍ୟ *VISIBLE memset(ଶୂନ୍ୟ *s, unsigned long c, unsigned long n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    ଯଦି (likely(c == 0 && ((unsigned long)s % ଆକାର(unsigned long)) == 0 && (n % ଆକାର(unsigned long)) == 0)) {
        memzero(s, n);
    } ଅନ୍ୟଥା {
        /* Otherwise, we use a slower, simple memset. */
        ପାଇଁ (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    ଫେରନ୍ତୁ s;
}

ଶୂନ୍ୟ *VISIBLE memcpy(ଶୂନ୍ୟ *ptr_dst, ଅଚଳ ଶୂନ୍ୟ *ptr_src, unsigned long n)
{
    uint8_t *p;
    ଅଚଳ uint8_t *q;

    ପାଇଁ (p = (uint8_t *)ptr_dst, q = (ଅଚଳ uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    ଫେରନ୍ତୁ ptr_dst;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା PURE strncmp(ଅଚଳ ଅକ୍ଷର *s1, ଅଚଳ ଅକ୍ଷର *s2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    word_t i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା diff;

    ପାଇଁ (i = 0; i < n; i++) {
        diff = ((unsigned ଅକ୍ଷର *)s1)[i] - ((unsigned ଅକ୍ଷର *)s2)[i];
        ଯଦି (diff != 0 || s1[i] == '\0') {
            ଫେରନ୍ତୁ diff;
        }
    }

    ଫେରନ୍ତୁ 0;
}

long CONST char_to_long(ଅକ୍ଷର c)
{
    ଯଦି (c >= '0' && c <= '9') {
        ଫେରନ୍ତୁ c - '0';
    } ଅନ୍ୟଥା ଯଦି (c >= 'A' && c <= 'F') {
        ଫେରନ୍ତୁ c - 'A' + 10;
    } ଅନ୍ୟଥା ଯଦି (c >= 'a' && c <= 'f') {
        ଫେରନ୍ତୁ c - 'a' + 10;
    }
    ଫେରନ୍ତୁ -1;
}

long PURE str_to_long(ଅଚଳ ଅକ୍ଷର *str)
{
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା base;
    long res;
    long val = 0;
    ଅକ୍ଷର c;

    /* NULL ptr and empty str */
    ଯଦି (str == ଖାଲି || *str == 0) {
        ଫେରନ୍ତୁ -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    ଯଦି (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        base = 16;
        str += 2;
        /* '0x' on its own is malformed */
        ଯଦି (*str == 0) {
            ଫେରନ୍ତୁ -1;
        }
    } ଅନ୍ୟଥା {
        base = 10;
    }

    c = *str;
    ଯେତେବେଳେ (c != '\0') {
        res = char_to_long(c);
        ଯଦି (res == -1 || res >= base) {
            ଫେରନ୍ତୁ -1;
        }
        val = val * base + res;
        str++;
        c = *str;
    }

    ଫେରନ୍ତୁ val;
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
compile_assert(clz_ulong_32_or_64, ଆକାର(unsigned long) == 4 || ଆକାର(unsigned long) == 8);
compile_assert(clz_ullong_64, ଆକାର(unsigned long long) == 8);
compile_assert(clz_word_size, ଆକାର(unsigned long) * 8 == CONFIG_WORD_SIZE);

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
ସ୍ଥିର UNUSED CONST ଧାଡ଼ିଭିତରେ unsigned clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    unsigned ଗଣନା_2 = 32;
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
    ଯଦି (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        unsigned bits = ((unsigned)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x0000ffff
        ଗଣନା_2 -= bits; // [16, 32]
    }
    ଯଦି (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        unsigned bits = ((unsigned)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x000000ff
        ଗଣନା_2 -= bits; // [8, 16, 24, 32]
    }
    ଯଦି (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        unsigned bits = ((unsigned)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x0000000f
        ଗଣନା_2 -= bits; // [4, 8, 12, ..., 32]
    }
    ଯଦି (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        unsigned bits = ((unsigned)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x00000003
        ଗଣନା_2 -= bits; // [2, 4, 6, ..., 32]
    }
    ଯଦି (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        unsigned bits = ((unsigned)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x00000001
        ଗଣନା_2 -= bits; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    ଫେରନ୍ତୁ ଗଣନା_2 - x;
}

ସ୍ଥିର UNUSED CONST ଧାଡ଼ିଭିତରେ unsigned clz64(uint64_t x)
{
    unsigned ଗଣନା_2 = 64;
    uint64_t mask = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    ଯଦି (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        unsigned bits = ((unsigned)(mask < x)) << 5; // [0, 32]
        x >>= bits; // <= 0x00000000ffffffff
        ଗଣନା_2 -= bits; // [32, 64]
    }
    ଯଦି (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        unsigned bits = ((unsigned)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x000000000000ffff
        ଗଣନା_2 -= bits; // [16, 32, 48, 64]
    }
    ଯଦି (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        unsigned bits = ((unsigned)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x00000000000000ff
        ଗଣନା_2 -= bits; // [8, 16, 24, ..., 64]
    }
    ଯଦି (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        unsigned bits = ((unsigned)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x000000000000000f
        ଗଣନା_2 -= bits; // [4, 8, 12, ..., 64]
    }
    ଯଦି (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        unsigned bits = ((unsigned)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x0000000000000003
        ଗଣନା_2 -= bits; // [2, 4, 6, ..., 64]
    }
    ଯଦି (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        unsigned bits = ((unsigned)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x0000000000000001
        ଗଣନା_2 -= bits; // [1, 2, 3, ..., 64]
    }

    ଫେରନ୍ତୁ ଗଣନା_2 - x;
}

// Count trailing zeros.
// See comments on clz32.
ସ୍ଥିର UNUSED CONST ଧାଡ଼ିଭିତରେ unsigned ctz32(uint32_t x)
{
    unsigned ଗଣନା_2 = (x == 0);
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

    ଯଦି (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x0000ffff != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 16] else 17
    }
    ଯଦି (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x000000ff != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    ଯଦି (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        unsigned bits = ((unsigned)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x0000000f != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    ଯଦି (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        unsigned bits = ((unsigned)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x00000003 != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    ଯଦି (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        unsigned bits = ((unsigned)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x00000001 != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    ଫେରନ୍ତୁ ଗଣନା_2;
}

ସ୍ଥିର UNUSED CONST ଧାଡ଼ିଭିତରେ unsigned ctz64(uint64_t x)
{
    unsigned ଗଣନା_2 = (x == 0);
    uint64_t mask = UINT64_MAX;

    ଯଦି (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 5; // [0, 32]
        x >>= bits; // xi != 0 --> x & 0x00000000ffffffff != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 32] else 33
    }
    ଯଦି (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x000000000000ffff != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    ଯଦି (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x00000000000000ff != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    ଯଦି (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        unsigned bits = ((unsigned)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x000000000000000f != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    ଯଦି (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        unsigned bits = ((unsigned)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x0000000000000003 != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    ଯଦି (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        unsigned bits = ((unsigned)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x0000000000000001 != 0
        ଗଣନା_2 += bits; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    ଫେରନ୍ତୁ ଗଣନା_2;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#ifdef CONFIG_CLZ_32
CONST ପୂର୍ଣ୍ଣସଂଖ୍ୟା __clzsi2(uint32_t x)
{
    ଫେରନ୍ତୁ clz32(x);
}
#endif

#ifdef CONFIG_CLZ_64
CONST ପୂର୍ଣ୍ଣସଂଖ୍ୟା __clzdi2(uint64_t x)
{
    ଫେରନ୍ତୁ clz64(x);
}
#endif

#ifdef CONFIG_CTZ_32
CONST ପୂର୍ଣ୍ଣସଂଖ୍ୟା __ctzsi2(uint32_t x)
{
    ଫେରନ୍ତୁ ctz32(x);
}
#endif

#ifdef CONFIG_CTZ_64
CONST ପୂର୍ଣ୍ଣସଂଖ୍ୟା __ctzdi2(uint64_t x)
{
    ଫେରନ୍ତୁ ctz64(x);
}
#endif
