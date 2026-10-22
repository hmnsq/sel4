/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <stdint.h>
#समावेश <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
प्रकार अचिन्हित दीर्घ __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
व्योम memzero(व्योम *s, अचिन्हित दीर्घ n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    निश्चित((अचिन्हित दीर्घ)s % माप(अचिन्हित दीर्घ) == 0);
    निश्चित(n % माप(अचिन्हित दीर्घ) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    जबतक (n != 0) {
        *(ulong_alias *)p = 0;
        p += माप(ulong_alias);
        n -= माप(ulong_alias);
    }
}

व्योम *VISIBLE स_रखो(व्योम *s, अचिन्हित दीर्घ c, अचिन्हित दीर्घ n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    अगर (likely(c == 0 && ((अचिन्हित दीर्घ)s % माप(अचिन्हित दीर्घ)) == 0 && (n % माप(अचिन्हित दीर्घ)) == 0)) {
        memzero(s, n);
    } अन्यथा {
        /* Otherwise, we use a slower, simple memset. */
        क्रम (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    वापस s;
}

व्योम *VISIBLE स_नकल(व्योम *ptr_dst, स्थिर व्योम *ptr_src, अचिन्हित दीर्घ n)
{
    uint8_t *p;
    स्थिर uint8_t *q;

    क्रम (p = (uint8_t *)ptr_dst, q = (स्थिर uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    वापस ptr_dst;
}

पूर्णांक PURE म_भेदन(स्थिर अक्षर *s1, स्थिर अक्षर *s2, पूर्णांक n)
{
    word_t i;
    पूर्णांक diff;

    क्रम (i = 0; i < n; i++) {
        diff = ((अचिन्हित अक्षर *)s1)[i] - ((अचिन्हित अक्षर *)s2)[i];
        अगर (diff != 0 || s1[i] == '\0') {
            वापस diff;
        }
    }

    वापस 0;
}

दीर्घ CONST char_to_long(अक्षर c)
{
    अगर (c >= '0' && c <= '9') {
        वापस c - '0';
    } अन्यथा अगर (c >= 'A' && c <= 'F') {
        वापस c - 'A' + 10;
    } अन्यथा अगर (c >= 'a' && c <= 'f') {
        वापस c - 'a' + 10;
    }
    वापस -1;
}

दीर्घ PURE str_to_long(स्थिर अक्षर *str)
{
    अचिन्हित पूर्णांक base;
    दीर्घ res;
    दीर्घ val = 0;
    अक्षर c;

    /* NULL ptr and empty str */
    अगर (str == शून्य || *str == 0) {
        वापस -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    अगर (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        base = 16;
        str += 2;
        /* '0x' on its own is malformed */
        अगर (*str == 0) {
            वापस -1;
        }
    } अन्यथा {
        base = 10;
    }

    c = *str;
    जबतक (c != '\0') {
        res = char_to_long(c);
        अगर (res == -1 || res >= base) {
            वापस -1;
        }
        val = val * base + res;
        str++;
        c = *str;
    }

    वापस val;
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
compile_assert(clz_ulong_32_or_64, माप(अचिन्हित दीर्घ) == 4 || माप(अचिन्हित दीर्घ) == 8);
compile_assert(clz_ullong_64, माप(अचिन्हित दीर्घ दीर्घ) == 8);
compile_assert(clz_word_आकार, माप(अचिन्हित दीर्घ) * 8 == CONFIG_WORD_आकार);

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
अटल UNUSED CONST अंतरभूत अचिन्हित clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    अचिन्हित गिनती = 32;
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
    अगर (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x0000ffff
        गिनती -= bits; // [16, 32]
    }
    अगर (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x000000ff
        गिनती -= bits; // [8, 16, 24, 32]
    }
    अगर (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x0000000f
        गिनती -= bits; // [4, 8, 12, ..., 32]
    }
    अगर (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x00000003
        गिनती -= bits; // [2, 4, 6, ..., 32]
    }
    अगर (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x00000001
        गिनती -= bits; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    वापस गिनती - x;
}

अटल UNUSED CONST अंतरभूत अचिन्हित clz64(uint64_t x)
{
    अचिन्हित गिनती = 64;
    uint64_t mask = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    अगर (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 5; // [0, 32]
        x >>= bits; // <= 0x00000000ffffffff
        गिनती -= bits; // [32, 64]
    }
    अगर (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x000000000000ffff
        गिनती -= bits; // [16, 32, 48, 64]
    }
    अगर (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x00000000000000ff
        गिनती -= bits; // [8, 16, 24, ..., 64]
    }
    अगर (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x000000000000000f
        गिनती -= bits; // [4, 8, 12, ..., 64]
    }
    अगर (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x0000000000000003
        गिनती -= bits; // [2, 4, 6, ..., 64]
    }
    अगर (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x0000000000000001
        गिनती -= bits; // [1, 2, 3, ..., 64]
    }

    वापस गिनती - x;
}

// Count trailing zeros.
// See comments on clz32.
अटल UNUSED CONST अंतरभूत अचिन्हित ctz32(uint32_t x)
{
    अचिन्हित गिनती = (x == 0);
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

    अगर (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x0000ffff != 0
        गिनती += bits; // if xi != 0 then [0, 16] else 17
    }
    अगर (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x000000ff != 0
        गिनती += bits; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    अगर (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x0000000f != 0
        गिनती += bits; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    अगर (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x00000003 != 0
        गिनती += bits; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    अगर (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x00000001 != 0
        गिनती += bits; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    वापस गिनती;
}

अटल UNUSED CONST अंतरभूत अचिन्हित ctz64(uint64_t x)
{
    अचिन्हित गिनती = (x == 0);
    uint64_t mask = UINT64_MAX;

    अगर (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 5; // [0, 32]
        x >>= bits; // xi != 0 --> x & 0x00000000ffffffff != 0
        गिनती += bits; // if xi != 0 then [0, 32] else 33
    }
    अगर (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x000000000000ffff != 0
        गिनती += bits; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    अगर (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x00000000000000ff != 0
        गिनती += bits; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    अगर (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x000000000000000f != 0
        गिनती += bits; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    अगर (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x0000000000000003 != 0
        गिनती += bits; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    अगर (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x0000000000000001 != 0
        गिनती += bits; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    वापस गिनती;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#अगर_घोषित CONFIG_CLZ_32
CONST पूर्णांक __clzsi2(uint32_t x)
{
    वापस clz32(x);
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_CLZ_64
CONST पूर्णांक __clzdi2(uint64_t x)
{
    वापस clz64(x);
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_CTZ_32
CONST पूर्णांक __ctzsi2(uint32_t x)
{
    वापस ctz32(x);
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_CTZ_64
CONST पूर्णांक __ctzdi2(uint64_t x)
{
    वापस ctz64(x);
}
#पूर्ण_अगर
