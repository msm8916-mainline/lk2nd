# lk2nd device-tree model

lk2nd uses control DT to store the per-device configuration. Often the initial
bootloader on the device doesn't allow to pick unique DT for each device, so
lk2nd relies on a special DT model to allow multiple devices share the same DT
file.

> [!NOTE]
> lk2nd DT format is custom and is not compatible with industry-standard DT
> bindings used by Linux. lk2nd, however, tries to keep it's custom bindings
> similar to industry-standard where it makes sense.

## DT layout

Generally an lk2nd file contains two parts:
 - Root node. This node contains information needed by the previous bootloader.
 - lk2nd node. This node contains all device-specific information used by lk2nd.

### Root node

This section would generally only contain msm-id and board-id. For lk2nd this node
should only contain the properties or nodes that the stock bootloader wants to see
to boot the image. lk2nd generally doesn't use this data.

```
/ {
	qcom,msm-id = <QCOM_ID_MSM8916 0>;
	qcom,board-id = <0xCE08FF01 1>, <0xCE08FF01 3>, <0xCE08FF01 7>;
};
```

### lk2nd node

This section would contain information about all the devices supported by this
specific dt file. Each supported device is listed there in a dedicated subnode.
If it's well-known that the dt file will only be matched by a single device, then
the device-specific information may be placed into the top level lk2nd node.

```
&lk2nd {
	a3lte {
		model = "Samsung Galaxy A3 (SM-A300F)";
		/* ... */
	};

	gt58lte { /* ... */ };
};
```

```
&lk2nd {
	model = "Huawei Y635-L01";
	/* ... */
};
```

## Device node

Device node contains all the information needed by lk2nd.

### Model and compatible

Every device should list it's `model` and list of `compatible` values.
Note that `model` should contain the marketing name of the device (what
was written on the box) and not whatever can be found in the downsteam DT.

Additionally `lk2nd,dtb-files` property can be present with a list of possible
device-tree names for this device. Don't add `qcom/` or	`qcom-` prefix and `.dtb`
suffix to the values since lk2nd will add them as necessary.

```
	gt58lte {
		model = "Samsung Galaxy Tab A 8.0 (LTE, SM-T355)";
		compatible = "samsung,gt58lte", "samsung,gt58";
		lk2nd,dtb-files = "msm8916-samsung-gt58";
	};
```

### Match property

When the DT file contains multiple devices, lk2nd needs a way to know,
on which of them it has booted. There are multple match properties to
help that.

#### By bootloader

lk2nd can check the cmdline from the previous bootloader and match the
`android.bootloader=` argument with an expression:

```
	lk2nd,match-bootloader = "A300F*";
```

#### By cmdline

Like with match-bootloader, but check the whole cmdline for a match:

```
	lk2nd,match-cmdline = "*lenok*";
```

#### By device

lk2nd can check the cmdline from the previous bootloader and match the
`android.device=` argument with an expression:

```
	lk2nd,match-device = "victara";
```

#### By display panel

In some cases there is no useful cmdline param to match against, so
it's possible to use a list of possible display panels and match if
one of them is present. See [Panel selection](#panel-selection).

```
	lk2nd,match-panel;
	panel { /* ... */ };
```

### Panel selection

Many devices ship with one of multiple possible display modules.
lk2nd can patch the device-tree of the OS to insert a compatible
value for the detected display. Please list all possible panels
when you add a new device, if you are planning to make use of this.

```
	panel {
		compatible = "wingtech,wt88047-panel", "lk2nd,panel";

		qcom,mdss_dsi_r69431_720p_video {
			compatible = "wingtech,sharp-r69431";
		};
		qcom,mdss_dsi_nt35521_720p_video {
			compatible = "wingtech,auo-nt35521";
		};
		/* ... */
	};
```

### msm-id override

For some devices it may be needed to let lk2nd know what msm- and
board-id to use when booting the downstream kernel. You can specify
them in the device node and lk2nd will use them instead of autodetecting.

```
	qcom,msm-id = <QCOM_ID_MSM8916 0>;
	qcom,board-id = <0xCE08FF01 1>;
```

### Single-key navigation

On some devices, such as smartwatches, there is only one key available for
navigation. In this case `lk2nd,single-key-navigation` property can be
used to switch the navigation scheme to short/long press of a single key.

```
	lk2nd,single-key-navigation;
```

### Custom menu navigation key hint

If your device uses re-mapped keys you can customize the hint that is shown in
the lk2nd boot menu.

For example, if your device only has `Volume Up` and `Volume Down` keys and no
`Power` key you can add the following optional property:

```
	lk2nd,menu-key-strings = "Volume Down", "Volume Up";
```

The first string describes the key(s) used for navigating the menu, the second
string describes the key used for selecting a menu item.

With the example setting shown above the boot menu would read:

  Volume Down to navigate.
  Volume Up to select.

If the property is not set the default hint will be shown:

  Volume keys to navigate.
  Power key to select.

## Additional drivers

lk2nd provides a set of optional nodes, following a simple driver
model.

### GPIO keys

If a device has non-default keymap for volume and power keys, it
can be overriden by the `gpio-keys` node. This node follows upstream
binding with an exception of the new `lk2nd,code` property.

```
	gpio-keys {
		compatible = "gpio-keys";

		volume-down {
			lk2nd,code = <KEY_VOLUMEDOWN>;
			gpios = <&tlmm 81 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
		};
		home {
			lk2nd,code = <KEY_HOME>;
			gpios = <&pmic 3 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
		};
	};
```

You can also use this to re-map keys to allow for navigating the menu if the
default keys are not available on the device.

The following example is for a device that only has Volume Up and Down keys, but
no Power key. It re-maps the Volume Up key to the Power key to allow selecting
items in the menu.

Note that you may need to unmap a particular key from its original function by
assigning an unused GPIO in its place. In this case the order of nodes in your
device tree matters, so ensure that you unmap the key first before you assign a
new keycode to the actual key.

```
	gpio-keys {
		compatible = "gpio-keys";

		/* HACK: map KEY_VOLUMEUP to non-existent button so the actual
		 * Volume Up button will successfully re-map to KEY_POWER below.
		 */
		volume-up-unmap {
			lk2nd,code = <KEY_VOLUMEUP>;
			gpios = <&pmic_pon 0 0>;
		};

		/*
		 * Remap Volume Up to KEY_POWER to allow selecting menu items as
		 * there is no power button present on the device.
		 * Use Volume Down to navigate the menu and Volume Up to select.
		 */
		volume-up {
			lk2nd,code = <KEY_POWER>;
			gpios = <&tlmm 85 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
		};
	};
```

### GPIO leds

lk2nd supports the `gpio-leds` node. This node follows upstream binding except
that most properties are not supported. Currently only `gpios` and
`default-state` are supported in lk2nd.

```
gpio-leds {
    compatible = "gpio-leds";

    red {
        gpios = <&tlmm 3 GPIO_ACTIVE_HIGH>;
        default-state = "on";
    };

    green {
        gpios = <&tlmm 4 GPIO_ACTIVE_HIGH>;
        default-state = "off";
    };
};
```

This driver is mainly for lk1st since no previous boot-loader is capable to
light up an LED in this case. This makes it unclear what state currently the
device is in. If you wish to make sure some leds are enabled or disabled by
lk1st on boot, you can use this driver.

lk2nd doesn't implement a fully-fledged led subsystem. The only purpose for
this driver is for debugging and the LED behavior consistancy between lk1st
and lk2nd.

### Fixed Regulator

lk2nd supports the `regulator-fixed` node. This node follows upstream binding except
that most properties are not supported. Currently only `gpios` are supported in lk2nd.

```
regulator-backlight {
    compatible = "regulator-fixed";
    gpios = <&tlmm 9 GPIO_ACTIVE_HIGH>;
};
```

This driver is mainly for lk1st since no previous boot-loader is capable to
turn on those regulator that is requied by the panel and backlight. 
If your device panel and backlight requied some regulator to be turned on,
you can use this driver.

lk2nd doesn't implement a fully-fledged regulator subsystem. The only purpose for
this driver is for turning on those regulator that is requied by the panel and backlight
and the regulator behavior consistancy between lk1st and lk2nd.


