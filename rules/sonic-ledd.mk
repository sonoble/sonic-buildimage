# sonic-ledd (SONiC Front-panel LED control daemon) Debian package

SONIC_LEDD = python-sonic-ledd_1.1-1_all.deb
$(SONIC_LEDD)_SRC_PATH = $(SRC_PATH)/sonic-platform-daemons/sonic-ledd
SONIC_PYTHON_STDEB_DEBS += $(SONIC_LEDD)
