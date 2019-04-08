# docker image for mlnx syncd

DOCKER_SYNCD_MLNX = docker-syncd-mlnx.gz
$(DOCKER_SYNCD_MLNX)_PATH = $(PLATFORM_PATH)/docker-syncd-mlnx
$(DOCKER_SYNCD_MLNX)_DEPENDS += $(SYNCD) $(PYTHON_SDK_API) $(MLNX_SFPD) $(CRIU) $(MLNX_ISSU)
$(DOCKER_SYNCD_MLNX)_LOAD_DOCKERS += $(DOCKER_CONFIG_ENGINE)
SONIC_DOCKER_IMAGES += $(DOCKER_SYNCD_MLNX)
ifneq ($(ENABLE_SYNCD_RPC),y)
SONIC_INSTALL_DOCKER_IMAGES += $(DOCKER_SYNCD_MLNX)
endif

$(DOCKER_SYNCD_MLNX)_CONTAINER_NAME = syncd
$(DOCKER_SYNCD_MLNX)_RUN_OPT += --net=host --privileged -t
$(DOCKER_SYNCD_MLNX)_RUN_OPT += -v /host/machine.conf:/etc/machine.conf
$(DOCKER_SYNCD_MLNX)_RUN_OPT += -v /etc/sonic:/etc/sonic:ro
$(DOCKER_SYNCD_MLNX)_RUN_OPT += -v /host/warmboot:/var/warmboot
$(DOCKER_SYNCD_MLNX)_RUN_OPT += --tmpfs /run/criu
