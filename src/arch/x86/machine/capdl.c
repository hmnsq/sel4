/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#ఒకవేళ defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#అన్తర్భావ <arch/machine/capdl.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/object/iospace.h>


శూన్య x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    రాయి("(ports: (%lu, %lu))\n",
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(ioports_cap),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(ioports_cap));
}

#నిర్వచితమైతే CONFIG_IOMMU

శూన్య x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    రాయి("(domain_id: %lu, pci_device: %lu)\n",
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_io_space_cap_get_capDomainID(iospace_cap));
}

శూన్య x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    రాయి("(level: %lu)\n", (దీర్ఘ గుర్తులేని పూర్ణాంకం)cap_io_పుట_పట్టిక_cap_get_cap_IOPTLevel(iopt_cap));
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
