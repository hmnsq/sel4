/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <arch/kernel/elf.h>
#شامل <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_رمز bool_t elf_checkFile(Elf_سرخی_t *elfFile, لفظ_t max_لمبائی)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    لفظ_t      phdr_جدول_سائز;

    اگر (max_لمبائی < حجم(Elf_سرخی_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != حجم(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        واپس false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_جدول_سائز = elfFile->e_phnum * حجم(Elf_Phdr_t);
    اگر (elfFile->e_phoff > max_لمبائی || phdr_جدول_سائز > max_لمبائی - elfFile->e_phoff) {
        واپس false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    برائے (i = 0; i < elfFile->e_phnum; i++) {
        اگر (phdr[i].p_type != PT_بھرو) {
            جاری;
        }
        اگر (phdr[i].p_انحراف > max_لمبائی || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_لمبائی - phdr[i].p_انحراف ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            واپس false;
        }
    }
    واپس true;
}

BOOT_رمز v_region_t elf_get_حافظہ_Bounds(Elf_سرخی_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_آغاز;
    vptr_t     sect_انتہا;
    uint32_t   i;

    elf_reg.آغاز = ~0UL;
    elf_reg.انتہا = 0;

    /* Loop through all loadable segments and record start/end address. */
    برائے (i = 0; i < elfFile->e_phnum; i++) {
        اگر (phdr[i].p_type == PT_بھرو && phdr[i].p_memsz > 0) {
            sect_آغاز = phdr[i].p_vaddr;
            sect_انتہا = sect_آغاز + phdr[i].p_memsz;
            اگر (sect_آغاز < elf_reg.آغاز) {
                elf_reg.آغاز = sect_آغاز;
            }
            اگر (sect_انتہا > elf_reg.انتہا) {
                elf_reg.انتہا = sect_انتہا;
            }
        }
    }
    واپس elf_reg;
}

BOOT_رمز خالی elf_بھرو(Elf_سرخی_t *elfFile, se_L4_لفظ انحراف)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    لفظ_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    برائے (i = 0; i < elfFile->e_phnum; i++) {
        اگر (phdr[i].p_type != PT_بھرو || phdr[i].p_memsz == 0) {
            جاری;
        }
        src = (paddr_t)elfFile + phdr[i].p_انحراف;
        dst = phdr[i].p_vaddr + انحراف;
        len = phdr[i].p_filesz;
        حافظہ_نقل((خالی *)dst, (حرف *)src, len);
    }
}
