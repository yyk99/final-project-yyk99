#
# iot_in_c (IOT_IN_C)
#
IOT_IN_C_VERSION = 1.0
IOT_IN_C_SITE = $(BR2_EXTERNAL_yyk99_extra_PATH)/package/iot_in_c
IOT_IN_C_SITE_METHOD = local
IOT_IN_C_LICENSE = MIT
#IOT_IN_C_CONF_OPTS = -DBUILD_DEMOS=ON
IOT_IN_C_DEPENDENCIES = libbcm2835-dev host-pkgconf

#define IOT_IN_C_BUILD_CMDS
#	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
#endef
#
#define IOT_IN_C_INSTALL_TARGET_CMDS
#	$(INSTALL) -D -m 0755 $(@D)/udpserver/udpserver $(TARGET_DIR)/usr/sbin
#	$(INSTALL) -D -m 0755 $(@D)/udpserver/udp_echo_client $(TARGET_DIR)/usr/bin
#	$(INSTALL) -D -m 0755 $(@D)/driver/S98dht11_driver $(TARGET_DIR)/etc/init.d
#endef

$(eval $(cmake-package))
