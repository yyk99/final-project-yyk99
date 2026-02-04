TERMOSOS_VERSION = 1.0
TERMOSOS_SITE = $(BR2_EXTERNAL_yyk99_extra_PATH)/package/termosos
TERMOSOS_SITE_METHOD = local
TERMOSOS_LICENSE = MIT

# For kernel modules
TERMOSOS_MODULE_SUBDIRS = driver
#TERMOSOS_MODULE_MAKE_OPTS = KVERSION=$(LINUX_VERSION_PROBED)

define TERMOSOS_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define TERMOSOS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/udpserver/udpserver $(TARGET_DIR)/usr/sbin
	$(INSTALL) -D -m 0755 $(@D)/udpserver/udp_echo_client $(TARGET_DIR)/usr/bin
	$(INSTALL) -D -m 0755 $(@D)/driver/S98dht11_driver $(TARGET_DIR)/etc/init.d
endef

$(eval $(kernel-module))
$(eval $(generic-package))
