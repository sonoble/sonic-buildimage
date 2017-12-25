# Arista Platform modules

ARISTA_PLATFORM_MODULE_VERSION = 1.0

export ARISTA_PLATFORM_MODULE_VERSION

ARISTA_PLATFORM_MODULE = sonic-platform-arista_$(ARISTA_PLATFORM_MODULE_VERSION)_amd64.deb
$(ARISTA_PLATFORM_MODULE)_SRC_PATH = $(PLATFORM_PATH)/sonic-platform-modules-arista
$(ARISTA_PLATFORM_MODULE)_DEPENDS += $(LINUX_HEADERS) $(LINUX_HEADERS_COMMON)
SONIC_DPKG_DEBS += $(ARISTA_PLATFORM_MODULE)
