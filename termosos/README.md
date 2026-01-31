# Custom buildroot project for raspi 3B

The manual setup

## Build

	cd buildroot
	make defconfig BR2_DEFCONFIG=../dot.config
	make menuconfig
	make

## Initial setup

- get buildroot repository

```
commit e687e3815f76c1c5ea9fb52b6558bedfe53ab117 (grafted, HEAD -> 2024.02.x, tag: 2024.02.13, origin/2024.02.x)
Author: Colin Evrard <colin.evrard@mind.be>
Date:   Thu Apr 17 15:23:26 2025 +0200

    Update for 2024.02.13

    Signed-off-by: Colin Evrard <colin.evrard@mind.be>
    Signed-off-by: Arnout Vandecappelle <arnout@rnout.be>
origin  https://gitlab.com/buildroot.org/buildroot/ (fetch)
origin  https://gitlab.com/buildroot.org/buildroot/ (push)
```

alternatively the submodule can be added

```
git submodule add --name buildroot https://gitlab.com/buildroot.org/buildroot/ buildroot
git config -f ../.gitmodules submodule.buildroot.shallow true
cd buildroot
git checkout e687e3815f76c1c5ea9fb52b6558bedfe53ab117
git fetch --depth 1
cd ..
git add buildroot
git commit -m "Add buildroot (2024.02.13)"
```

## Save Config

Once a build was successful, save the config

	make -C buildroot BR2_DEFCONFIG=../dot.config savedefconfig

## Troubleshooting

fakeroot: preload library `libfakeroot.so' not found, aborting.

Solution 5: Buildroot-Specific Fix

If this happens during Buildroot build:

	make host-fakeroot-dirclean
    make host-fakeroot
