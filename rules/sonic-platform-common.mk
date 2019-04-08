# sonic-platform-common package

SONIC_PLATFORM_COMMON_PY2 = sonic_platform_common-1.0-py2-none-any.whl
$(SONIC_PLATFORM_COMMON_PY2)_SRC_PATH = $(SRC_PATH)/sonic-platform-common
$(SONIC_PLATFORM_COMMON_PY2)_PYTHON_VERSION = 2
SONIC_PYTHON_WHEELS += $(SONIC_PLATFORM_COMMON_PY2)

# Als build sonic-platform-common into python3 wheel, so we can use PSU code in SNMP docker
# Note: _DEPENDS macro is not defined
SONIC_PLATFORM_COMMON_PY3 = sonic_platform_common-1.0-py3-none-any.whl
$(SONIC_PLATFORM_COMMON_PY3)_SRC_PATH = $(SRC_PATH)/sonic-platform-common
$(SONIC_PLATFORM_COMMON_PY3)_PYTHON_VERSION = 3
SONIC_PYTHON_WHEELS += $(SONIC_PLATFORM_COMMON_PY3)
