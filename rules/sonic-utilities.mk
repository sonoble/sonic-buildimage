# sonic utilities package
#
# NOTE: sonic-config-engine is a build-time dependency of sonic-utilities
# due to unit tests which are run during the build. However,
# sonic-platform-common and swsssdk are runtime dependencies, and should be
# added here also. However, the current build system assumes all runtime
# dependencies are .deb packages.
#
# TODO: Create a way to specify both .deb and .whl runtime dependencies
#       then add the aforementioned runtime dependencies here.
#

SONIC_UTILS = python-sonic-utilities_1.2-1_all.deb
$(SONIC_UTILS)_SRC_PATH = $(SRC_PATH)/sonic-utilities
$(SONIC_UTILS)_WHEEL_DEPENDS = $(SONIC_CONFIG_ENGINE)
SONIC_PYTHON_STDEB_DEBS += $(SONIC_UTILS)
