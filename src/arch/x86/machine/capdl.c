/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ಆದರೆ defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#ಅನ್ತರ್ಭಾವ <arch/machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>


ಶೂನ್ಯ x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    ಬರೆ("(ports: (%lu, %lu))\n",
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(ioports_cap),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(ioports_cap));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU

ಶೂನ್ಯ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    ಬರೆ("(domain_id: %lu, pci_device: %lu)\n",
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_io_space_cap_get_capDomainID(iospace_cap));
}

ಶೂನ್ಯ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    ಬರೆ("(level: %lu)\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTLevel(iopt_cap));
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
