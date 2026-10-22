/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#எனில் defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#அந்தர்பாவ <arch/machine/capdl.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <arch/object/iospace.h>


வெறுமை x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    printf("(ports: (%lu, %lu))\n",
           (long unsigned முழு)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (long unsigned முழு)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#ifdef CONFIG_IOMMU

வெறுமை x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    printf("(domain_id: %lu, pci_device: %lu)\n",
           (long unsigned முழு)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (long unsigned முழு)cap_io_space_cap_get_capDomainID(iospace_cap));
}

வெறுமை x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    printf("(level: %lu)\n", (long unsigned முழு)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#endif

#endif /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
