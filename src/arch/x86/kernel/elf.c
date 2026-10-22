/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <arch/kernel/elf.h>
#అన్తర్భావ <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_సంకేతం bool_t elf_checkFile(Elf_శీర్షిక_t *elfFile, మాట_t max_నిడివి)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    మాట_t      phdr_పట్టిక_కొలత;

    ఒకవేళ (max_నిడివి < పరిమాణం(Elf_శీర్షిక_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != పరిమాణం(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        తిరిగి false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_పట్టిక_కొలత = elfFile->e_phnum * పరిమాణం(Elf_Phdr_t);
    ఒకవేళ (elfFile->e_phoff > max_నిడివి || phdr_పట్టిక_కొలత > max_నిడివి - elfFile->e_phoff) {
        తిరిగి false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    కొరకు (i = 0; i < elfFile->e_phnum; i++) {
        ఒకవేళ (phdr[i].p_type != PT_ఎక్కించు) {
            కొనసాగు;
        }
        ఒకవేళ (phdr[i].p_స్థానభ్రంశం > max_నిడివి || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_నిడివి - phdr[i].p_స్థానభ్రంశం ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            తిరిగి false;
        }
    }
    తిరిగి true;
}

BOOT_సంకేతం v_region_t elf_get_జ్ఞాపకం_Bounds(Elf_శీర్షిక_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_ప్రారంభం;
    vptr_t     sect_చివర;
    uint32_t   i;

    elf_reg.ప్రారంభం = ~0UL;
    elf_reg.చివర = 0;

    /* Loop through all loadable segments and record start/end address. */
    కొరకు (i = 0; i < elfFile->e_phnum; i++) {
        ఒకవేళ (phdr[i].p_type == PT_ఎక్కించు && phdr[i].p_memsz > 0) {
            sect_ప్రారంభం = phdr[i].p_vaddr;
            sect_చివర = sect_ప్రారంభం + phdr[i].p_memsz;
            ఒకవేళ (sect_ప్రారంభం < elf_reg.ప్రారంభం) {
                elf_reg.ప్రారంభం = sect_ప్రారంభం;
            }
            ఒకవేళ (sect_చివర > elf_reg.చివర) {
                elf_reg.చివర = sect_చివర;
            }
        }
    }
    తిరిగి elf_reg;
}

BOOT_సంకేతం శూన్య elf_ఎక్కించు(Elf_శీర్షిక_t *elfFile, se_L4_మాట స్థానభ్రంశం)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    మాట_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    కొరకు (i = 0; i < elfFile->e_phnum; i++) {
        ఒకవేళ (phdr[i].p_type != PT_ఎక్కించు || phdr[i].p_memsz == 0) {
            కొనసాగు;
        }
        src = (paddr_t)elfFile + phdr[i].p_స్థానభ్రంశం;
        dst = phdr[i].p_vaddr + స్థానభ్రంశం;
        len = phdr[i].p_filesz;
        జ్ఞాపకం_నకలు((శూన్య *)dst, (అక్షరం *)src, len);
    }
}
