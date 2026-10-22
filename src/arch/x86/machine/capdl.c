/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#إذا defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#ضمن <machine/io.h>
#ضمن <arch/object/iospace.h>


فراغ x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    چاپ_کړه("(ports: (%lu, %lu))\n",
           (اوږد بینښه صحيح)cap_io_پورټ_cap_get_cap_IOPort_لومړی_پورټ(ioports_cap),
           (اوږد بینښه صحيح)cap_io_پورټ_cap_get_cap_IOPort_وروستی_پورټ(ioports_cap));
}

#که_تعریف_شوی CONFIG_IOMMU

فراغ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    چاپ_کړه("(domain_id: %lu, pci_device: %lu)\n",
           (اوږد بینښه صحيح)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (اوږد بینښه صحيح)cap_io_space_cap_get_capDomainID(iospace_cap));
}

فراغ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    چاپ_کړه("(level: %lu)\n", (اوږد بینښه صحيح)cap_io_پاڼه_جدول_cap_get_cap_IOPTLevel(iopt_cap));
}
#د_که_پای

#د_که_پای /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
