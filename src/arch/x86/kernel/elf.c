/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/elf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_ਕੋਡ bool_t elf_checkFile(Elf_ਸਿਰਲੇਖ_t *elfFile, ਸ਼ਬਦ_t max_ਲੰਮਾਈ)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    ਸ਼ਬਦ_t      phdr_ਸਾਰਣੀ_ਮਾਪ;

    ਜੇ (max_ਲੰਮਾਈ < ਆਕਾਰ(Elf_ਸਿਰਲੇਖ_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != ਆਕਾਰ(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        ਵਾਪਸ false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_ਸਾਰਣੀ_ਮਾਪ = elfFile->e_phnum * ਆਕਾਰ(Elf_Phdr_t);
    ਜੇ (elfFile->e_phoff > max_ਲੰਮਾਈ || phdr_ਸਾਰਣੀ_ਮਾਪ > max_ਲੰਮਾਈ - elfFile->e_phoff) {
        ਵਾਪਸ false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    ਲਈ (i = 0; i < elfFile->e_phnum; i++) {
        ਜੇ (phdr[i].p_type != PT_ਲੱਦੋ) {
            ਜਾਰੀ;
        }
        ਜੇ (phdr[i].p_ਵਿੱਥ > max_ਲੰਮਾਈ || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_ਲੰਮਾਈ - phdr[i].p_ਵਿੱਥ ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            ਵਾਪਸ false;
        }
    }
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ v_region_t elf_get_ਸਮ੍ਰਿਤੀ_Bounds(Elf_ਸਿਰਲੇਖ_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_ਸ਼ੁਰੂ;
    vptr_t     sect_ਸਿਰਾ;
    uint32_t   i;

    elf_reg.ਸ਼ੁਰੂ = ~0UL;
    elf_reg.ਸਿਰਾ = 0;

    /* Loop through all loadable segments and record start/end address. */
    ਲਈ (i = 0; i < elfFile->e_phnum; i++) {
        ਜੇ (phdr[i].p_type == PT_ਲੱਦੋ && phdr[i].p_memsz > 0) {
            sect_ਸ਼ੁਰੂ = phdr[i].p_vaddr;
            sect_ਸਿਰਾ = sect_ਸ਼ੁਰੂ + phdr[i].p_memsz;
            ਜੇ (sect_ਸ਼ੁਰੂ < elf_reg.ਸ਼ੁਰੂ) {
                elf_reg.ਸ਼ੁਰੂ = sect_ਸ਼ੁਰੂ;
            }
            ਜੇ (sect_ਸਿਰਾ > elf_reg.ਸਿਰਾ) {
                elf_reg.ਸਿਰਾ = sect_ਸਿਰਾ;
            }
        }
    }
    ਵਾਪਸ elf_reg;
}

BOOT_ਕੋਡ ਖਾਲੀ elf_ਲੱਦੋ(Elf_ਸਿਰਲੇਖ_t *elfFile, se_L4_ਸ਼ਬਦ ਵਿੱਥ)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    ਸ਼ਬਦ_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    ਲਈ (i = 0; i < elfFile->e_phnum; i++) {
        ਜੇ (phdr[i].p_type != PT_ਲੱਦੋ || phdr[i].p_memsz == 0) {
            ਜਾਰੀ;
        }
        src = (paddr_t)elfFile + phdr[i].p_ਵਿੱਥ;
        dst = phdr[i].p_vaddr + ਵਿੱਥ;
        len = phdr[i].p_filesz;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)dst, (ਅੱਖਰ *)src, len);
    }
}
