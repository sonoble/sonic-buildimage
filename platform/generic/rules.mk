include $(PLATFORM_PATH)/aboot-image.mk
include $(PLATFORM_PATH)/onie-image.mk

SONIC_ALL += $(DOCKER_DATABASE) \
         $(DOCKER_LLDP_SV2) \
         $(DOCKER_SNMP_SV2) \
         $(DOCKER_PLATFORM_MONITOR) \
         $(DOCKER_DHCP_RELAY) \
         $(DOCKER_PTF) \
         $(SONIC_GENERIC_ONIE_IMAGE)
