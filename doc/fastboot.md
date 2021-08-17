# lk2nd fastboot interface

lk2nd provides a standard fastboot interface for flashing system images.
Additionally lk2nd provides a set of additional oem commands that are described in this file.
Not all of those commands may be available depending on the platform, use
`oem help` to see what is available in your lk2nd.

## Common extended fastboot commands

- `oem dump [PARTITION]` Dump a specific partition into staged storage.
- `oem lk_log` Copy log into staged storage.
- `oem screenshot` Copy the framebuffer into the staged storage.
- `oem reboot-edl` Reboot the device into the Emergency Download mode.
- `oem dump-regulators` Get regulators state.
- `oem dump-boot-room` Copy the boot-room into the staged storage.
- `oem dump-saw2` Get SAW2 configuration.

Data from staged storage can be downloaded using `fastboot get_staged [FILE]` command.
*FILE* can also be `/dev/stdout` to print the output to the console.  
Some useful pipelines are:
- `fastboot oem lk_log && fastboot get_staged /dev/stdout`
- `fastboot oem screenshot && fastboot get_staged /dev/stdout | convert - lk2nd.png`

## lk2nd specific commands

- `oem dtb` Stage FDT.
- `oem cmdline` Stage cmdline.
- `oem smb1306-reload` Reload it.

## msm8916 specific commands

- `oem dump-speedbin` Print speed binning of the SoC.

