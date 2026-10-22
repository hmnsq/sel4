/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
തരനിർവചനം unsigned long __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
ശൂന്യം memzero(ശൂന്യം *s, unsigned long n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    പ്രതിജ്ഞാ((unsigned long)s % വലുപ്പം(unsigned long) == 0);
    പ്രതിജ്ഞാ(n % വലുപ്പം(unsigned long) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    ആകുമ്പോൾ (n != 0) {
        *(ulong_alias *)p = 0;
        p += വലുപ്പം(ulong_alias);
        n -= വലുപ്പം(ulong_alias);
    }
}

ശൂന്യം *VISIBLE memset(ശൂന്യം *s, unsigned long c, unsigned long n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    എങ്കിൽ (likely(c == 0 && ((unsigned long)s % വലുപ്പം(unsigned long)) == 0 && (n % വലുപ്പം(unsigned long)) == 0)) {
        memzero(s, n);
    } അല്ലെങ്കിൽ {
        /* Otherwise, we use a slower, simple memset. */
        വേണ്ടി (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    തിരികെ s;
}

ശൂന്യം *VISIBLE memcpy(ശൂന്യം *ptr_dst, സ്ഥിരം ശൂന്യം *ptr_src, unsigned long n)
{
    uint8_t *p;
    സ്ഥിരം uint8_t *q;

    വേണ്ടി (p = (uint8_t *)ptr_dst, q = (സ്ഥിരം uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    തിരികെ ptr_dst;
}

പൂർണ്ണസംഖ്യ PURE strncmp(സ്ഥിരം അക്ഷരം *s1, സ്ഥിരം അക്ഷരം *s2, പൂർണ്ണസംഖ്യ n)
{
    word_t i;
    പൂർണ്ണസംഖ്യ diff;

    വേണ്ടി (i = 0; i < n; i++) {
        diff = ((unsigned അക്ഷരം *)s1)[i] - ((unsigned അക്ഷരം *)s2)[i];
        എങ്കിൽ (diff != 0 || s1[i] == '\0') {
            തിരികെ diff;
        }
    }

    തിരികെ 0;
}

long CONST char_to_long(അക്ഷരം c)
{
    എങ്കിൽ (c >= '0' && c <= '9') {
        തിരികെ c - '0';
    } അല്ലെങ്കിൽ എങ്കിൽ (c >= 'A' && c <= 'F') {
        തിരികെ c - 'A' + 10;
    } അല്ലെങ്കിൽ എങ്കിൽ (c >= 'a' && c <= 'f') {
        തിരികെ c - 'a' + 10;
    }
    തിരികെ -1;
}

long PURE str_to_long(സ്ഥിരം അക്ഷരം *str)
{
    unsigned പൂർണ്ണസംഖ്യ base;
    long res;
    long val = 0;
    അക്ഷരം c;

    /* NULL ptr and empty str */
    എങ്കിൽ (str == ശൂന്യ || *str == 0) {
        തിരികെ -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    എങ്കിൽ (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        base = 16;
        str += 2;
        /* '0x' on its own is malformed */
        എങ്കിൽ (*str == 0) {
            തിരികെ -1;
        }
    } അല്ലെങ്കിൽ {
        base = 10;
    }

    c = *str;
    ആകുമ്പോൾ (c != '\0') {
        res = char_to_long(c);
        എങ്കിൽ (res == -1 || res >= base) {
            തിരികെ -1;
        }
        val = val * base + res;
        str++;
        c = *str;
    }

    തിരികെ val;
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
compile_assert(clz_ulong_32_or_64, വലുപ്പം(unsigned long) == 4 || വലുപ്പം(unsigned long) == 8);
compile_assert(clz_ullong_64, വലുപ്പം(unsigned long long) == 8);
compile_assert(clz_word_size, വലുപ്പം(unsigned long) * 8 == CONFIG_WORD_SIZE);

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
സ്ഥിതം UNUSED CONST വരിയിൽ unsigned clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    unsigned എണ്ണം = 32;
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
    എങ്കിൽ (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        unsigned bits = ((unsigned)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x0000ffff
        എണ്ണം -= bits; // [16, 32]
    }
    എങ്കിൽ (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        unsigned bits = ((unsigned)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x000000ff
        എണ്ണം -= bits; // [8, 16, 24, 32]
    }
    എങ്കിൽ (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        unsigned bits = ((unsigned)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x0000000f
        എണ്ണം -= bits; // [4, 8, 12, ..., 32]
    }
    എങ്കിൽ (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        unsigned bits = ((unsigned)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x00000003
        എണ്ണം -= bits; // [2, 4, 6, ..., 32]
    }
    എങ്കിൽ (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        unsigned bits = ((unsigned)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x00000001
        എണ്ണം -= bits; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    തിരികെ എണ്ണം - x;
}

സ്ഥിതം UNUSED CONST വരിയിൽ unsigned clz64(uint64_t x)
{
    unsigned എണ്ണം = 64;
    uint64_t mask = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    എങ്കിൽ (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        unsigned bits = ((unsigned)(mask < x)) << 5; // [0, 32]
        x >>= bits; // <= 0x00000000ffffffff
        എണ്ണം -= bits; // [32, 64]
    }
    എങ്കിൽ (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        unsigned bits = ((unsigned)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x000000000000ffff
        എണ്ണം -= bits; // [16, 32, 48, 64]
    }
    എങ്കിൽ (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        unsigned bits = ((unsigned)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x00000000000000ff
        എണ്ണം -= bits; // [8, 16, 24, ..., 64]
    }
    എങ്കിൽ (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        unsigned bits = ((unsigned)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x000000000000000f
        എണ്ണം -= bits; // [4, 8, 12, ..., 64]
    }
    എങ്കിൽ (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        unsigned bits = ((unsigned)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x0000000000000003
        എണ്ണം -= bits; // [2, 4, 6, ..., 64]
    }
    എങ്കിൽ (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        unsigned bits = ((unsigned)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x0000000000000001
        എണ്ണം -= bits; // [1, 2, 3, ..., 64]
    }

    തിരികെ എണ്ണം - x;
}

// Count trailing zeros.
// See comments on clz32.
സ്ഥിതം UNUSED CONST വരിയിൽ unsigned ctz32(uint32_t x)
{
    unsigned എണ്ണം = (x == 0);
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

    എങ്കിൽ (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x0000ffff != 0
        എണ്ണം += bits; // if xi != 0 then [0, 16] else 17
    }
    എങ്കിൽ (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x000000ff != 0
        എണ്ണം += bits; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    എങ്കിൽ (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        unsigned bits = ((unsigned)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x0000000f != 0
        എണ്ണം += bits; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    എങ്കിൽ (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        unsigned bits = ((unsigned)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x00000003 != 0
        എണ്ണം += bits; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    എങ്കിൽ (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        unsigned bits = ((unsigned)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x00000001 != 0
        എണ്ണം += bits; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    തിരികെ എണ്ണം;
}

സ്ഥിതം UNUSED CONST വരിയിൽ unsigned ctz64(uint64_t x)
{
    unsigned എണ്ണം = (x == 0);
    uint64_t mask = UINT64_MAX;

    എങ്കിൽ (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 5; // [0, 32]
        x >>= bits; // xi != 0 --> x & 0x00000000ffffffff != 0
        എണ്ണം += bits; // if xi != 0 then [0, 32] else 33
    }
    എങ്കിൽ (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x000000000000ffff != 0
        എണ്ണം += bits; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    എങ്കിൽ (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        unsigned bits = ((unsigned)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x00000000000000ff != 0
        എണ്ണം += bits; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    എങ്കിൽ (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        unsigned bits = ((unsigned)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x000000000000000f != 0
        എണ്ണം += bits; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    എങ്കിൽ (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        unsigned bits = ((unsigned)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x0000000000000003 != 0
        എണ്ണം += bits; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    എങ്കിൽ (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        unsigned bits = ((unsigned)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x0000000000000001 != 0
        എണ്ണം += bits; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    തിരികെ എണ്ണം;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#ifdef CONFIG_CLZ_32
CONST പൂർണ്ണസംഖ്യ __clzsi2(uint32_t x)
{
    തിരികെ clz32(x);
}
#endif

#ifdef CONFIG_CLZ_64
CONST പൂർണ്ണസംഖ്യ __clzdi2(uint64_t x)
{
    തിരികെ clz64(x);
}
#endif

#ifdef CONFIG_CTZ_32
CONST പൂർണ്ണസംഖ്യ __ctzsi2(uint32_t x)
{
    തിരികെ ctz32(x);
}
#endif

#ifdef CONFIG_CTZ_64
CONST പൂർണ്ണസംഖ്യ __ctzdi2(uint64_t x)
{
    തിരികെ ctz64(x);
}
#endif
