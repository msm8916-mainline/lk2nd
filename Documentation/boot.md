# lk2nd boot process

lk2nd provides mutliple ways to boot, each of which has advantages and limitations.

## Android boot.img

lk2nd is based on CAF lk and includes aboot implementation. It can boot an android
boot image stored at 512k offset from the start of the `boot` partition (in lk2nd)
or withoug the offset (for lk1st). The offset is required so the next stage image
doesn't overwrite the lk2nd itself.

## Fastboot boot

lk2nd provides a fastboot interface and allows one to boot an OS via USB. Same as
the previous option, this comes from CAF lk and uses android boot images.

```
fastboot boot boot.img
```

## extlinux.conf

lk2nd provides a very rudimentary support for `extlinux.conf` file, similar to
u-boot and depthcharge.

The file shall be stored at an `ext2` formated partition as `/extlinux/extlinux.conf`.
lk2nd will search for it on any partition that is bigger than 16MiB and on a `boot`
partition (offset by 512k in lk2nd).

Following commands are supported:

- `linux <kernel>` - Path to the kernel image.
- `initrd <initramfs>` - Path to the initramfs file.
- `fdt <devicetree>` - Path to the devicetree.
- `fdtdir <directory>` - Path to automatically find the DT in.
- `append <cmdline>` - Cmdline to boot the kernel with.

> [!NOTE]
> lk2nd includes only a very rudimentary extlinux support at this time.
> only commands listed above are considered and the file should contain only one
> entry. All other commands are ignored.

### Example extlinux.conf

This exapmle showcases a correct extlinux.conf file even though lk2nd will only
read the above listed commands. Commands like `timeout` and `label` are currently
ignored.

```
timeout 1
menu title Boot the OS
default MyOS

# Bootloader should pick the DT
label MyOS
    linux /vmlinuz
    initrd /initramfs
    fdtdir /dtbs
    append earlycon console=ttyMSM0,115200
```

## lk2nd cmdline arguments

lk2nd can read OS cmdline argument and make some decisions while booting it.

> [!IMPORTANT]  
> lk2nd reads those values from the OS it boots, not from it's own cmdline.

- `lk2nd.pass-simplefb(=...)` - Add simplefb node to the dtb.
  If `autorefresh` is set, display autorefresh for command mode panels will be enabled.
  If `xrgb8888` or `rgb565` is set, the display mode will be switched to selected one.
  If `relocate` is set, the framebuffer address will be changed to a large reasonably
  safe region. Options can be combined. (i.e. `...=xrgb8888,autorefresh`)
- `lk2nd.pass-ramoops(=zap)` - Add ramoops node to the dtb. If `zap` is set, clear
  the region before booting. Use `fastboot oem ramoops ...` commands to get the data.
- `lk2nd.spin-table=force` - Force enable spintable even if PSCI is available.
