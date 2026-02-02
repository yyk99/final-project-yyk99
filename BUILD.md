# Custom buildroot project for raspi 3B

The manual setup

## Build

	cd buildroot
	make defconfig BR2_DEFCONFIG=../dot.config BR2_EXTERNAL=../yyk99_extra
	make menuconfig
	make

## Initial setup

	git clone ...
	git submodule --init --recursive

## Save Config

Once the build was successful, save the config

	make -C buildroot BR2_DEFCONFIG=../dot.config savedefconfig

## Troubleshooting

fakeroot: preload library `libfakeroot.so' not found, aborting.

Solution 5: Buildroot-Specific Fix

If this happens during Buildroot build:

	make host-fakeroot-dirclean
	make host-fakeroot
