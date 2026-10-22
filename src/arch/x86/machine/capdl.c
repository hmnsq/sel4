/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#എങ്കിൽ defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#അന്തര്ഭാവ <arch/machine/capdl.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <arch/object/iospace.h>


ശൂന്യം x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    printf("(ports: (%lu, %lu))\n",
           (long unsigned പൂർണ്ണസംഖ്യ)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (long unsigned പൂർണ്ണസംഖ്യ)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#ifdef CONFIG_IOMMU

ശൂന്യം x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    printf("(domain_id: %lu, pci_device: %lu)\n",
           (long unsigned പൂർണ്ണസംഖ്യ)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (long unsigned പൂർണ്ണസംഖ്യ)cap_io_space_cap_get_capDomainID(iospace_cap));
}

ശൂന്യം x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    printf("(level: %lu)\n", (long unsigned പൂർണ്ണസംഖ്യ)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#endif

#endif /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
