# lk2nd DTS format and properties

> The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL
> NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and
> "OPTIONAL" in this document are to be interpreted as described in
> [RFC 2119](https://datatracker.ietf.org/doc/html/rfc2119).

lk2nd uses DeviceTree to describe specific devices that it supports.

DT files are stored in the `/dts` dirrectory, categorised by the target pltform,
e.g. `msm8916`. Each DT file has it's own `board-id` and is responsible for one
or multiple devices. If only one device is described in the DTS file, it's
properties can be decribed in the root node of the tree. For multple devices in
one file subnodes are created for each supported device with one of `match-xxx`
proprties used to choose matching subnods.

Many useful constats are defined in `lk2nd.h` header file, C preprocesor style
includes **MUST** be used for both including headers and dtsi files.

## example.dts

An example dts file that describes two devices is shown below:

```
// SPDX-License-Identifier: GPL-2.0-only

/dts-v1/;

#include <skeleton.dtsi>
#include <lk2nd.h>

/ {
	// This is used by the bootloader to find the correct DTB
	qcom,msm-id = <206 0>;
	qcom,board-id = <100 0>;

	c50 {
		model = "LG Leon LTE";
		compatible = "lg,c50", "qcom,msm8916", "lk2nd,device";
		lk2nd,match-cmdline = "* model.name=LG-H34*";

		lk2nd,keys =
			<KEY_VOLUMEDOWN 107 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>,
			<KEY_VOLUMEUP   108 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>;
	};

	m216 {
		model = "LG K10 (K420n)";
		compatible = "lg,m216", "qcom,msm8916", "lk2nd,device";
		lk2nd,match-cmdline = "* model.name=LG-K420n *";
	};
};
```

# DT Properties

## Base properties
Every dts MUST contain these properties.

### `model`
```
	model = "My Device";
```
This is a pretty name for the device model. It will be shown on
the lk2nd screen.

### `compatible`
```
	compatible = "vendor,codename", "qcom,msm8916", "lk2nd,device";
```
This property is used to find device nodes. First value MUST be the same as
compatible value in the Linux dts. Second value SHOULD be the same as compatible
value of SoC in the Linux tree. Third value MUST be `"lk2nd,device"`.

In the future this property might be used to select DeviceTree for Linux.

## Matching properties
lk2nd has multiple ways to detct on which device in the dts it was started.
If dts file describes multiple devices, one of matching properties MUST be used.

### `lk2nd,match-bootloader`
```
	lk2nd,match-bootloader = "BLVERSION*";
```
This property is used to compare bootloader version string with
the expected one. `*` can be used to match any substring.

### `lk2nd,match-cmdline`
```
	lk2nd,match-cmdline = "* magic=foo *";
```
This property is used to match an arbitrary substring in the cmdline passed
from previous bootloader. `*` can be used.

### `lk2nd,match-panel`
```
	lk2nd,match-panel;
```
This property is used to indicate that panel types should be used to select the
device. See "Panel selection" below.

## Other properties
lk2nd contains some helper properties described below.

### `lk2nd,keys`
```
	lk2nd,keys =
		<KEY_VOLUMEDOWN 107 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>,
		<KEY_VOLUMEUP   108 (GPIO_PULL_UP | GPIO_ACTIVE_LOW)>;
```
This property is used to overide default key configuration in case the device
implements them differently. `lk2nd.h` MUST be included to use constants for
this property. 

## Panel selection
Some device models may have different display panels between individual devices.
panel selection properties are used. Panels are described in the `panel` subnode
in the device node. `compatible` property defines panel compatible in the Linux
DT that should be changed following with multiple subnodes named as panel ID's
that can be provided by device's bootloader with a `compatible` property that
contains compatible string for the required panel driver.

Example:
```
	panel {
		compatible = "longcheer,l8150-panel";

		qcom,mdss_dsi_dijing_ILI9881C_720p_video {
			compatible = "longcheer,dijing-ili9881c";
		};

		qcom,mdss_dsi_booyi_OTM1287_720p_video {
			compatible = "longcheer,booyi-otm1287";
		};
	};
```



