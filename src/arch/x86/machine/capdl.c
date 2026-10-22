/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>


ਖਾਲੀ x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    ਲਿਖੋ("(ports: (%lu, %lu))\n",
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(ioports_cap),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(ioports_cap));
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU

ਖਾਲੀ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    ਲਿਖੋ("(domain_id: %lu, pci_device: %lu)\n",
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_io_space_cap_get_capDomainID(iospace_cap));
}

ਖਾਲੀ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    ਲਿਖੋ("(level: %lu)\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTLevel(iopt_cap));
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
