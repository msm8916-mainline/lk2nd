# lk2nd fastboot commands

lk2nd includes a fastboot implementation and provides a selection of additional
commands for debugging and development.

> [!NOTE]
> Not all fastboot commands may be enabled on a given build of lk2nd.
> Use `fastboot oem help` to find which commands are available.

- `oem hash` - Hash staged data using hardware crypto.
- `oem log` - Stage lk log.
- `oem reboot-edl` - Reboot into EDL mode.
- `oem screenshot` - Stage a screenshot.
- `oem debug cpuid` - Dump CPUID registers.
- `oem debug (read|write)(b|hw|l|q|pmic)` - Peek/Poke memory.
- `oem debug spmi-regulators` - Dump regulstors state.
