# Building lk2nd

## Requirements
- `make`
- Python 3: `python3`
- ARM (32 bit) GCC tool chain
  - Arch Linux: `arm-none-eabi-gcc`
  - Alpine Linux and postmarketOS: `gcc-arm-none-eabi`
  - Debian and Ubuntu: `gcc-arm-none-eabi`
  - Fedora: `arm-none-eabi-gcc-cs`
- [Device Tree Compiler](https://git.kernel.org/pub/scm/utils/dtc/dtc.git)
  - Arch Linux: `dtc`
  - Alpine Linux and postmarketOS: `dtc`
  - Debian and Ubuntu: `device-tree-compiler`
  - Fedora: `dtc`
- libfdt
  - Alpine Linux and postmarketOS: `dtc-dev`
  - Debian and Ubuntu: `libfdt-dev`
  - Fedora: `libfdt-devel`
- GNU tar
  - Alpine Linux and postmarketOS: `tar`
- Optional requirements when using image signing
  - Arch Linux: `python-pyasn1-modules python-pycryptodome`
  - Alpine Linux and postmarketOS: `py3-asn1-modules py3-pycryptodome`
  - Debian and Ubuntu: `python3-pyasn1-modules python3-pycryptodome`
  - Fedora: `python3-pyasn1-modules python3-pycryptodomex`

## Building lk2nd

Check targets.md for the make target you should use below.
(It depends on the SoC of your device.)

```
$ make TOOLCHAIN_PREFIX=arm-none-eabi- lk2nd-msmXXXX
```

Replace `TOOLCHAIN_PREFIX` with the path to your tool chain.
`lk2nd.img` is built and placed into `build-lk2nd-msmXXXX/lk2nd.img`.

## Building lk1st

**Note:** Unlike lk2nd, lk1st is still experimental and therefore not described
here yet.

## Additional build flags

lk2nd build system provides few additional compile time settings that you can add
to the `make` cmdline to enable some additional features.

### General settings

#### `DEBUG=` - Log level

Set to 0 to suppress most of the logging, set to 2 to enable excessive log messages
(may slow down boot). Default is 1.

#### `DEBUG_FBCON=` - Enable logging to the display

Set to 1 to make lk2nd print the logs on the screen.

#### `LK2ND_VERSION=` - Override lk2nd version string

By default lk2nd build system will try to get the version from git. If you need
to override the version (i.e. if you want to package lk2nd build), set this varable.

#### `LK2ND_FORCE_FASTBOOT=` - Force lk2nd to boot into fastboot menu

By setting this option to 1 lk2nd will always enter the menu upon boot instead of
continuing with the usual workflow. This is useful for debugging and development.

#### `LK2ND_FASTBOOT_DELAY=` - Add delay before booting via `fastboot boot` (ms)

This can help with debugging on devices with carkit uart.
You need to switch the cable before starting linux to see all the logs.

### lk2nd specific

#### `LK2ND_ADTBS=`, `LK2ND_QCDTBS=`, `LK2ND_DTBS=` - Only build listed dtbs

### lk1st specific

#### `LK2ND_COMPATIBLE=` - Board compatible

Set the board compatible value. 

#### `LK2ND_BUNDLE_DTB=` - Board dtb file

Use this dtb for lk1st build.

#### `LK2ND_DISPLAY=` - Display panel driver

Set specific panel driver. By default it uses `cont-splash`.

### Signing of images

#### `SIGN_BOOTIMG=` - Sign `lk2nd.img` after build

Set to 1 to have the resulting `lk2nd.img` be signed for AVB1. By default
images are not signed.

#### `BOOTIMG_CERT=` and `BOOTIMG_KEY=` - Set signing credentials used to sign images

Set the signing certificate and private key to use for signing boot images.
If signing is enabled and no keys are specified a pair of default test keys
from the AOSP 13 source tree located in `lk2nd/certs/` are used to sign the
image.
`BOOTIMG_CERT` is expected to be in X.509 PEM format and `BOOTIMG_KEY` is
expected to be in PKCS#8 format.
