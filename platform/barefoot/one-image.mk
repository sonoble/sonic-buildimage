# sonic one image installer

SONIC_ONE_IMAGE = sonic-barefoot.bin
$(SONIC_ONE_IMAGE)_MACHINE = barefoot
$(SONIC_ONE_IMAGE)_IMAGE_TYPE = onie
$(SONIC_ONE_IMAGE)_INSTALLS += $(BFN_PLATFORM_MODULE) 
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(BFN_MONTARA_PLATFORM_MODULE)
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(WNC_OSW1800_PLATFORM_MODULE) 
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(INGRASYS_S9180_32X_PLATFORM_MODULE)
$(SONIC_ONE_IMAGE)_LAZY_INSTALLS += $(INGRASYS_S9280_64X_PLATFORM_MODULE)
$(SONIC_ONE_IMAGE)_DOCKERS += $(SONIC_INSTALL_DOCKER_IMAGES)
SONIC_INSTALLERS += $(SONIC_ONE_IMAGE)
