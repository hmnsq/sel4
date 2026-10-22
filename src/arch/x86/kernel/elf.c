/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/kernel/elf.h>
#ضمن <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_کوڈ bool_t elf_checkFile(Elf_سرخی_t *elfFile, لفظ_t max_لمائی)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    لفظ_t      phdr_جدول_ناپ;

    إذا (max_لمائی < حجم(Elf_سرخی_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != حجم(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        أرجع false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_جدول_ناپ = elfFile->e_phnum * حجم(Elf_Phdr_t);
    إذا (elfFile->e_phoff > max_لمائی || phdr_جدول_ناپ > max_لمائی - elfFile->e_phoff) {
        أرجع false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type != PT_لدو) {
            استمر;
        }
        إذا (phdr[i].p_وتھ > max_لمائی || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_لمائی - phdr[i].p_وتھ ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            أرجع false;
        }
    }
    أرجع true;
}

BOOT_کوڈ v_region_t elf_get_حافظہ_Bounds(Elf_سرخی_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_شروع;
    vptr_t     sect_سرا;
    uint32_t   i;

    elf_reg.شروع = ~0UL;
    elf_reg.سرا = 0;

    /* Loop through all loadable segments and record start/end address. */
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type == PT_لدو && phdr[i].p_memsz > 0) {
            sect_شروع = phdr[i].p_vaddr;
            sect_سرا = sect_شروع + phdr[i].p_memsz;
            إذا (sect_شروع < elf_reg.شروع) {
                elf_reg.شروع = sect_شروع;
            }
            إذا (sect_سرا > elf_reg.سرا) {
                elf_reg.سرا = sect_سرا;
            }
        }
    }
    أرجع elf_reg;
}

BOOT_کوڈ فراغ elf_لدو(Elf_سرخی_t *elfFile, se_L4_لفظ وتھ)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    لفظ_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type != PT_لدو || phdr[i].p_memsz == 0) {
            استمر;
        }
        src = (paddr_t)elfFile + phdr[i].p_وتھ;
        dst = phdr[i].p_vaddr + وتھ;
        len = phdr[i].p_filesz;
        حافظہ_نقل((فراغ *)dst, (حرف *)src, len);
    }
}
