/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <arch/kernel/elf.h>
#ضمن <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_کوډ bool_t elf_checkFile(Elf_سرلیک_t *elfFile, کلمه_t max_اوږدتیا)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    کلمه_t      phdr_جدول_اندازه;

    إذا (max_اوږدتیا < حجم(Elf_سرلیک_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != حجم(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        أرجع false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_جدول_اندازه = elfFile->e_phnum * حجم(Elf_Phdr_t);
    إذا (elfFile->e_phoff > max_اوږدتیا || phdr_جدول_اندازه > max_اوږدتیا - elfFile->e_phoff) {
        أرجع false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type != PT_بارول) {
            استمر;
        }
        إذا (phdr[i].p_دننه_واټن > max_اوږدتیا || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_اوږدتیا - phdr[i].p_دننه_واټن ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            أرجع false;
        }
    }
    أرجع true;
}

BOOT_کوډ v_region_t elf_get_حافظه_Bounds(Elf_سرلیک_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_پیل;
    vptr_t     sect_پای_ټکی;
    uint32_t   i;

    elf_reg.پیل = ~0UL;
    elf_reg.پای_ټکی = 0;

    /* Loop through all loadable segments and record start/end address. */
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type == PT_بارول && phdr[i].p_memsz > 0) {
            sect_پیل = phdr[i].p_vaddr;
            sect_پای_ټکی = sect_پیل + phdr[i].p_memsz;
            إذا (sect_پیل < elf_reg.پیل) {
                elf_reg.پیل = sect_پیل;
            }
            إذا (sect_پای_ټکی > elf_reg.پای_ټکی) {
                elf_reg.پای_ټکی = sect_پای_ټکی;
            }
        }
    }
    أرجع elf_reg;
}

BOOT_کوډ فراغ elf_بارول(Elf_سرلیک_t *elfFile, se_L4_کلمه دننه_واټن)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    کلمه_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    لكل (i = 0; i < elfFile->e_phnum; i++) {
        إذا (phdr[i].p_type != PT_بارول || phdr[i].p_memsz == 0) {
            استمر;
        }
        src = (paddr_t)elfFile + phdr[i].p_دننه_واټن;
        dst = phdr[i].p_vaddr + دننه_واټن;
        len = phdr[i].p_filesz;
        حافظه_نقل((فراغ *)dst, (حرف *)src, len);
    }
}
