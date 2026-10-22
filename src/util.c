/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
प्रकारनिर्देश अचिन्हित दीर्घ __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
शून्य memzero(शून्य *s, अचिन्हित दीर्घ n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    प्रतिज्ञा((अचिन्हित दीर्घ)s % परिमाण(अचिन्हित दीर्घ) == 0);
    प्रतिज्ञा(n % परिमाण(अचिन्हित दीर्घ) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    यावत् (n != 0) {
        *(ulong_alias *)p = 0;
        p += परिमाण(ulong_alias);
        n -= परिमाण(ulong_alias);
    }
}

शून्य *VISIBLE स_रखो(शून्य *s, अचिन्हित दीर्घ c, अचिन्हित दीर्घ n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    यदि (likely(c == 0 && ((अचिन्हित दीर्घ)s % परिमाण(अचिन्हित दीर्घ)) == 0 && (n % परिमाण(अचिन्हित दीर्घ)) == 0)) {
        memzero(s, n);
    } अन्यथा {
        /* Otherwise, we use a slower, simple memset. */
        प्रति (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    प्रत्यागम s;
}

शून्य *VISIBLE स_नकल(शून्य *ptr_dst, ध्रुव शून्य *ptr_src, अचिन्हित दीर्घ n)
{
    uint8_t *p;
    ध्रुव uint8_t *q;

    प्रति (p = (uint8_t *)ptr_dst, q = (ध्रुव uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    प्रत्यागम ptr_dst;
}

पूर्णाङ्क PURE म_भेदन(ध्रुव वर्ण *s1, ध्रुव वर्ण *s2, पूर्णाङ्क n)
{
    word_t i;
    पूर्णाङ्क diff;

    प्रति (i = 0; i < n; i++) {
        diff = ((अचिन्हित वर्ण *)s1)[i] - ((अचिन्हित वर्ण *)s2)[i];
        यदि (diff != 0 || s1[i] == '\0') {
            प्रत्यागम diff;
        }
    }

    प्रत्यागम 0;
}

दीर्घ CONST char_to_long(वर्ण c)
{
    यदि (c >= '0' && c <= '9') {
        प्रत्यागम c - '0';
    } अन्यथा यदि (c >= 'A' && c <= 'F') {
        प्रत्यागम c - 'A' + 10;
    } अन्यथा यदि (c >= 'a' && c <= 'f') {
        प्रत्यागम c - 'a' + 10;
    }
    प्रत्यागम -1;
}

दीर्घ PURE str_to_long(ध्रुव वर्ण *str)
{
    अचिन्हित पूर्णाङ्क base;
    दीर्घ res;
    दीर्घ val = 0;
    वर्ण c;

    /* NULL ptr and empty str */
    यदि (str == रिक्त || *str == 0) {
        प्रत्यागम -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    यदि (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        base = 16;
        str += 2;
        /* '0x' on its own is malformed */
        यदि (*str == 0) {
            प्रत्यागम -1;
        }
    } अन्यथा {
        base = 10;
    }

    c = *str;
    यावत् (c != '\0') {
        res = char_to_long(c);
        यदि (res == -1 || res >= base) {
            प्रत्यागम -1;
        }
        val = val * base + res;
        str++;
        c = *str;
    }

    प्रत्यागम val;
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
compile_assert(clz_ulong_32_or_64, परिमाण(अचिन्हित दीर्घ) == 4 || परिमाण(अचिन्हित दीर्घ) == 8);
compile_assert(clz_ullong_64, परिमाण(अचिन्हित दीर्घ दीर्घ) == 8);
compile_assert(clz_word_size, परिमाण(अचिन्हित दीर्घ) * 8 == CONFIG_WORD_SIZE);

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
स्थिर UNUSED CONST अन्तःस्थ अचिन्हित clz32(uint32_t x)
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
    यदि (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x0000ffff
        गिनती -= bits; // [16, 32]
    }
    यदि (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x000000ff
        गिनती -= bits; // [8, 16, 24, 32]
    }
    यदि (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x0000000f
        गिनती -= bits; // [4, 8, 12, ..., 32]
    }
    यदि (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x00000003
        गिनती -= bits; // [2, 4, 6, ..., 32]
    }
    यदि (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x00000001
        गिनती -= bits; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    प्रत्यागम गिनती - x;
}

स्थिर UNUSED CONST अन्तःस्थ अचिन्हित clz64(uint64_t x)
{
    अचिन्हित गिनती = 64;
    uint64_t mask = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    यदि (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 5; // [0, 32]
        x >>= bits; // <= 0x00000000ffffffff
        गिनती -= bits; // [32, 64]
    }
    यदि (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 4; // [0, 16]
        x >>= bits; // <= 0x000000000000ffff
        गिनती -= bits; // [16, 32, 48, 64]
    }
    यदि (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 3; // [0, 8]
        x >>= bits; // <= 0x00000000000000ff
        गिनती -= bits; // [8, 16, 24, ..., 64]
    }
    यदि (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 2; // [0, 4]
        x >>= bits; // <= 0x000000000000000f
        गिनती -= bits; // [4, 8, 12, ..., 64]
    }
    यदि (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 1; // [0, 2]
        x >>= bits; // <= 0x0000000000000003
        गिनती -= bits; // [2, 4, 6, ..., 64]
    }
    यदि (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        अचिन्हित bits = ((अचिन्हित)(mask < x)) << 0; // [0, 1]
        x >>= bits; // <= 0x0000000000000001
        गिनती -= bits; // [1, 2, 3, ..., 64]
    }

    प्रत्यागम गिनती - x;
}

// Count trailing zeros.
// See comments on clz32.
स्थिर UNUSED CONST अन्तःस्थ अचिन्हित ctz32(uint32_t x)
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

    यदि (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x0000ffff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x0000ffff != 0
        गिनती += bits; // if xi != 0 then [0, 16] else 17
    }
    यदि (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x000000ff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x000000ff != 0
        गिनती += bits; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    यदि (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x0000000f
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x0000000f != 0
        गिनती += bits; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    यदि (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x00000003
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x00000003 != 0
        गिनती += bits; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    यदि (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x00000001
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x00000001 != 0
        गिनती += bits; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    प्रत्यागम गिनती;
}

स्थिर UNUSED CONST अन्तःस्थ अचिन्हित ctz64(uint64_t x)
{
    अचिन्हित गिनती = (x == 0);
    uint64_t mask = UINT64_MAX;

    यदि (1) {
        // iteration 5
        mask >>= (1 << 5); // 0x00000000ffffffff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 5; // [0, 32]
        x >>= bits; // xi != 0 --> x & 0x00000000ffffffff != 0
        गिनती += bits; // if xi != 0 then [0, 32] else 33
    }
    यदि (1) {
        // iteration 4
        mask >>= (1 << 4); // 0x000000000000ffff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 4; // [0, 16]
        x >>= bits; // xi != 0 --> x & 0x000000000000ffff != 0
        गिनती += bits; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    यदि (1) {
        // iteration 3
        mask >>= (1 << 3); // 0x00000000000000ff
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 3; // [0, 8]
        x >>= bits; // xi != 0 --> x & 0x00000000000000ff != 0
        गिनती += bits; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    यदि (1) {
        // iteration 2
        mask >>= (1 << 2); // 0x000000000000000f
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 2; // [0, 4]
        x >>= bits; // xi != 0 --> x & 0x000000000000000f != 0
        गिनती += bits; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    यदि (1) {
        // iteration 1
        mask >>= (1 << 1); // 0x0000000000000003
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 1; // [0, 2]
        x >>= bits; // xi != 0 --> x & 0x0000000000000003 != 0
        गिनती += bits; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    यदि (1) {
        // iteration 0
        mask >>= (1 << 0); // 0x0000000000000001
        अचिन्हित bits = ((अचिन्हित)((x & mask) == 0)) << 0; // [0, 1]
        x >>= bits; // xi != 0 --> x & 0x0000000000000001 != 0
        गिनती += bits; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    प्रत्यागम गिनती;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#ifdef CONFIG_CLZ_32
CONST पूर्णाङ्क __clzsi2(uint32_t x)
{
    प्रत्यागम clz32(x);
}
#endif

#ifdef CONFIG_CLZ_64
CONST पूर्णाङ्क __clzdi2(uint64_t x)
{
    प्रत्यागम clz64(x);
}
#endif

#ifdef CONFIG_CTZ_32
CONST पूर्णाङ्क __ctzsi2(uint32_t x)
{
    प्रत्यागम ctz32(x);
}
#endif

#ifdef CONFIG_CTZ_64
CONST पूर्णाङ्क __ctzdi2(uint64_t x)
{
    प्रत्यागम ctz64(x);
}
#endif
