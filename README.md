# lk2nd
lk2nd is a custom bootloader for many different devices based on Qualcomm
system-on-chips (SoCs), including smartphones, tablets, smartwatches and a few
single board computers (SBCs). It simplifies booting standard operating systems
by providing:

  - **A unified boot and flashing interface** (Android Fastboot), independent of
    device-specific modifications made to the boot chain (e.g. the proprietary
    flashing interface on Samsung devices)
  - **Automatic device and hardware detection** (e.g. display panels, batteries, ...)
  - **Additional bootloader features** (e.g. file system boot, booting from SD card)
  - **Additional firmware features** (e.g. booting secondary CPU cores if the firmware
    lacks [PSCI] support)
  - **Workarounds for various device-specific issues** (e.g. USB on some Samsung
    devices, modem on Motorola smartphones, ...)
  - **Various debugging commands** that allow better understanding of the hardware
    for research and development purposes

There are two different configurations:

  - **lk2nd:** "secondary" bootloader intended for devices where existing
    firmware cannot be replaced easily (most smartphones and tablets). In this
    configuration, lk2nd does **not** replace the stock bootloader. Instead, it
    is packed into an Android boot image, which is then loaded by the stock
    bootloader just like the original Android image.
    The real operating system can be placed in the boot partition with 512 KiB
    offset or stored in a ext2 file system. It does not have to be Android or
    even Linux, any kind of kernel can be packed into an Android boot image.

  - **lk1st:** primary bootloader intended for single-board computers (SBCs)
    and expert users. In this case, it is the "first" bootloader responsible for
    loading the main operating system.

This repository is a fork of the [original open-source bootloader from Qualcomm](
https://source.codeaurora.org/quic/la/kernel/lk/) (tag `LA.BR.1.2.9.1-02310-8x16.0`),
which is a heavily modified version of the [Little Kernel Embedded Operating System].

## Supported SoCs
- `lk2nd-msm8916`: APQ8016, MSM8216, MSM8916, MSM8929, MSM8939
- `lk2nd-msm8974`: MSM8974
- `lk2nd-msm8226`: APQ8026, MSM8226, MSM8926

See [Chipsets](https://github.com/efidroid/projectmanagement/wiki/%5BReference%5D-Chipsets)
page on the EFIDroid wiki for an exact mapping of LK targets to SoCs.

## Supported devices
### lk2nd-msm8916
- Acer Iconia Talk S A1-724 (quirky - see comment in `dts/msm8916/msm8916-mtp-smb1360.dts`)
- Alcatel OneTouch Idol 3 (4.7) - 6039*
- Alcatel OneTouch Idol 3 (5.5) - 6045*
- ARK Benefit A3 - peach
- Asus Zenfone 2 Laser (720p) - Z00L
- Asus Zenfone 2 Laser (1080p) - Z00T
- Asus Zenfone Max ZC550KL (2016) - Z010D
- BQ Aquaris M5 - piccolo (use `lk2nd-msm8916-appended-dtb.img`)
- BQ Aquaris X5 - paella, picmt
- DragonBoard 410c - apq8016-sbc
- GPLUS FL8005A
- HTC One M8s - m8qlul (quirky - see comment in `dts/msm8916/msm8939-htc-m8qlul.dts`)
- Huawei Ascend G7 - G7-L01
- Huawei Honor 5X - kiwi
- Huawei Y635 - Y635-L01 (quirky - see comment in `dts/msm8916/msm8916-huawei-y635-l01.dts`)
- Lenovo A6000
- Lenovo A6010
- Lenovo PHAB Plus - PB1-770M, PB1-770N, PB1-770P
- Lenovo Vibe K5 - A6020a40, A6020a41
- LG K10 (m216) - K420
- LG Leon LTE (c50) - H340, H342, H343, H345, MS345, etc
- LYF Wind 1 - ls5010
- Marshall London
- Motorola Moto E (2015) - surnia
- Motorola Moto G (2015) - osprey
- Motorola Moto G4 Play - harpia
- Samsung Galaxy A3 (2015) - SM-A300F, SM-A300FU
- Samsung Galaxy A5 (2015) - SM-A500F, SM-A500FU, SM-A500H, SM-A500YZ
- Samsung Galaxy A7 (2015) - SM-A700YD
- Samsung Galaxy Ace 4 - SM-G357FZ (quirky - see comment in `dts/msm8916/msm8916-samsung-r02.dts`)
- Samsung Galaxy Core Max - SM-G5108Q (quirky - see comment in `dts/msm8916/msm8916-samsung-r08.dts`)
- Samsung Galaxy Core Prime LTE - SM-G360F, SM-G360G (rossaltezt is quirky - see comment in `dts/msm8916/msm8916-samsung-r03.dts`)
- Samsung Galaxy E7 - SM-E7000
- Samsung Galaxy Grand Max - SM-G720AX
- Samsung Galaxy Grand Prime - SM-G530FZ, SM-G530H, SM-G530W, SM-G530Y (G530Y is quirky - see comment in `dts/msm8916/msm8916-samsung-r11.dts`)
- Samsung Galaxy J3 (2016) - SM-J3109, SM-J320YZ
- Samsung Galaxy J3 Pro - SM-J3110, SM-J3119
- Samsung Galaxy J5 (2015) - SM-J5007, SM-J5008, SM-J500F, SM-J500FN, SM-J500H, SM-J500M
- Samsung Galaxy J5 (2016) - SM-J5108, SM-J510F, SM-J510FN, SM-J510UN
- Samsung Galaxy J7 (2015) - SM-J7008, SM-J700P
- Samsung Galaxy On7 (2015) - SM-G6000
- Samsung Galaxy S4 Mini Value Edition - GT-I9195I
- Samsung Galaxy Tab 4 10.1 (2015) - SM-T533
- Samsung Galaxy Tab A 8.0 (2015) - SM-T355, SM-T355Y, SM-T357W
- Samsung Galaxy Tab A 9.7 (2015) - SM-T550, SM-T555
- Samsung Galaxy Tab E 9.6 WiFi (2015) - SM-T560NU
- Vodafone Smart prime 6
- Wileyfox Swift - crackling
- Xiaomi Mi 4i - ferrari
- Xiaomi Redmi 2 - wt86047, wt88047
- Yamada EveryPad III (rebrand of Lenovo PHAB Plus)
- Zhihe-series 4G Modem Stick - ufi-001c(b), uf896

### lk2nd-msm8974
- Fairphone 2 - FP2
- LG G3 - D855
- LG Google Nexus 5 - hammerhead D820, D821
- Samsung Galaxy S5 - SM-G900F
- Samsung Galaxy S5 China Unicom (Duos) - SM-G9006V/W

### lk2nd-msm8226
- ASUS ZenWatch 2 - sparrow
- Huawei Ascend G6 4G - G6-L11 (quirky - see comment in `dts/msm8226/msm8926-huawei-g6-l11-vb.dts`)
- Huawei Watch - sturgeon
- LG G Watch R - lenok
- Samsung Galaxy Grand 2 - SM-G7102
- Samsung Galaxy Tab 4 10.1 (2014) - SM-T530, SM-T535

## Installation
1. Download `lk2nd.img` (available in [Releases](https://github.com/msm8916-mainline/lk2nd/releases))
2. Flash `lk2nd.img` using the stock flashing interface:
    - Fastboot: `fastboot flash boot lk2nd.img`
    - Samsung: `heimdall flash --BOOT lk2nd.img`

If you get `fastboot: error: Couldn't parse partition size '0x'` try one of the following workarounds:

- `fastboot flash:raw boot lk2nd.img`
- `fastboot boot lk2nd.img`, then `fastboot flash lk2nd lk2nd.img`

## Usage
lk2nd provides the standard Android fastboot protocol for flashing/booting Android boot images.

Press `Volume Down` while booting to enter Fastboot mode.
Press `Volume Up` while booting to boot into Recovery mode.

**Note:** If your stock bootloader uses the same key combinations, you need to wait a bit before
pressing the volume keys. Usually, waiting until the screen turns on and/or the device vibrates
should be enough to make the stock bootloader ignore the keys.

`fastboot flash lk2nd lk2nd.img` can be used to update lk2nd directly from its
fastboot interface.

**Note:** `fastboot flash boot boot.img` will flash the actual boot image with 512 KiB offset
into the boot partition. This is done to avoid replacing lk2nd (since it is also booted from
the boot partition).

Other fastboot commands work normally.

### Troubleshooting
If the device shows up via fastboot you can get a log file from lk2nd using
`fastboot oem lk_log && fastboot get_staged <output-file>`, where `<output-file>`
is either some text file to write to (e.g. `output.txt`) or `/dev/stdout` to
write the log to standard output.

## Building
Check [Supported SoCs](#supported-socs) for the make target you should use below.
(It depends on the SoC of your device.)

```
$ make TOOLCHAIN_PREFIX=arm-none-eabi- lk2nd-msmXXXX
```

**Requirements:**
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

Replace `TOOLCHAIN_PREFIX` with the path to your tool chain.
`lk2nd.img` is built and placed into `build-lk2nd-msmXXXX/lk2nd.img`.

**Note:** Unlike lk2nd, lk1st is still experimental and therefore not described
here yet.

## Porting
### To other devices with a supported SoC
- Add a simple device tree to `dts/`. You just need `model` and the
  `qcom,msm-id`/`qcom,board-id` from downstream.

### To other SoCs
Qualcomm maintains separate branches for various groups of SoCs. The branches can
be seen on the [Chipsets](https://github.com/efidroid/projectmanagement/wiki/%5BReference%5D-Chipsets)
page on the EFIDroid wiki. This version of lk2nd is based on the LA.BR branch for
MSM8916. There is a [fork for MSM8953 based on the LA.UM branch](https://github.com/msm8953-mainline/lk2nd).

The bootloader will work best when you use the correct branch for your device.
Older platforms are usually kept around by Qualcomm but barely tested and may
not work, or not even compile.

However, if make files for your SoC are present in this version or the MSM8953 fork
you can try to enable it and see if it works well enough for you. Otherwise you
would need to go through the Git history and pick the relevant commits to another
branch from https://source.codeaurora.org/quic/la/kernel/lk/.

To enable support for a SoC that is already present in this repository:

1. Create a new `project/lk2nd-<target>.mk` which looks like the others.
2. Try to compile it and fix all the compile errors.
3. Try to run it and hope that it works.

Good luck!

## Contact
Ask on [`#postmarketos-mainline`](https://wiki.postmarketos.org/wiki/Matrix_and_IRC) (Matrix or IRC).

[PSCI]: https://developer.arm.com/architectures/system-architectures/software-standards/psci
[Little Kernel Embedded Operating System]: https://github.com/littlekernel/lk
