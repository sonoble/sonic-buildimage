# docker image for teamd agent

DOCKER_TEAMD = docker-teamd.gz
$(DOCKER_TEAMD)_PATH = $(DOCKERS_PATH)/docker-teamd
$(DOCKER_TEAMD)_DEPENDS += $(SWSS) $(LIBTEAMDCT) $(LIBTEAM_UTILS) $(REDIS_TOOLS)
$(DOCKER_TEAMD)_LOAD_DOCKERS += $(DOCKER_CONFIG_ENGINE)
SONIC_DOCKER_IMAGES += $(DOCKER_TEAMD)
SONIC_INSTALL_DOCKER_IMAGES += $(DOCKER_TEAMD)

$(DOCKER_TEAMD)_CONTAINER_NAME = teamd
$(DOCKER_TEAMD)_RUN_OPT += --net=host --privileged -t
$(DOCKER_TEAMD)_RUN_OPT += -v /etc/sonic:/etc/sonic:ro
$(DOCKER_TEAMD)_RUN_OPT += -v /host/warmboot:/var/warmboot

$(DOCKER_TEAMD)_BASE_IMAGE_FILES += teamdctl:/usr/bin/teamdctl
