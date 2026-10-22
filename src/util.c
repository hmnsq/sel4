/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
ಶೂನ್ಯ memzero(ಶೂನ್ಯ *s, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    ಪ್ರತಿಜ್ಞಾ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)s % ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) == 0);
    ಪ್ರತಿಜ್ಞಾ(n % ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    ಆಗ (n != 0) {
        *(ulong_alias *)p = 0;
        p += ಗಾತ್ರ(ulong_alias);
        n -= ಗಾತ್ರ(ulong_alias);
    }
}

ಶೂನ್ಯ *VISIBLE memset(ಶೂನ್ಯ *s, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ c, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    ಆದರೆ (likely(c == 0 && ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)s % ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) == 0 && (n % ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)) == 0)) {
        memzero(s, n);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Otherwise, we use a slower, simple memset. */
        ಗಾಗಿ (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    ಹಿಂತಿರುಗಿ s;
}

ಶೂನ್ಯ *VISIBLE ಸ್ಮರಣೆ_ನಕಲು(ಶೂನ್ಯ *ptr_dst, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr_src, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ n)
{
    uint8_t *p;
    ಸ್ಥಿರಾಂಕ uint8_t *q;

    ಗಾಗಿ (p = (uint8_t *)ptr_dst, q = (ಸ್ಥಿರಾಂಕ uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    ಹಿಂತಿರುಗಿ ptr_dst;
}

ಪೂರ್ಣಾಂಕ ಶುದ್ಧ ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s2, ಪೂರ್ಣಾಂಕ n)
{
    ಶಬ್ದ_t i;
    ಪೂರ್ಣಾಂಕ diff;

    ಗಾಗಿ (i = 0; i < n; i++) {
        diff = ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)s1)[i] - ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)s2)[i];
        ಆದರೆ (diff != 0 || s1[i] == '\0') {
            ಹಿಂತಿರುಗಿ diff;
        }
    }

    ಹಿಂತಿರುಗಿ 0;
}

ದೀರ್ಘ CONST char_ಗಮ್ಯ_long(ಅಕ್ಷರ c)
{
    ಆದರೆ (c >= '0' && c <= '9') {
        ಹಿಂತಿರುಗಿ c - '0';
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= 'A' && c <= 'F') {
        ಹಿಂತಿರುಗಿ c - 'A' + 10;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c >= 'a' && c <= 'f') {
        ಹಿಂತಿರುಗಿ c - 'a' + 10;
    }
    ಹಿಂತಿರುಗಿ -1;
}

ದೀರ್ಘ ಶುದ್ಧ str_ಗಮ್ಯ_long(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ತಳ;
    ದೀರ್ಘ res;
    ದೀರ್ಘ val = 0;
    ಅಕ್ಷರ c;

    /* NULL ptr and empty str */
    ಆದರೆ (str == ಖಾಲಿ || *str == 0) {
        ಹಿಂತಿರುಗಿ -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    ಆದರೆ (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        ತಳ = 16;
        str += 2;
        /* '0x' on its own is malformed */
        ಆದರೆ (*str == 0) {
            ಹಿಂತಿರುಗಿ -1;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ತಳ = 10;
    }

    c = *str;
    ಆಗ (c != '\0') {
        res = char_ಗಮ್ಯ_long(c);
        ಆದರೆ (res == -1 || res >= ತಳ) {
            ಹಿಂತಿರುಗಿ -1;
        }
        val = val * ತಳ + res;
        str++;
        c = *str;
    }

    ಹಿಂತಿರುಗಿ val;
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
compile_assert(clz_ulong_32_or_64, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) == 4 || ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) == 8);
compile_assert(clz_ullong_64, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ) == 8);
compile_assert(clz_ಶಬ್ದ_ಗಾತ್ರ, ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) * 8 == CONFIG_ಶಬ್ದ_ಗಾತ್ರ);

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
ಸ್ಥಿರ ಬಳಸದ CONST ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2 = 32;
    uint32_t ಮುಖವಾಡ_2 = UINT32_MAX;

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
    ಆದರೆ (1) {
        // iteration 4
        ಮುಖವಾಡ_2 >>= (1 << 4); // 0x0000ffff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 4; // [0, 16]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x0000ffff
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [16, 32]
    }
    ಆದರೆ (1) {
        // iteration 3
        ಮುಖವಾಡ_2 >>= (1 << 3); // 0x000000ff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 3; // [0, 8]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x000000ff
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [8, 16, 24, 32]
    }
    ಆದರೆ (1) {
        // iteration 2
        ಮುಖವಾಡ_2 >>= (1 << 2); // 0x0000000f
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 2; // [0, 4]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x0000000f
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [4, 8, 12, ..., 32]
    }
    ಆದರೆ (1) {
        // iteration 1
        ಮುಖವಾಡ_2 >>= (1 << 1); // 0x00000003
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 1; // [0, 2]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x00000003
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [2, 4, 6, ..., 32]
    }
    ಆದರೆ (1) {
        // iteration 0
        ಮುಖವಾಡ_2 >>= (1 << 0); // 0x00000001
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 0; // [0, 1]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x00000001
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2 - x;
}

ಸ್ಥಿರ ಬಳಸದ CONST ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ clz64(uint64_t x)
{
    ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2 = 64;
    uint64_t ಮುಖವಾಡ_2 = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    ಆದರೆ (1) {
        // iteration 5
        ಮುಖವಾಡ_2 >>= (1 << 5); // 0x00000000ffffffff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 5; // [0, 32]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x00000000ffffffff
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [32, 64]
    }
    ಆದರೆ (1) {
        // iteration 4
        ಮುಖವಾಡ_2 >>= (1 << 4); // 0x000000000000ffff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 4; // [0, 16]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x000000000000ffff
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [16, 32, 48, 64]
    }
    ಆದರೆ (1) {
        // iteration 3
        ಮುಖವಾಡ_2 >>= (1 << 3); // 0x00000000000000ff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 3; // [0, 8]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x00000000000000ff
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [8, 16, 24, ..., 64]
    }
    ಆದರೆ (1) {
        // iteration 2
        ಮುಖವಾಡ_2 >>= (1 << 2); // 0x000000000000000f
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 2; // [0, 4]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x000000000000000f
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [4, 8, 12, ..., 64]
    }
    ಆದರೆ (1) {
        // iteration 1
        ಮುಖವಾಡ_2 >>= (1 << 1); // 0x0000000000000003
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 1; // [0, 2]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x0000000000000003
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [2, 4, 6, ..., 64]
    }
    ಆದರೆ (1) {
        // iteration 0
        ಮುಖವಾಡ_2 >>= (1 << 0); // 0x0000000000000001
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)(ಮುಖವಾಡ_2 < x)) << 0; // [0, 1]
        x >>= ಬಿಟ್ಟುಗಳು; // <= 0x0000000000000001
        ಎಣಿಕೆ_2 -= ಬಿಟ್ಟುಗಳು; // [1, 2, 3, ..., 64]
    }

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2 - x;
}

// Count trailing zeros.
// See comments on clz32.
ಸ್ಥಿರ ಬಳಸದ CONST ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ctz32(uint32_t x)
{
    ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2 = (x == 0);
    uint32_t ಮುಖವಾಡ_2 = UINT32_MAX;

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

    ಆದರೆ (1) {
        // iteration 4
        ಮುಖವಾಡ_2 >>= (1 << 4); // 0x0000ffff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 4; // [0, 16]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x0000ffff != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 16] else 17
    }
    ಆದರೆ (1) {
        // iteration 3
        ಮುಖವಾಡ_2 >>= (1 << 3); // 0x000000ff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 3; // [0, 8]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x000000ff != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    ಆದರೆ (1) {
        // iteration 2
        ಮುಖವಾಡ_2 >>= (1 << 2); // 0x0000000f
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 2; // [0, 4]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x0000000f != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    ಆದರೆ (1) {
        // iteration 1
        ಮುಖವಾಡ_2 >>= (1 << 1); // 0x00000003
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 1; // [0, 2]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x00000003 != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    ಆದರೆ (1) {
        // iteration 0
        ಮುಖವಾಡ_2 >>= (1 << 0); // 0x00000001
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 0; // [0, 1]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x00000001 != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

ಸ್ಥಿರ ಬಳಸದ CONST ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ctz64(uint64_t x)
{
    ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2 = (x == 0);
    uint64_t ಮುಖವಾಡ_2 = UINT64_MAX;

    ಆದರೆ (1) {
        // iteration 5
        ಮುಖವಾಡ_2 >>= (1 << 5); // 0x00000000ffffffff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 5; // [0, 32]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x00000000ffffffff != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 32] else 33
    }
    ಆದರೆ (1) {
        // iteration 4
        ಮುಖವಾಡ_2 >>= (1 << 4); // 0x000000000000ffff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 4; // [0, 16]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x000000000000ffff != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    ಆದರೆ (1) {
        // iteration 3
        ಮುಖವಾಡ_2 >>= (1 << 3); // 0x00000000000000ff
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 3; // [0, 8]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x00000000000000ff != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    ಆದರೆ (1) {
        // iteration 2
        ಮುಖವಾಡ_2 >>= (1 << 2); // 0x000000000000000f
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 2; // [0, 4]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x000000000000000f != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    ಆದರೆ (1) {
        // iteration 1
        ಮುಖವಾಡ_2 >>= (1 << 1); // 0x0000000000000003
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 1; // [0, 2]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x0000000000000003 != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    ಆದರೆ (1) {
        // iteration 0
        ಮುಖವಾಡ_2 >>= (1 << 0); // 0x0000000000000001
        ಚಿಹ್ನೆರಹಿತ ಬಿಟ್ಟುಗಳು = ((ಚಿಹ್ನೆರಹಿತ)((x & ಮುಖವಾಡ_2) == 0)) << 0; // [0, 1]
        x >>= ಬಿಟ್ಟುಗಳು; // xi != 0 --> x & 0x0000000000000001 != 0
        ಎಣಿಕೆ_2 += ಬಿಟ್ಟುಗಳು; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_CLZ_32
CONST ಪೂರ್ಣಾಂಕ __clzsi2(uint32_t x)
{
    ಹಿಂತಿರುಗಿ clz32(x);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_CLZ_64
CONST ಪೂರ್ಣಾಂಕ __clzdi2(uint64_t x)
{
    ಹಿಂತಿರುಗಿ clz64(x);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_CTZ_32
CONST ಪೂರ್ಣಾಂಕ __ctzsi2(uint32_t x)
{
    ಹಿಂತಿರುಗಿ ctz32(x);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_CTZ_64
CONST ಪೂರ್ಣಾಂಕ __ctzdi2(uint64_t x)
{
    ಹಿಂತಿರುಗಿ ctz64(x);
}
#ಆದರೆ_ಅಂತ್ಯ
