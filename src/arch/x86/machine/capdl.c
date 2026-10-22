/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/object/iospace.h>


शून्य x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    प्रारूप_लिख("(ports: (%lu, %lu))\n",
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(ioports_cap),
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(ioports_cap));
}

#यदि_परिभाषितम् CONFIG_IOMMU

शून्य x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    प्रारूप_लिख("(domain_id: %lu, pci_device: %lu)\n",
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (दीर्घ अचिह्नित पूर्णाङ्क)cap_io_space_cap_get_capDomainID(iospace_cap));
}

शून्य x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    प्रारूप_लिख("(level: %lu)\n", (दीर्घ अचिह्नित पूर्णाङ्क)cap_io_page_सारणी_cap_get_cap_IOPTLevel(iopt_cap));
}
#यदि_अन्तः

#यदि_अन्तः /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
