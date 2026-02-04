################################################################################
#
# libbcm2835-dev (LIBBCM2835_DEV)
#
################################################################################
LIBBCM2835_DEV_VERSION = 1.73
LIBBCM2835_DEV_SOURCE = bcm2835-$(LIBBCM2835_DEV_VERSION).tar.gz
LIBBCM2835_DEV_SITE = http://www.airspayce.com/mikem/bcm2835
LIBBCM2835_DEV_INSTALL_STAGING = YES
LIBBCM2835_DEV_INSTALL_TARGET = YES
#LIBBCM2835_DEV_CONF_OPTS = --disable-shared
#LIBBCM2835_DEV_DEPENDENCIES = libglib2 host-pkgconf

$(eval $(autotools-package))
