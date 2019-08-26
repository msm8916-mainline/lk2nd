# lk2nd
lk2nd is a bootloader for Qualcomm MSM devices (at the moment only MSM8916),
based on the [CodeAurora Little Kernel fork](https://source.codeaurora.org/quic/la/kernel/lk/).
It provides an Android Fastboot interface on devices where the stock bootloader
does not provide Fastboot (e.g. Samsung).

lk2nd does not replace the stock bootloader. It is packaged into an Android
boot image and then loaded by the stock bootloader as a "secondary" bootloader.
The real Android boot image is placed into the boot partition with 1 MB offset,
and then loaded by lk2nd.

## Supported SoCs
- MSM8916

### Supported devices
- Samsung Galaxy A3 (2015) - SM-A300FU
- Samsung Galaxy A5 (2015) - SM-A500FU
- Wileyfox Swift

## Installation
1. Download `boot.img` (available in [Releases](https://github.com/msm8916-mainline/lk2nd/releases))
2. Flash `boot.img` using the stock flashing procedure:
  - Fastboot: `fastboot flash boot boot.img`
  - Samsung: `heimdall flash --BOOT boot.img`

## Usage
lk2nd provides the standard Android fastboot protocol for flashing/booting Android boot images.

`fastboot flash lk2nd lk2nd.img` can be used to update lk2nd directory from its
fastboot interface.

**Note:** `fastboot flash boot boot.img` will flash the actual boot image with 1 MB offset
into the boot partition. This is done to avoid replacing lk2nd (since it is also booted from
the boot partition).

## Building
```
$ make TOOLCHAIN-PREFIX=arm-none-eabi- msm8916_secondary
```

**Requirements:**
- ARM (32 bit) GCC tool chain
  - Arch Linux: `arm-none-eabi-gcc`
- [Device Tree Compiler](https://git.kernel.org/pub/scm/utils/dtc/dtc.git)
  - Arch Linux: `dtc`

Replace `TOOLCHAIN_PREFIX` with the path to your tool chain.

## Porting
### To other MSM8916 devices
- Add a simple device tree to `dts/`. You just need `model` and the
  `qcom,msm-id`/`qcom,board-id` from downstream.

### To other SoCs
- Cherry-pick changes
- Make some changes

(TODO: Document this properly)

## Contact
Ping `minecrell`/`Mis012` on [`#postmarketos-mainline`](https://wiki.postmarketos.org/wiki/Matrix_and_IRC).

## License
GPL-2.0-only
