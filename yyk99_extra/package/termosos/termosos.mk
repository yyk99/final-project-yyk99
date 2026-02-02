TERMOSOS_VERSION = 1.0
TERMOSOS_SITE = $(BR2_EXTERNAL_yyk99_extra_PATH)/package/termosos
TERMOSOS_SITE_METHOD = local
TERMOSOS_LICENSE = MIT

# For kernel modules
#MYMODULE_MODULE_SUBDIRS = .
#MYMODULE_MODULE_MAKE_OPTS = KVERSION=$(LINUX_VERSION_PROBED)

define TERMOSOS_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define TERMOSOS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/udpserver/udpserver $(TARGET_DIR)/usr/sbin
	$(INSTALL) -D -m 0755 $(@D)/udpserver/udp_echo_client $(TARGET_DIR)/usr/bin
endef

#$(eval $(kernel-module))
$(eval $(generic-package))


# # example from the documentation

# LIBFOO_VERSION = 1.0
# LIBFOO_SOURCE = libfoo-$(LIBFOO_VERSION).tar.gz
# LIBFOO_SITE = http://www.foosoftware.org/download
# LIBFOO_LICENSE = GPL-3.0+
# LIBFOO_LICENSE_FILES = COPYING
# LIBFOO_INSTALL_STAGING = YES
# LIBFOO_CONFIG_SCRIPTS = libfoo-config
# LIBFOO_DEPENDENCIES = host-libaaa libbbb

# define LIBFOO_BUILD_CMDS
#      $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
# endef

# define LIBFOO_INSTALL_STAGING_CMDS
#      $(INSTALL) -D -m 0755 $(@D)/libfoo.a $(STAGING_DIR)/usr/lib/libfoo.a
#      $(INSTALL) -D -m 0644 $(@D)/foo.h $(STAGING_DIR)/usr/include/foo.h
#      $(INSTALL) -D -m 0755 $(@D)/libfoo.so* $(STAGING_DIR)/usr/lib
# endef

# define LIBFOO_INSTALL_TARGET_CMDS
#      $(INSTALL) -D -m 0755 $(@D)/libfoo.so* $(TARGET_DIR)/usr/lib
#      $(INSTALL) -d -m 0755 $(TARGET_DIR)/etc/foo.d
# endef

# define LIBFOO_USERS
#     foo -1 libfoo -1 * - - - LibFoo daemon
# endef

# define LIBFOO_DEVICES
#     /dev/foo c 666 0 0 42 0 - - -
# endef

# define LIBFOO_PERMISSIONS
# 	/bin/foo f 4755 foo libfoo - - - - -
# endef

# $(eval $(generic-package))
