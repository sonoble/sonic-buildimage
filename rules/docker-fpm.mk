# Docker-fpm rule-file is simply a wrapper containing routing-stack selection logic.

ifeq ($(SONIC_ROUTING_STACK), quagga)
SONIC_INSTALL_DOCKER_IMAGES += $(DOCKER_FPM_QUAGGA)
else ifeq ($(SONIC_ROUTING_STACK), frr)
SONIC_INSTALL_DOCKER_IMAGES += $(DOCKER_FPM_FRR)
else
SONIC_INSTALL_DOCKER_IMAGES += $(DOCKER_FPM_GOBGP)
endif
