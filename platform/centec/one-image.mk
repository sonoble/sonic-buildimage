# sonic centec one image installer

SONIC_ONE_IMAGE = sonic-centec.bin
$(SONIC_ONE_IMAGE)_MACHINE = centec
$(SONIC_ONE_IMAGE)_IMAGE_TYPE = onie
$(SONIC_ONE_IMAGE)_INSTALLS += $(CENTEC_SDK_KERNEL)
$(SONIC_ONE_IMAGE)_DOCKERS += $(SONIC_INSTALL_DOCKER_IMAGES)
SONIC_INSTALLERS += $(SONIC_ONE_IMAGE)
