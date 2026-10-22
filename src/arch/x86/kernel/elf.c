/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/kernel/elf.h>
#ضمن <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_ڪوڊ bool_t elf_checkFile(Elf_سرو_t *elfFile, لفظ_t max_ڊيگهائي)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    لفظ_t      phdr_جدول_سائيز;

    إذا (max_ڊيگهائي < حجم(Elf_سرو_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != حجم(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        أرجع false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_جدول_سائيز = elfFile->e_phnum * حجم(Elf_Phdr_t);
    إذا (elfFile->e_phoff > max_ڊيگهائي || phdr_جدول_سائيز > max_ڊيگهائي - elfFile->e_phoff) {
        أرجع false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type != PT_ڀريو) {
            استمر;
        }
        إذا (phdr[i].p_وٿي > max_ڊيگهائي || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_ڊيگهائي - phdr[i].p_وٿي ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            أرجع false;
        }
    }
    أرجع true;
}

BOOT_ڪوڊ v_region_t elf_get_يادگيري_Bounds(Elf_سرو_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_شروعات;
    vptr_t     sect_پڇاڙي;
    uint32_t   i;

    elf_reg.شروعات = ~0UL;
    elf_reg.پڇاڙي = 0;

    /* Loop through all loadable segments and record start/end address. */
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type == PT_ڀريو && phdr[i].p_memsz > 0) {
            sect_شروعات = phdr[i].p_vaddr;
            sect_پڇاڙي = sect_شروعات + phdr[i].p_memsz;
            إذا (sect_شروعات < elf_reg.شروعات) {
                elf_reg.شروعات = sect_شروعات;
            }
            إذا (sect_پڇاڙي > elf_reg.پڇاڙي) {
                elf_reg.پڇاڙي = sect_پڇاڙي;
            }
        }
    }
    أرجع elf_reg;
}

BOOT_ڪوڊ فراغ elf_ڀريو(Elf_سرو_t *elfFile, se_L4_لفظ وٿي)
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
        إذا (phdr[i].p_type != PT_ڀريو || phdr[i].p_memsz == 0) {
            استمر;
        }
        src = (paddr_t)elfFile + phdr[i].p_وٿي;
        dst = phdr[i].p_vaddr + وٿي;
        len = phdr[i].p_filesz;
        يادگيري_نقل((فراغ *)dst, (حرف *)src, len);
    }
}
