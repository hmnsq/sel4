/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>

/*
 * memzero needs a custom type that allows us to use a word
 * that has the aliasing properties of a char.
 */
రకనిర్వచనం గుర్తులేని దీర్ఘ __attribute__((__may_alias__)) ulong_alias;

/*
 * Zero 'n' bytes of memory starting from 's'.
 *
 * 'n' and 's' must be word aligned.
 */
శూన్య memzero(శూన్య *s, గుర్తులేని దీర్ఘ n)
{
    uint8_t *p = s;

    /* Ensure alignment constraints are met. */
    ప్రతిజ్ఞా((గుర్తులేని దీర్ఘ)s % పరిమాణం(గుర్తులేని దీర్ఘ) == 0);
    ప్రతిజ్ఞా(n % పరిమాణం(గుర్తులేని దీర్ఘ) == 0);

    /* We will never memzero an area larger than the largest current
       live object */
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>n <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Write out words. */
    అయితే (n != 0) {
        *(ulong_alias *)p = 0;
        p += పరిమాణం(ulong_alias);
        n -= పరిమాణం(ulong_alias);
    }
}

శూన్య *VISIBLE memset(శూన్య *s, గుర్తులేని దీర్ఘ c, గుర్తులేని దీర్ఘ n)
{
    uint8_t *p;

    /*
     * If we are only writing zeros and we are word aligned, we can
     * use the optimized 'memzero' function.
     */
    ఒకవేళ (likely(c == 0 && ((గుర్తులేని దీర్ఘ)s % పరిమాణం(గుర్తులేని దీర్ఘ)) == 0 && (n % పరిమాణం(గుర్తులేని దీర్ఘ)) == 0)) {
        memzero(s, n);
    } లేకపోతే {
        /* Otherwise, we use a slower, simple memset. */
        కొరకు (p = (uint8_t *)s; n > 0; n--, p++) {
            *p = (uint8_t)c;
        }
    }

    తిరిగి s;
}

శూన్య *VISIBLE జ్ఞాపకం_నకలు(శూన్య *ptr_dst, స్థిరరాశి శూన్య *ptr_src, గుర్తులేని దీర్ఘ n)
{
    uint8_t *p;
    స్థిరరాశి uint8_t *q;

    కొరకు (p = (uint8_t *)ptr_dst, q = (స్థిరరాశి uint8_t *)ptr_src; n; n--, p++, q++) {
        *p = *q;
    }

    తిరిగి ptr_dst;
}

పూర్ణాంకం శుద్ధ అక్షరశ్రేణి_పోలిక(స్థిరరాశి అక్షరం *s1, స్థిరరాశి అక్షరం *s2, పూర్ణాంకం n)
{
    మాట_t i;
    పూర్ణాంకం diff;

    కొరకు (i = 0; i < n; i++) {
        diff = ((గుర్తులేని అక్షరం *)s1)[i] - ((గుర్తులేని అక్షరం *)s2)[i];
        ఒకవేళ (diff != 0 || s1[i] == '\0') {
            తిరిగి diff;
        }
    }

    తిరిగి 0;
}

దీర్ఘ CONST char_గమ్యం_long(అక్షరం c)
{
    ఒకవేళ (c >= '0' && c <= '9') {
        తిరిగి c - '0';
    } లేకపోతే ఒకవేళ (c >= 'A' && c <= 'F') {
        తిరిగి c - 'A' + 10;
    } లేకపోతే ఒకవేళ (c >= 'a' && c <= 'f') {
        తిరిగి c - 'a' + 10;
    }
    తిరిగి -1;
}

దీర్ఘ శుద్ధ str_గమ్యం_long(స్థిరరాశి అక్షరం *str)
{
    గుర్తులేని పూర్ణాంకం పునాది;
    దీర్ఘ res;
    దీర్ఘ val = 0;
    అక్షరం c;

    /* NULL ptr and empty str */
    ఒకవేళ (str == ఖాళీ || *str == 0) {
        తిరిగి -1;
    }

    /* check for "0x", *(str + 1) may be 0, but must be allocated since str is not empty */
    ఒకవేళ (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        పునాది = 16;
        str += 2;
        /* '0x' on its own is malformed */
        ఒకవేళ (*str == 0) {
            తిరిగి -1;
        }
    } లేకపోతే {
        పునాది = 10;
    }

    c = *str;
    అయితే (c != '\0') {
        res = char_గమ్యం_long(c);
        ఒకవేళ (res == -1 || res >= పునాది) {
            తిరిగి -1;
        }
        val = val * పునాది + res;
        str++;
        c = *str;
    }

    తిరిగి val;
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
compile_assert(clz_ulong_32_or_64, పరిమాణం(గుర్తులేని దీర్ఘ) == 4 || పరిమాణం(గుర్తులేని దీర్ఘ) == 8);
compile_assert(clz_ullong_64, పరిమాణం(గుర్తులేని దీర్ఘ దీర్ఘ) == 8);
compile_assert(clz_మాట_కొలత, పరిమాణం(గుర్తులేని దీర్ఘ) * 8 == CONFIG_మాట_కొలత);

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
స్థిర వాడని CONST అంతఃవరుస గుర్తులేని clz32(uint32_t x)
{
    // Compiler builtins typically return int, but we use unsigned internally
    // to reduce the number of guards we see in the proofs.
    గుర్తులేని లెక్క = 32;
    uint32_t ముసుగు_2 = UINT32_MAX;

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
    ఒకవేళ (1) {
        // iteration 4
        ముసుగు_2 >>= (1 << 4); // 0x0000ffff
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 4; // [0, 16]
        x >>= బిట్లు; // <= 0x0000ffff
        లెక్క -= బిట్లు; // [16, 32]
    }
    ఒకవేళ (1) {
        // iteration 3
        ముసుగు_2 >>= (1 << 3); // 0x000000ff
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 3; // [0, 8]
        x >>= బిట్లు; // <= 0x000000ff
        లెక్క -= బిట్లు; // [8, 16, 24, 32]
    }
    ఒకవేళ (1) {
        // iteration 2
        ముసుగు_2 >>= (1 << 2); // 0x0000000f
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 2; // [0, 4]
        x >>= బిట్లు; // <= 0x0000000f
        లెక్క -= బిట్లు; // [4, 8, 12, ..., 32]
    }
    ఒకవేళ (1) {
        // iteration 1
        ముసుగు_2 >>= (1 << 1); // 0x00000003
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 1; // [0, 2]
        x >>= బిట్లు; // <= 0x00000003
        లెక్క -= బిట్లు; // [2, 4, 6, ..., 32]
    }
    ఒకవేళ (1) {
        // iteration 0
        ముసుగు_2 >>= (1 << 0); // 0x00000001
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 0; // [0, 1]
        x >>= బిట్లు; // <= 0x00000001
        లెక్క -= బిట్లు; // [1, 2, 3, ..., 32]
    }

    // If the original input was zero, there will have been no shifts, so this
    // gives a result of 32. Otherwise, x is now exactly 1, so subtracting from
    // count gives a result from [0, 1, 2, ..., 31].
    తిరిగి లెక్క - x;
}

స్థిర వాడని CONST అంతఃవరుస గుర్తులేని clz64(uint64_t x)
{
    గుర్తులేని లెక్క = 64;
    uint64_t ముసుగు_2 = UINT64_MAX;

    // Although we could implement this using clz32, we spell out the
    // iterations in full for slightly better code generation at low
    // optimisation levels, and to allow us to reuse the proof machinery we
    // developed for clz32.
    ఒకవేళ (1) {
        // iteration 5
        ముసుగు_2 >>= (1 << 5); // 0x00000000ffffffff
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 5; // [0, 32]
        x >>= బిట్లు; // <= 0x00000000ffffffff
        లెక్క -= బిట్లు; // [32, 64]
    }
    ఒకవేళ (1) {
        // iteration 4
        ముసుగు_2 >>= (1 << 4); // 0x000000000000ffff
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 4; // [0, 16]
        x >>= బిట్లు; // <= 0x000000000000ffff
        లెక్క -= బిట్లు; // [16, 32, 48, 64]
    }
    ఒకవేళ (1) {
        // iteration 3
        ముసుగు_2 >>= (1 << 3); // 0x00000000000000ff
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 3; // [0, 8]
        x >>= బిట్లు; // <= 0x00000000000000ff
        లెక్క -= బిట్లు; // [8, 16, 24, ..., 64]
    }
    ఒకవేళ (1) {
        // iteration 2
        ముసుగు_2 >>= (1 << 2); // 0x000000000000000f
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 2; // [0, 4]
        x >>= బిట్లు; // <= 0x000000000000000f
        లెక్క -= బిట్లు; // [4, 8, 12, ..., 64]
    }
    ఒకవేళ (1) {
        // iteration 1
        ముసుగు_2 >>= (1 << 1); // 0x0000000000000003
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 1; // [0, 2]
        x >>= బిట్లు; // <= 0x0000000000000003
        లెక్క -= బిట్లు; // [2, 4, 6, ..., 64]
    }
    ఒకవేళ (1) {
        // iteration 0
        ముసుగు_2 >>= (1 << 0); // 0x0000000000000001
        గుర్తులేని బిట్లు = ((గుర్తులేని)(ముసుగు_2 < x)) << 0; // [0, 1]
        x >>= బిట్లు; // <= 0x0000000000000001
        లెక్క -= బిట్లు; // [1, 2, 3, ..., 64]
    }

    తిరిగి లెక్క - x;
}

// Count trailing zeros.
// See comments on clz32.
స్థిర వాడని CONST అంతఃవరుస గుర్తులేని ctz32(uint32_t x)
{
    గుర్తులేని లెక్క = (x == 0);
    uint32_t ముసుగు_2 = UINT32_MAX;

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

    ఒకవేళ (1) {
        // iteration 4
        ముసుగు_2 >>= (1 << 4); // 0x0000ffff
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 4; // [0, 16]
        x >>= బిట్లు; // xi != 0 --> x & 0x0000ffff != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 16] else 17
    }
    ఒకవేళ (1) {
        // iteration 3
        ముసుగు_2 >>= (1 << 3); // 0x000000ff
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 3; // [0, 8]
        x >>= బిట్లు; // xi != 0 --> x & 0x000000ff != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 8, 16, 24] else 25
    }
    ఒకవేళ (1) {
        // iteration 2
        ముసుగు_2 >>= (1 << 2); // 0x0000000f
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 2; // [0, 4]
        x >>= బిట్లు; // xi != 0 --> x & 0x0000000f != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 4, 8, ..., 28] else 29
    }
    ఒకవేళ (1) {
        // iteration 1
        ముసుగు_2 >>= (1 << 1); // 0x00000003
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 1; // [0, 2]
        x >>= బిట్లు; // xi != 0 --> x & 0x00000003 != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 2, 4, ..., 30] else 31
    }
    ఒకవేళ (1) {
        // iteration 0
        ముసుగు_2 >>= (1 << 0); // 0x00000001
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 0; // [0, 1]
        x >>= బిట్లు; // xi != 0 --> x & 0x00000001 != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 1, 2, ..., 31] else 32
    }

    తిరిగి లెక్క;
}

స్థిర వాడని CONST అంతఃవరుస గుర్తులేని ctz64(uint64_t x)
{
    గుర్తులేని లెక్క = (x == 0);
    uint64_t ముసుగు_2 = UINT64_MAX;

    ఒకవేళ (1) {
        // iteration 5
        ముసుగు_2 >>= (1 << 5); // 0x00000000ffffffff
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 5; // [0, 32]
        x >>= బిట్లు; // xi != 0 --> x & 0x00000000ffffffff != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 32] else 33
    }
    ఒకవేళ (1) {
        // iteration 4
        ముసుగు_2 >>= (1 << 4); // 0x000000000000ffff
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 4; // [0, 16]
        x >>= బిట్లు; // xi != 0 --> x & 0x000000000000ffff != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 16, 32, 48] else 49
    }
    ఒకవేళ (1) {
        // iteration 3
        ముసుగు_2 >>= (1 << 3); // 0x00000000000000ff
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 3; // [0, 8]
        x >>= బిట్లు; // xi != 0 --> x & 0x00000000000000ff != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 8, 16, ..., 56] else 57
    }
    ఒకవేళ (1) {
        // iteration 2
        ముసుగు_2 >>= (1 << 2); // 0x000000000000000f
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 2; // [0, 4]
        x >>= బిట్లు; // xi != 0 --> x & 0x000000000000000f != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 4, 8, ..., 60] else 61
    }
    ఒకవేళ (1) {
        // iteration 1
        ముసుగు_2 >>= (1 << 1); // 0x0000000000000003
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 1; // [0, 2]
        x >>= బిట్లు; // xi != 0 --> x & 0x0000000000000003 != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 2, 4, ..., 62] else 63
    }
    ఒకవేళ (1) {
        // iteration 0
        ముసుగు_2 >>= (1 << 0); // 0x0000000000000001
        గుర్తులేని బిట్లు = ((గుర్తులేని)((x & ముసుగు_2) == 0)) << 0; // [0, 1]
        x >>= బిట్లు; // xi != 0 --> x & 0x0000000000000001 != 0
        లెక్క += బిట్లు; // if xi != 0 then [0, 1, 2, ..., 63] else 64
    }

    తిరిగి లెక్క;
}

// GCC's builtins will emit calls to these functions when the platform does
// not provide suitable inline assembly.
// These are only provided when the relevant config items are set.
// We define these separately from `ctz32` etc. so that we can verify all of
// `ctz32` etc. without necessarily linking them into the kernel binary.
#నిర్వచితమైతే CONFIG_CLZ_32
CONST పూర్ణాంకం __clzsi2(uint32_t x)
{
    తిరిగి clz32(x);
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_CLZ_64
CONST పూర్ణాంకం __clzdi2(uint64_t x)
{
    తిరిగి clz64(x);
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_CTZ_32
CONST పూర్ణాంకం __ctzsi2(uint32_t x)
{
    తిరిగి ctz32(x);
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_CTZ_64
CONST పూర్ణాంకం __ctzdi2(uint64_t x)
{
    తిరిగి ctz64(x);
}
#అయితే_ముగింపు
