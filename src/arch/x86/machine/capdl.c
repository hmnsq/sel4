/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>

#যদি defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#অন্তর্ভাৱ <arch/machine/capdl.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <kernel/cspace.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <arch/object/iospace.h>


শূন্য x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    printf("(ports: (%lu, %lu))\n",
           (long unsigned পূর্ণাঙ্ক)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (long unsigned পূর্ণাঙ্ক)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#ifdef CONFIG_IOMMU

শূন্য x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    printf("(domain_id: %lu, pci_device: %lu)\n",
           (long unsigned পূর্ণাঙ্ক)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (long unsigned পূর্ণাঙ্ক)cap_io_space_cap_get_capDomainID(iospace_cap));
}

শূন্য x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    printf("(level: %lu)\n", (long unsigned পূর্ণাঙ্ক)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#endif

#endif /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
