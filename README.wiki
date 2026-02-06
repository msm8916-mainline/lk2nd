[https://www.qualcomm.com/products/snapdragon-processors-410 MSM8916]/[https://www.qualcomm.com/products/apq8016e APQ8016] (or [[Qualcomm_Snapdragon_410/412_(MSM8916)|Snapdragon 410/412]]) is Qualcomm SoC released in 2014. Support in mainline was originally added for the [https://www.96boards.org/product/dragonboard410c/ Dragonboard 410c], but most of it also works well on other MSM8916-based devices.

== Status ==
The following features provided by the MSM8916 SoC are supported in mainline and should work on most MSM8916-based devices after the device tree has been set up:
* [[wikipedia:Universal asynchronous receiver-transmitter|UART]]
* USB
* Internal/External Storage ([[wikipedia:MultiMediaCard#eMMC|eMMC]]/[[wikipedia:SD card|SD card]])
* WiFi/Bluetooth
* GPU, Display ('''Note:''' There are many different display panels and each needs a custom panel driver...)
* Audio
* Buttons
* Vibration
* Modem (SMS, voice calls with audio, mobile data)

Certain components (e.g. touchscreen, sensors, ...) are device-specific. With a bit of luck, some will be already supported by mainline, others won't.

== Overview ==
The first MSM8916 devices using mainline were added in {{MR|582|pmaports}}. The following components are involved when running mainline on MSM8916 devices.

=== [https://github.com/msm8916-mainline/linux msm8916-mainline/linux] ===
The close-to-mainline [https://www.kernel.org/ Linux] kernel fork with patches that have not been accepted/submitted upstream yet. Patches in the <code>master</code> branch are generally in good shape unless marked otherwise. Submit a PR to add new patches there.

=== [https://github.com/msm8916-mainline/lk2nd msm8916-mainline/lk2nd] ===
The reference bootloader provided by Qualcomm for MSM8916 is open-source and based on [https://github.com/littlekernel/lk Little Kernel (LK)]. Most devices will use this bootloader in a more or less modified form. On Samsung devices for example, the standard Fastboot interface was replaced by their proprietary download mode.

[https://github.com/msm8916-mainline/lk2nd lk2nd] is a fork of the reference bootloader with the goal to provide a unified boot interface on all MSM8916 devices. It provides a standard Fastboot interface (even on Samsung devices!). It also used for a number of mainline quirks (e.g. to set a WiFi/BT MAC address in the device tree; without lk2nd, WiFi/BT does not work out of the box). Eventually, more quirks may be added in the future. It is therefore recommended for all mainline MSM8916 devices. Porting it is generally much easier than porting mainline.

{{note|'''Note:''' lk2nd does not replace your stock bootloader. It is packed into a standard Android boot image and loaded by the stock bootloader from the boot partition. The real boot partition used by lk2nd is then placed with a small offset into the boot partition.}}

=== {{pmaport|soc-qcom-msm8916}} ===
The shared device package in postmarketOS. It contains shared dependencies and configuration files.

== Getting Started ==
=== Requirements ===
* MSM8916-based device
* UART if possible (with a bit of luck you may get USB working without UART)
* (Downstream) Linux kernel source for your device (explaining ways to mainline without kernel source is out of scope for this article)
* Basic knowledge about Linux, Git, C, Device Trees, ...
* Willingness to learn about "mainlining" and to figure out things on your own

=== Before you start ===
{{note|'''Warning:''' Mainlining (or any unintended "modding") may brick your device permanently if you make a mistake. There is a good chance that nothing happens if you are a bit careful, but do not continue with a device that you need every day. You have been warned!}}

Mainlining is not easy. MSM8916 is a platform where a lot components can be easily enabled by only setting up a device tree, which can be largely copied from other devices with minor changes. However, at some point you will reach the point where you would like to enable a particular component that is device-specific (e.g. the touchscreen, sensors, ...). In this case you will be largely on your own, and need to figure out how to enable it yourself. (Do you just need to add something to the device tree or even write a new kernel driver?) That requires some familiarity with the way the Linux kernel is working for MSM8916.

The best way to make yourself familiar with the process is to attempt to figure out some simple things on your own. Therefore, this guide will only describe everything until (eventually) USB network is working. For everything else, this article provides only some information that might be helpful to figure it out yourself.

If you have any questions, ask in [[Matrix_and_IRC|the mainline channel on Matrix or IRC]]. Make sure to mention "msm8916" if your question is specific to this article.

=== Preparations ===
First, let's take a short look at the downstream kernel. The device trees on downstream are usually in <code>arch/arm/boot/dts/qcom/msm8916-*.dts(i)</code> (in Samsung kernels: <code>arch/arm/boot/dts/samsung/msm8916/msm8916-*.dts(i)</code>). You need to find the <code>.dts</code> file that describes the device tree for your device. One way to check this is to look at <code>/proc/device-tree/model</code> on downstream, and search for the same string in that directory.

Now, let's check how it looks on mainline. Clone [https://github.com/msm8916-mainline/linux msm8916-mainline/linux] and take a look at <code>arch/arm64/boot/dts/qcom/</code>. This is the directory where you will add your device tree later.

=== lk2nd ===
{{note|'''Note:''' lk2nd is not strictly required to boot mainline. However, certain features (e.g. WiFi, BT, secondary CPU cores, ...) only work with some additional mainline quirks included in lk2nd. In the future, there may be more features that depend on extra code that needs to be run in the bootloader. Therefore, lk2nd is recommended for all devices, even if your stock bootloader already provides a standard Fastboot interface.}}

[https://github.com/msm8916-mainline/lk2nd msm8916-mainline/lk2nd] does not require any device-specific code. Normally, it should just run out of the box on your device. However, most stock bootloaders require a valid device tree to be present in the Android boot image. To avoid problems, a dummy device tree with the <code>qcom,board-id</code> from the downstream kernel needs to be added to lk2nd.

Before you make any changes, try [https://github.com/msm8916-mainline/lk2nd#building building it]. Then, add a new file for your device to <code>lk2nd/device/dts/msm8916/</code> that looks like:

<syntaxhighlight lang="c">
// SPDX-License-Identifier: GPL-2.0-only
 
#include <skeleton64.dtsi>
#include <lk2nd.dtsi>

/ {
	// This is used by the bootloader to find the correct DTB
	qcom,msm-id = <206 0>; // qcom,msm-id = <247 0>; for APQ8016
	qcom,board-id = /* FIXME: Check your downstream device tree */;
};

&lk2nd {
	model = "Device Name"; // FIXME
	compatible = "<vendor>,<codename>"; // FIXME

    // FIXME: lk2nd,dtb-files = "msm8916-<vendor>-<codename>";

};
</syntaxhighlight>

Note the <code>FIXME</code> lines in the example above. This is where you still need to make changes for your device. For example, replace the <code>model</code> with a display name for your device, and the placeholders in the <code>compatible</code> string.

The <code>qcom,board-id</code> property allows the bootloader to select the correct device tree. MSM8916 devices usually use [[QCDT]] (multiple device tree blobs (dtb) are packaged in one Android boot image). You should be able to find the <code>qcom,board-id</code> in the downstream <code>.dts</code> file for your device.

{{note|Some devices (in particular Samsung) do not implement the [[QCDT]] specification correctly - they all have the same <code>qcom,board-id</code>. In that case, the device cannot be detected only based on the selected DTB. For Samsung devices, <code>msm8916-samsung-r0x.dts</code> covers multiple devices - the actual device is then detected using <code>androidboot.bootloader</code> version passed by the Samsung bootloader.}}

Make sure to add the new file to <code>lk2nd/device/dts/msm916/rules.mk</code>, and re-compile lk2nd.

With a bit of luck, flashing the boot image on your device should make it boot into a lk2nd specific Fastboot screen ([[:File:Lk2nd-0.10.0-seed.png|example]]), and the device should show up via USB. If either of that is not the case, we need to investigate further. Sometimes, there is some kind of [[Micro-USB Interface Controller]] that sits between USB and the SoC, which may require manual setup to work correctly. Setting that up would require device-specific code. If you need help, ask in [[Matrix_and_IRC|the mainline channel on Matrix or IRC]].

{{note|This guide assumes that ARM64 is available on the device but on rare occasions the device might be shipped with old bootloader that only supports 32bit OS. This will be indicated with a red line in lk2nd. In such cases you can try updating your device to the latest firmware. If this doesn't help, ask in [[Matrix_and_IRC|the mainline channel on Matrix or IRC]] for recommendations.}}

=== Device Package ===
Before you can build the mainline kernel for your device, you need to setup the postmarketOS device packages. Avoid creating the device/kernel package directly through pmbootstrap. There is no need to analyze a boot image. (The boot image offsets are not actually used on MSM8916; plus, all MSM8916 devices should use Fastboot through lk2nd...) You can start with the following examples:

<code>device/testing/device-<vendor>-<codename>/APKBUILD</code>:

<syntaxhighlight lang="sh">
# Reference: <https://postmarketos.org/devicepkg>
pkgname=device-<vendor>-<codename>
pkgdesc="Device Name"
pkgver=1
pkgrel=0
url="https://postmarketos.org"
license="MIT"
arch="aarch64"
options="!check !archcheck"
depends="postmarketos-base mkbootimg linux-postmarketos-qcom-msm8916 soc-qcom-msm8916"
makedepends="devicepkg-dev"
source="deviceinfo"

build() {
	devicepkg_build $startdir $pkgname
}

package() {
	devicepkg_package $startdir $pkgname
}
</syntaxhighlight>

<code>device/testing/device-<vendor>-<codename>/deviceinfo</code>:

<syntaxhighlight lang="sh">
# Reference: <https://postmarketos.org/deviceinfo>
# Please use double quotes only. You can source this file in shell scripts.

deviceinfo_format_version="0"
deviceinfo_name="Device Name"
deviceinfo_manufacturer="Vendor"
deviceinfo_codename="<vendor>-<codename>"
deviceinfo_year="<release year>"
deviceinfo_dtb="qcom/msm8916-<vendor>-<codename>"
deviceinfo_append_dtb="true"
deviceinfo_arch="aarch64"

# Device related
deviceinfo_chassis="handset"
deviceinfo_external_storage="true"
deviceinfo_screen_width="720"
deviceinfo_screen_height="1280"
deviceinfo_getty="ttyMSM0;115200"

# MSM DRM cannot take over the framebuffer from the bootloader at the moment
deviceinfo_no_framebuffer="true"

# Bootloader related
deviceinfo_flash_method="fastboot"
deviceinfo_kernel_cmdline="earlycon console=ttyMSM0,115200"
deviceinfo_generate_bootimg="true"
deviceinfo_flash_offset_base="0x80000000"
deviceinfo_flash_offset_kernel="0x00080000"
deviceinfo_flash_offset_ramdisk="0x02000000"
deviceinfo_flash_offset_second="0x00f00000"
deviceinfo_flash_offset_tags="0x01e00000"
deviceinfo_flash_pagesize="2048"
deviceinfo_flash_sparse="true"
</syntaxhighlight>

Follow the [[Porting_to_a_new_device#Device_specific_package|postmarketOS porting guide]] until the device package builds successfully.

=== Build Kernel ===

Go back to your [https://github.com/msm8916-mainline/linux msm8916-mainline/linux] clone. Try to build it with [[Compiling_kernels_with_envkernel.sh|envkernel.sh]]:

<syntaxhighlight lang="shell-session">
$ cd path/to/msm8916-mainline/linux
$ source path/to/pmbootstrap/helpers/envkernel.sh

# Initialize kernel configuration
$ make msm8916_defconfig pmos.config

# Compile kernel; replace <cores> with the number of cores you'd like to use for compilation
$ make -j<cores>

# Create postmarketOS package with your built kernel
$ pmbootstrap build --envkernel linux-postmarketos-qcom-msm8916
</syntaxhighlight>

This is how you can build and test local changes for the kernel from now on. Now we will continue setting up an initial device tree for your device.

Later on, if you want to change the config, for example to build a new display driver as a module, you can run <code>make menuconfig</code>.

=== Initial Device Tree ===
{{Sidebox
| box-text = '''Device Tree''' is a description of the hardware that the specific device has. Kernel will use it to load and configure required drivers. See e.g. [[Mainlining#Materials_to_look_into]] for introductions.
| border-color = #7EABEA
| border-width = 3px
| background = #EBF3FF
| background-dark = #202B3C
}}
The available hardware components of a device are described in the device tree. Components provided by the [[wikipedia:System on a chip|SoC]] (MSM8916) are described in the common include <code>msm8916.dtsi</code>. Components provided by the [[wikipedia:Power management integrated circuit|PMIC]] (PM8916) are described in <code>pm8916.dtsi</code>. Finally, the combination of both is described in <code>msm8916-pm8916.dtsi</code>.

You need to create a new device tree with the device-specific configuration, plus additional components used in your device (e.g. touchscreen).

The device tree for your device should be placed in a new file at <code>arch/arm64/boot/dts/qcom/msm8916-<vendor>-<codename>.dts</code>. A very simple device tree that only enables UART output could look like this:

<syntaxhighlight lang="c">
// SPDX-License-Identifier: GPL-2.0-only

/dts-v1/;

#include "msm8916-pm8916.dtsi"

/ {
 	model = "Device Name"; // FIXME
 	compatible = "<vendor>,<codename>", "qcom,msm8916"; // FIXME
	chassis-type = "handset"; // FIXME

	aliases {
		serial0 = &blsp_uart2;
	};

	chosen {
		stdout-path = "serial0";
	};
};

&blsp_uart2 {
	status = "okay";
};

// make sure gcc is probed with display disabled.
&gcc {
	clocks = <&xo_board>, <&sleep_clk>, <0>, <0>, <0>, <0>, <0>;
};
</syntaxhighlight>

We see two different kind of device tree nodes here. There is the root device tree node (<code>/</code>), which we use to set up some configuration in the <code>chosen</code> node. Then there is the <code>&blsp_uart2</code> node. This is a reference to a device tree node that was defined somewhere under the root node by one of the includes. In this case, <code>msm8916.dtsi</code> sets up lots of pre-defined devices (e.g. for [[wikipedia:Universal asynchronous receiver-transmitter|UART]], [[wikipedia:MultiMediaCard#eMMC|eMMC]], [[wikipedia:SD card|SD card]], ...). We don't care exactly where or how they are defined. We just want to enable them and (if necessary) add device-specific configuration to them.

We can reference existing device tree nodes using ''labels''. The original device tree node for <code>blsp_uart2</code> in <code>msm8916.dtsi</code> looks like this:

<syntaxhighlight lang="c">
/ {
	/* ... */
	soc {
		/* ... */
		blsp_uart2: serial@78b0000 {
			compatible = "qcom,msm-uartdm-v1.4", "qcom,msm-uartdm";
			/* ... */
			status = "disabled";
		};
		/* ... */
	};
	/* ... */
};
</syntaxhighlight>

The part before the colon (<code>blsp_uart2:</code>) is the label we can use to reference the node. We can use that to add additional properties or to change one of the existing ones.
In particular, we can see that <code>msm8916.dtsi</code> declares <code>blsp_uart2</code> (one of the UART ports) as ''disabled'' by default. We want to enable the UART port so we override <code>status = "disabled"</code> with <code>status = "okay"</code>.

Finally, the combination of the <code>aliases</code> and <code>chosen</code> node selects <code>blsp_uart2</code> as default console output.

{{note|'''Note:''' MSM8916 has 122 [[wikipedia:General-purpose input/output|GPIO]] pins that can be configured with different functions. GPIO 4 and 5 can be configured to provide UART (<code>blsp_uart2</code>). Typically, this is the UART port that is used for debug console output. However, there are also some devices that have UART on GPIO 0 and GPIO 1 instead. In that case, you would use <code>blsp_uart1</code>.}}

Before you build, don't forget to add an entry to <code>arch/arm64/boot/dts/qcom/Makefile</code>.
This device tree should be already enough to boot with working serial console if you are able to access UART.
If you don't have UART you will need to skip testing this step and hope that USB works out of the box in the next step.

=== USB ===
Theoretically, enabling USB would be as simple as adding:

<syntaxhighlight lang="c">
&usb {
	status = "okay";
};
</syntaxhighlight>

However, due to an implementation detail USB only works if the USB controller is notified when you actually insert an USB cable. On mainline, these notifications come from the ''extcon'' (external connector) subsystem. Unfortunately, how we can detect that an USB cable was inserted differs quite significantly between devices. Sometimes (mainly on Samsung devices), there is some kind of [[Micro-USB Interface Controller]] that sits between USB and the SoC, which can detect various different cable types (connected to PC, connected to charger adapter, [[wikipedia:USB On-The-Go|USB-OTG]], ...). In most other cases, this notification comes from the charger driver, which is usually quite hard to get working (see [[#Battery/Charging]]).

How to handle some of the common variants is described here. Feel free to ask in the chat for further help.

==== ID pin ====

{{note|Make sure to set gpio to high/low according to <code>qcom,usbid-switch</code> under the <code>&usb_otg</code> node (if the device tree has <code>qcom,usbid-switch</code>). Some device may required this for the usb work. (See <code>usb_id_switch_default</code> node in [[BQ Aquaris M5 (bq-piccolo)]] mainline dts for learn more how to set that pin to high/low in mainline.}}
This is the easiest solution if you have some kind of USB ID GPIO defined downstream. Usually it's called <code>qcom,usbid-gpio</code> under the <code>&usb_otg</code> node but the name varies a bit sometimes. The USB ID pin is used to differentiate between:
{{note|In some case (Some MSM8909 Device) the id pin isn't a gpio pin.}}
* Gadget mode (where you connect the device to a PC)
* Host mode (where you use a special [[wikipedia:USB On-The-Go|USB-OTG]] adapter to connect other USB devices, e.g. an USB keyboard)

It can only detect these two cases, so it does not allow us to detect that an USB cable is actually inserted. However, as a start we can pretend that an USB cable is inserted whenever we are '''not''' in host mode (i.e. whenever there is no [[wikipedia:USB On-The-Go|USB-OTG]] adapter inserted). This means that the USB controller will stay active all the time. This is not much of a problem for getting started, right? :)

For this example, there was <code>qcom,usbid-gpio = <&msm_gpio 110 0>;</code> defined downstream, so the USB ID pin is connected to GPIO 110. Now we declare a new device tree node, which sets up an extcon device using the USB ID GPIO. We put this under the root node (<code>/</code>) because it is an entirely new node (previously we were mainly re-configuring existing device nodes):

<syntaxhighlight lang="c">
#include <dt-bindings/gpio/gpio.h>

/ {
	/* ... */

	usb_id: usb-id {
		compatible = "linux,extcon-usb-gpio";
		id-gpios = <&tlmm 110 GPIO_ACTIVE_HIGH>;
		pinctrl-0 = <&usb_id_default>;
		pinctrl-names = "default";
	};
};
</syntaxhighlight>

Something we have not seen before in this guide is ''pinctrl'' (or Pin Control): As mentioned earlier, MSM8916 has 122 [[wikipedia:General-purpose input/output|GPIO]] pins that can be configured with different functions. The idea is that there are only so many physical pins you can afford to have on an SoC, but you want to expose a lot of features. Thankfully, most of the time you won't need to use all of the features in the same device, which is why SoC manufatcturers get away with multiplexing multiple functions behind one physical pin. The most common use of a pin is as a GPIO (basically a digital input/output line that can be either set high or low, respectively can be sensed to determine the level it was set to by something outside the SoC). But it's also possible to configure other functions (e.g. [[wikipedia:Universal asynchronous receiver-transmitter|UART]] or [[wikipedia:I²C|I²C]]) for some of the pins. Additionally, pins can be configured with [[wikipedia:Pull-up resistor|pull-up/pull-down resistors]] or with a particular [https://www.raspberrypi.org/documentation/hardware/raspberrypi/gpio/gpio_pads_control.md drive strength]. In the Linux kernel, the [https://elinux.org/images/b/b6/Pin_Control_Subsystem_Overview.pdf Pin Control Subsystem] (''pinctrl'') handles that configuration.

In most cases you don't need to figure out yourself if you need pull-down/pull-up or a particular drive strength. We can check on downstream how the pins were configured and translate that to a similar configuration on mainline. While downstream may contain errors (like enabling an internal pull-up on a line with external one), it's proven to work reliably. When in doubt, a datasheet is of immense help. In our example here we find the following line downstream: <code>pinctrl-0 = <&usbid_default>;</code>. We can then search for <code>usbid_default:</code> in the related device tree files (if it exists multiple times, check which includes are actually used for your device). In this case, we find it in <code>msm8916-pinctrl.dtsi</code> and it looks like:

<syntaxhighlight lang="c">
usb-id-pin {
	qcom,pins = <&gp 110>;
	qcom,num-grp-pins = <1>;
	qcom,pin-func = <0>;
	label = "usb-id-pin";
	usbid_default: default {
		drive-strength = <8>;
		bias-pull-up;
	};
};
</syntaxhighlight>

The pin is configured as GPIO (<code>qcom,pin-func = <0></code>) with pull-up and a drive-strength of 8mA.

We can easily define a similar configuration on mainline: To do that, we define a new pin configuration as part of the <code>&tlmm</code> node, and then reference it with <code>pinctrl-names</code>/<code>pinctrl-0</code> as seen above. The above example translates to the following:

<syntaxhighlight lang="c">
&tlmm {
	usb_id_default: usb-id-default-state {
		pins = "gpio110";
		function = "gpio";

		drive-strength = <8>;
		bias-pull-up;
	};
};
</syntaxhighlight>

Finally, we assign our new extcon device to both <code>&usb</code> and <code>&usb_hs_phy</code>, and then USB should be working. :)

<syntaxhighlight lang="c">
&usb {
	extcon = <&usb_id>, <&usb_id>;
	status = "okay";
};

&usb_hs_phy {
	extcon = <&usb_id>;
};
</syntaxhighlight>

{{note|'''Note:''' In the <code>&usb</code> node, the first extcon should point to the one that detects USB gadget mode, while the second one should detect USB host mode. In our case, our extcon device forces USB gadget mode as long as you don't connect an USB-OTG adapter. Later these would potentially be separate extcon devices. For <code>&usb_hs_phy</code> only the one for USB gadget mode should be specified.}}

==== MUIC ====
Especially Samsung devices do not have the USB ID pin connected to a GPIO like explained in the previous section. Instead, there is an additional chip that sits between the USB connector and the USB controller: a [[Micro-USB Interface Controller]] (or: MUIC). The MUIC handles all USB cable related detection and therefore pretty much represents an extcon device as we have set up manually using the USB ID GPIO in the last section.

The disadvantage is that we now need a special driver for the MUIC. Many different chips exist so often a driver is not available immediately, since you usually don't want to write an entire new driver before you know if mainline is booting on your device at all. Anyway, if there is a driver for it already, the approach to make it work would be the following:

# Identify the MUIC chip. Usually you can find it by searching for <code>muic</code> in the downstream device tree. In this example we will use SM5502, which has the <code>extcon-sm5502.c</code> driver in mainline.
# Identify the [[wikipedia:I²C|I²C bus]] it is connected to. (it will probably be an [[wikipedia:I²C|I²C bus]], but in theory it might be e.g [[wikipedia:Serial_Peripheral_Interface|SPI]].) Samsung tends to use i2c-gpio, which emulates the [[wikipedia:I²C|I²C]] protocol on GPIO pins using [[wikipedia:Bit banging|bit banging]]. Note that sometimes they use i2c-gpio even when there is a hardware [[wikipedia:I²C|I²C bus]] available on the same pins. Not sure why they do that. They also sometimes add i2c-gpio-specific dts properties to hw I²C nodes. You can compare the SDA/SCL GPIO with the [https://github.com/torvalds/linux/blob/eff48ddeab782e35e58ccc8853f7386bbae9dec4/arch/arm64/boot/dts/qcom/msm8916-pins.dtsi#L191-L269 pins of the hardware I²C controllers] and if it matches it is probably better to use the hardware I²C controller instead of i2c-gpio.
# Declare a new device tree node for the MUIC device (and eventually the i2c-gpio bus) and reference it as extcon for <code>&usb</code>/<code>&usb_hs_phy</code> like in the <code>usb_id</code> example above.

If the MUIC chip is not supported by a mainline driver (yet), check out the next section ([[#Otherwise]]) for now.

Let's get to the example. The downstream device tree contains:

<syntaxhighlight lang="c">
i2c_9:i2c@9 { /* SM5502 GPIO-I2C */
	compatible = "i2c-gpio";
	i2c-gpio-scl = <&msm_gpio 3 0x00>;
	i2c-gpio-sda = <&msm_gpio 2 0x00>;
	/* ... */

	sm5502@25 {
		compatible = "sm5502,i2c";
		reg = <0x25>;
		interrupt-parent = <&msm_gpio>;
		interrupts = <12 0>;
		/* ... */
	};
};
</syntaxhighlight>

As mentioned earlier, there is actually a hardware I²C bus available on these pins (<code>blsp_i2c1</code>) so we have two options. We can use the hardware I²C bus (preferred if possible):

<syntaxhighlight lang="c">
#include <dt-bindings/interrupt-controller/irq.h>

&blsp_i2c1 {
	status = "okay";

	muic: extcon@25 {
		compatible = "siliconmitus,sm5502-muic";

		reg = <0x25>;
		interrupts-extended = <&tlmm 12 IRQ_TYPE_EDGE_FALLING>;

		pinctrl-0 = <&muic_int_default>;
		pinctrl-names = "default";
	};
};
</syntaxhighlight>

... or we can use i2c-gpio like Samsung:

<syntaxhighlight lang="c">
#include <dt-bindings/gpio/gpio.h>

/ {
	/* ... */

	i2c-muic {
		compatible = "i2c-gpio";
		sda-gpios = <&tlmm 2 (GPIO_ACTIVE_HIGH|GPIO_OPEN_DRAIN)>;
		scl-gpios = <&tlmm 3 (GPIO_ACTIVE_HIGH|GPIO_OPEN_DRAIN)>;

		pinctrl-0 = <&muic_i2c_default>;
		pinctrl-names = "default";

		#address-cells = <1>;
		#size-cells = <0>;

		/* muic: extcon@25 as above... */
	};
};
</syntaxhighlight>

How to define the pinctrl for these nodes is left as an exercise for the reader (see [[#ID pin]] for explanations). Ask if you need any help!

==== Otherwise ====
If you have neither USB ID GPIO nor MUIC, none of the options above will work for you. However, you should be able to use the USB detection of the PM8916 [[wikipedia:Power management integrated circuit|PMIC]] in this case:

<syntaxhighlight lang="c">
&pm8916_usbin {
	status = "okay";
};

&usb {
	extcon = <&pm8916_usbin>;
	dr_mode = "peripheral";
	status = "okay";
};

&usb_hs_phy {
	extcon = <&pm8916_usbin>;
};
</syntaxhighlight>

Typically <code>&pm8916_usbin</code> should be replaced later with a reference to the charging hardware. It is usually the most reliable source of detecting if an USB cable is plugged in.

==== Testing ====
If you do not have (easy) access to UART, you now have the chance to boot your kernel for the first time!

# Make sure to build the kernel (see [[#Build Kernel]]).
# Create a postmarketOS installation: <code>pmbootstrap install</code>
# [[Inspecting_the_initramfs|Enable the <code>debug-shell</code>]] hook. This will provide a telnet console via USB without having to flash the root file system to the internal storage.
# Boot the kernel using <code>pmbootstrap flasher boot</code>. (Alternatively you could flash it using <code>pmbootstrap flasher flash_kernel</code> and reboot, but that's not nearly as convenient during development...)
# With a bit of luck, the device should show up on telnet via USB after booting. Take a look at <code>dmesg</code> to see the kernel log of mainline booting on your device!

Note that you need to remove the <code>debug-shell</code> hook explicitly using <code>pmbootstrap initfs hook_del debug-shell</code> before attempting to boot with the rootfs in the next section.

=== eMMC ===
If you have USB working, it's time to get the eMMC (internal storage) working next. This will allow you to flash the postmarketOS rootfs to get access to SSH instead of the minimal telnet session you had so far.

In most cases, enabling the eMMC should be as simple as adding:

<syntaxhighlight lang="c">
/* Already included in msm8916.dtsi */
&sdhc_1 {
	status = "okay";
};
</syntaxhighlight>

In this case, you don't need to write the pinctrl nodes yourself. Almost all of the devices use the same configuration here so these are shared for all devices in <code>tlmm: pinctrl@1000000</code>, <code>msm8916-pins.dtsi</code>.

That's it! Now you can build the kernel again, flash the rootfs using <code>pmbootstrap flasher flash_rootfs</code> and reboot the device. Make sure to remove the <code>debug-shell</code> hook first using <code>pmbootstrap initfs hook_del debug-shell</code>. If you used <code>pmbootstrap flasher flash_kernel</code> previously, you may want to erase the boot partition to avoid confusion as lk2nd will prefer to use the file from ext2 part of the rootfs instead of that. With a bit of luck, you might boot straight into postmarketOS with working SSH! :)

=== SD card ===
Your device probably has a (micro) SD card slot. If you would like to keep your Android installation, you can flash the postmarketOS rootfs there alternatively. Or you can use it as extra/removable storage for some special files. It should be almost as simple as getting the eMMC working, so grab a micro SD card for testing and try to get it working:

==== Removable ====

<syntaxhighlight lang="c">
#include <dt-bindings/gpio/gpio.h>

&sdhc_2 {
	pinctrl-0 = <&sdc2_default &sdc2_cd_default>;
	pinctrl-1 = <&sdc2_sleep &sdc2_cd_default>;
	pinctrl-names = "default", "sleep";

	cd-gpios = <&tlmm 38 GPIO_ACTIVE_HIGH>;

	status = "okay";
};

&tlmm {
	sdc2_cd_default: sdc2-cd-default-state {
		pins = "gpio38";
		function = "gpio";
		drive-strength = <2>;
		bias-disable;
	};
};
</syntaxhighlight>

==== Non-removable ====

<syntaxhighlight lang="c">
/* Already included in msm8916.dtsi */
&sdhc_2 {
	status = "okay";
};
</syntaxhighlight>

Please take a close look at the comments here. The implementation of SD card detection varies depending on the device. The following two types are common:

* Hotplug detection using <code>cd-gpios</code> (card detect): This allows detecting if the SD card is removed at runtime, so this is often used if the SD card is removable without turning off the device.
* Scanning for a SD card once during bootup: The assumption is that the device is <code>non-removable</code> at runtime, e.g. because it is not accessible without removing the battery. In that case, they were able to omit the card-detect GPIO.

Check your downstream device tree for <code>cd-gpios</code> (usually on the <code>&sdhc_2</code> node). If it exists, uncomment it here and also add <code>&sdc2_cd_on/off</code> to the pinctrl lines. Otherwise, uncomment <code>non-removable</code>.

'''Note:''' Use <code>GPIO_ACTIVE_LOW (= 0x1)</code> for <code>cd-gpios</code> if downstream contains <code>cd-gpios = <&msm_gpio 38 0x1>;</code> Otherwise, use <code>GPIO_ACTIVE_HIGH (0x0)</code>.

That's it! Try to boot it and see if the SD card shows up in <code>lsblk</code> or <code>dmesg</code>. If you want, you can also try to mount it and read/write some files.

=== Buttons ===
As a last part of this guide, let's try to get the hardware buttons working. Your device probably has at least a (mechanical) {{Button|Power}}, {{Button|Volume Up}} and {{Button|Volume Down}} button.

The Power and Volume Down buttons are usually connected to the [[wikipedia:Power management integrated circuit|PMIC]] (PM8916). The Power button is - obviously - used to power on the device. Volume Down is used in combination with the Power button to force a reboot (technically, this behavior is somewhat configurable, but thankfully it is usually set up like this), essentially simulating removing the battery and putting it back in. On devices without removable battery holding these two buttons allows rebooting ("resetting") the device.

On mainline, the <code>pm8941-pwrkey</code> driver implements support for these two buttons. The power button is set up by default in the common <code>pm8916.dtsi</code> include. As mentioned, the Volume Down button is typically connected to the "reset input" (''resin''). Theoretically, a device could have any button (or even some test points on the mainboard) connected there so we need to enable it explicitly and assign <code>KEY_VOLUMEDOWN</code>:

<syntaxhighlight lang="c">
&pm8916_resin {
	linux,code = <KEY_VOLUMEDOWN>;
	status = "okay";
};
</syntaxhighlight>

Volume Up and other additional keys (e.g. Home on Samsung devices) are usually connected to a GPIO pin on the SoC. Therefore, setting them up is trivial using [https://www.kernel.org/doc/Documentation/devicetree/bindings/input/gpio-keys.txt gpio-keys]. Try to find the GPIOs in the downstream device tree and set up something like:

<syntaxhighlight lang="c">
#include <dt-bindings/gpio/gpio.h>
#include <dt-bindings/input/input.h>

/ {
	/* ... */
	gpio-keys {
		compatible = "gpio-keys";

		pinctrl-0 = <&gpio_keys_default>;
		pinctrl-names = "default";

		label = "GPIO Buttons";

		button-volume-up {
			label = "Volume Up";
			gpios = <&tlmm 107 GPIO_ACTIVE_LOW>;
			linux,code = <KEY_VOLUMEUP>;
		};
	};
};
</syntaxhighlight>

Make sure to set up pinctrl properly: typically these buttons need to be configured with <code>bias-pull-up</code>.

To test the buttons, <code>apk add evtest</code>. Then you can select the input devices in <code>evtest</code> and see if an input event show up when you press them.

=== Finishing up ===
If the features enabled during the guide are working, then it is a good time to do some cleanup and propose your changes "upstream" on [https://github.com/msm8916-mainline/linux msm8916-mainline/linux]. This repository is intended as a temporary place for patches that are already in good quality, but either pending or not quite finished to submit them on the Linux mainline mailing lists.

{{note|'''Note:''' The [https://github.com/msm8916-mainline/linux msm8916-mainline/linux] repository is not affiliated to the mainline Linux kernel (as maintained by Linus Torvalds). To get your changes into mainline you will need to submit them on the Linux mainline mailing lists later.}}

==== Device Tree Structure ====
So far you have assembled your device tree from various examples shown in this guide. For consistency with the other MSM8916 device trees, please make sure to put your device tree in the following order:

<syntaxhighlight lang="c">
// SPDX-License-Identifier: GPL-2.0-only

/dts-v1/;

#include ...

/ {
 	model = "Device Name"; // FIXME
 	compatible = "<vendor>,<codename>", "qcom,msm8916"; // FIXME
	chassis-type = "handset"; // FIXME

	aliases {
		mmc0 = &sdhc_1; /* eMMC */
		mmc1 = &sdhc_2; /* SD card */
		serial0 = &blsp_uart2;
	};

	chosen {
		stdout-path = "serial0";
	};

	/* Alphabetically ordered list of new device tree nodes */
};

/* Alphabetically ordered list of device tree node references, e.g. */
&blsp_uart2 {
	status = "okay";
};

&sdhc_1 {
	status = "okay";
};

&sdhc_2 {
	/* ... */
	status = "okay";
};

&usb {
	status = "okay";
};

/* Pinctrl always comes last */
&tlmm {
	/* ... */
};
</syntaxhighlight>

==== Patch Requirements ====

To maintain the quality, all patches are required to:
* Compile without warnings
* Have no critical (<code>ERROR:</code>) checkpatch failures (also see [https://www.kernel.org/doc/html/latest/process/submitting-patches.html#style-check-your-changes Style-check your changes in the mainline "Submitting patches" guide])
** Sometimes it makes sense to ignore notes and warnings from checkpatch if the code is more readable without fixing them.
* Have a properly formatted one-line summary in the commit message (there is no need to write a detailed commit message, yet)
** Look at similar commits in the commit history for examples.
* Comply with the repository's [https://github.com/msm8916-mainline/linux/blob/master/CONTRIBUTING.md contributing guidelines] that also inherit some requirements of upstream Linux.

The first two will be automatically checked for you when you open a PR - the CI build will fail otherwise - so you don't have to check it locally. Eventually, you should expect a few additional review comments to improve your code. This will help you to speed up submission of the patches upstream later! ;)

==== Hints ====
Please check these to reduce the time required to review your patch:
* Add pinctrl for all GPIOs you are using. This is good practice in case they were incorrectly configured by the previous bootloader or operating system (consider kexec booting another Linux kernel).

== Reference ==
The general approach for mainlining is:
* Look at all resources you can find, e.g.:
** [https://github.com/msm8916-mainline/linux/commits/master Commits in msm8916-mainline/linux]
** Existing device trees in mainline and/or commits with relevant changes (sometimes there is helpful information in the commit message)
** The kernel documentation, especially the device tree binding documentation (all available device tree properties should be documented there)
and compare it with what you can find on downstream (e.g. device tree, kernel code, ...). Do '''not''' take the downstream device tree as a base or even copy it as-is. That is the wrong approach. Downstream code tends to be full of mistakes and unnecessarily verbose. It's easier to start from scratch and rather take something in mainline as a base.

Additional tips and tricks:
* When making changes to kernel code compiled as modules, you need to update the kernel modules on the rootfs (they are not part of the boot image that is loaded via Fastboot). There are different options to do this:
** Re-flash rootfs
** Upgrade <code>linux-postmarketos-qcom-msm8916</code> on your device: see [[Installing packages on a running phone]]
** Other hacky solutions like copying kernel modules manually via SSH

=== Firmware ===
Unfortunately, some components in the SoC rely on proprietary [[firmware]] to work properly:

* WiFi/Bluetooth (''wcnss'')
* Modem (''modem'')
* Hardware-accelerated video codecs (''venus'')

Perhaps more unfortunately, typically the firmware is signed with a device-specific key, which means you are forced to continue using the potentially outdated/buggy firmware provided by the stock firmware of your device manufacturer. This means that the firmware must be either loaded from the special partition or packaged separately for each device. The following files are needed:

* <code>wcnss.mdt</code> plus all related segments (<code>wcnss.b00</code>, ...) (usually installed on a separate firmware (<code>NON-HLOS</code>) partition)
* <code>WCNSS_qcom_wlan_nv.bin</code> (usually installed on the system partition, e.g. <code>/system/etc/firmware/wlan/prima/WCNSS_qcom_wlan_nv.bin</code>)
* <code>mba.mbn</code>, <code>modem.mdt</code>, <code>modem.b00</code>, ... (usually installed on a separate firmware (<code>NON-HLOS</code> or <code>modem</code>) partition)
* <code>venus.mdt</code>, <code>venus.00</code>, ... (usually installed on a separate firmware (<code>NON-HLOS</code>) partition, or in <code>/system/etc/firmware/</code>)

First, check if adding <code>msm-firmware-loader</code> package places any blobs to <code>/lib/firmware</code>. If some files from the above list are missing, create a new firmware <code>APKBUILD</code> (see <code>device/community/firmware-wileyfox-crackling</code> for an example). Host the firmware binaries outside of <code>pmaports</code> due to the unclear licensing and import those files as needed.

Then, add a new <code>nonfree-firmware</code> subpackage to your device package:

<syntaxhighlight lang="sh">
subpackages="$pkgname-nonfree-firmware:nonfree_firmware"

nonfree_firmware() {
	pkgdesc="GPU/WiFi/BT/Modem/Video firmware"
	depends="firmware-qcom-adreno-a300 msm-firmware-loader
		 firmware-vendor-device-wcnss-nv firmware-vendor-device-venus"
	mkdir "$subpkgdir"
}
</syntaxhighlight>

Generate the checksum for your firmware package with <code>pmbootstrap checksum firmware-vendor-codename</code> and build it with <code>pmbootstrap build firmware-vendor-codename</code>. Then continue to build and install as you have been doing above.

{{note|'''Note:''' On some devices, the manufacturer did not "properly" set up secure boot. We call these devices ''test-signed'', because typically their firmware was signed with keys Qualcomm has provided for testing (''test keys''). Usually, these devices accept any kind of signed firmware, so you can also use newer firmware from other devices (e.g. from the [https://www.96boards.org/product/dragonboard410c/ Dragonboard 410c]).}}

=== WiFi/Bluetooth ===
WiFi/Bluetooth are usually provided by the Qualcomm Wireless Connectivity Subsystem (WCNSS), which is partially built into the SoC but paired with an external RF module ("iris").<ref>[https://lwn.net/Articles/681614/ <nowiki>[PATCH v2 0/9] Qualcomm WCNSS remoteproc</nowiki>] (patch on Linux mainline mailing list)</ref> Therefore, enabling it is usually as simple as enabling the <code>&wcnss</code> node in the device tree:

<syntaxhighlight lang="c">
&wcnss {
	status = "okay";
};

&wcnss_iris {
	compatible = "qcom,wcn3620";
};

&wcnss_mem {
	status = "okay";
};
</syntaxhighlight>

By default mainline expects [https://developer.qualcomm.com/qfile/29369/lm80-p0436-33_b_wcn3620_wireless_connectivity_ic_device_spec.pdf WCN3620] as external RF module. However, MSM8916 can be also paired with other RF modules - another common chip is [https://developer.qualcomm.com/qfile/35297/lm80-p0436-70_b_wcn3680bwcn3660b_devicespecification.pdf WCN3660B] which has dual-band (5 GHz) support additionally. The chip identifier can be obtained from a downstream kernel log (dmesg): There should be a message like <code>wcnss: IRIS Reg: 04000004</code>.<ref>[https://github.com/msm8916-mainline/android_kernel_qcom_msm8916/blob/b20608408caff817ec874f325127b07609fbaeb8/drivers/net/wireless/wcnss/wcnss_vreg.c#L366 drivers/net/wireless/wcnss/wcnss_vreg.c, line 366] (downstream kernel code)</ref> Take the first 4 hex chars (i.e. <code>>> 16</code>) and compare it with the [https://github.com/msm8916-mainline/android_kernel_qcom_msm8916/blob/b20608408caff817ec874f325127b07609fbaeb8/drivers/net/wireless/wcnss/wcnss_vreg.c#L46-L53 chip IDs listed in downstream].

If you do not have WCN3620, you need to override the <code>compatible</code> property of the iris node:

<syntaxhighlight lang="c">
&wcnss {
	status = "okay";
};

&wcnss_iris {
	compatible = "qcom,wcn3660b";
};

&wcnss_mem {
	status = "okay";
};
</syntaxhighlight>

A few more notes:

* WiFi and Bluetooth share the same antenna, and they tend to conflict with each other when they are used at the same time ("WiFi/BT coexistence"). In case of problems, try disabling one of them.
* Dual-band support on WCN3660B seems to be broken at the moment: only 2.4 GHz networks show up in the WiFi network list.
* <code>WCN3620A</code> (aka 0x5112) does not exist, use the default <code>WCN3620</code> instead

=== Display/GPU ===
{{note|'''Note:''' Due to weird dependencies issues in the DRM MSM driver, you need to make [[#WiFi/Bluetooth|WiFi/Bluetooth]] work to have the DRM MSM driver finish loading successfully. (WCNSS takes a while to load, so it will trigger attempting to probe DRM MSM again after it fails to load on its own due to dependencies issues between the DRM driver and the panel driver...)}}

The display tends to be one of the most difficult things to get working. This is because manufacturers have many different options how to implement the display in the hardware. Generally, all devices will have a display panel connected to the SoC via [[wikipedia:Display Serial Interface|MIPI Display Serial Interface (DSI)]]. The panel can vary even for one device model; it is common to support multiple panels to be more flexible when assembling the device. As if this wasn't enough, panels are usually paired with additional controllers (e.g. for the backlight) or regulators (to provide special power supplies). Those may, or may not need to be controlled from the kernel.

The easy part is usually the panel itself. To make it work we likely only need to send some special DSI commands to initialize it. [https://github.com/freedreno/freedreno/wiki/DSI-Panel-Driver-Porting DSI Panel Driver Porting] on the Freedreno wiki has an introduction how to port panels. For MSM8916 mainlining, we have created [https://github.com/msm8916-mainline/linux-mdss-dsi-panel-driver-generator linux-mdss-dsi-panel-driver-generator] which attempts to automatically parse the downstream device tree and generates panel drivers for all the supported panels.

How can you figure out which panel is being used by the device you have? The panel is usually listed in a <code>mdss_mdp.panel=</code> kernel parameter provided by the (primary) bootloader. You can check it from the lk2nd log, or using <code>fastboot oem cmdline && fastboot get_staged /dev/stdout</code>. The parameter lists the name of the panel node in the device tree.

{{note|'''Note:''' The generated panel driver should usually work without larger changes (follow the README). As usual, it is recommended to ask in the chat for further advice before running it.}}

You also need to add the display panel to your device tree. The panel driver generator also generates a device tree fragment in the <code>.dtsi</code> file that you can use as a base. Typically you just need to fill in the <code>reset-gpios</code>, which can be found downstream. You should also add pinctrl to the <code>&dsi0</code> node. See commits for other devices for examples.

'''Note:''' If you have <code>qcom,regulator-ldo-mode</code> defined downstream you should add ('''do not add it if you do not have this property downstream!'''):

<syntaxhighlight lang="c">
&dsi_phy0 {
	qcom,dsi-phy-regulator-ldo-mode;
};
</syntaxhighlight>

In some cases, you may need to turn on a GPIO to enable the power supply of the panel. On mainline, this is implemented using a [https://github.com/torvalds/linux/blob/master/Documentation/devicetree/bindings/regulator/fixed-regulator.yaml fixed regulator], which represents the hw component that creates the required voltage. You can then use the <code>--regulator</code> option in the panel driver generator to add code to automatically turn on that GPIO.
{{note|'''Note:''' On downstream, you can often run into drivers which pretend that the GPIO is instead routed to some magic "enable" pin of the panel controller. Ideally, you can dispute this by looking at schematics, respectively by looking at datasheet of said controller and attempting to locate said pin.}}

Some devices have an extra backlight IC that needs to be controlled separately in the kernel. The easy case is where it is simply enabled using a GPIO: in this case one can simply treat it like a fixed regulator. (It may, in fact, actually be one.) But some backlight ICs have an [[wikipedia:I²C|I²C interface]] and then need an extra driver in the kernel.

==== Brightness ====
There are different approaches used to dim the brightness of the screen. The approach used in downstream is usually defined by the <code>qcom,mdss-dsi-bl-pmic-control-type</code> property in the panel node of the downstream device tree:

* '''Pulse-width modulation (PWM)''' (<code>bl_ctrl_pwm</code>): In this case there is an external PWM generator that generates a PWM signal for the display. PM8916 (the PMIC of MSM8916) contains one PWM generator that can be configured for the display. <code>arm64: dts: msm8916-longcheer-l8150: Add PWM backlight support</code> is an example of how to set it up.
* '''MIPI DCS commands''' (<code>bl_ctrl_dcs</code>): In this case the panel controller handles the brightness automatically. The generated panel driver should implement brightness without further changes.
* '''Custom''': Some devices (e.g. Samsung AMOLED panels) have <code>bl_ctrl_dcs</code> or something else listed, but actually use an entirely custom way to control display brightness. This needs to be implemented manually in the panel driver (or using an extra backlight driver).

=== Touchscreen ===
The touchscreen is usually connected to an [[wikipedia:I²C|I²C bus]], e.g. <code>blsp_i2c5</code>. Often it is either supplied by some PM8916 regulators or a set of fixed regulators. (This is often visible when the touchscreen has some "enable" GPIOs listed downstream; those usually represent a chip that can be enabled through a GPIO and then supplies a fixed voltage to the touchscreen...) This is where similarities end. There can be all sorts of touchscreens connected to the I²C bus and manufacturers like to use many different ones. Look through the supported touchscreen in mainline to see if a driver exists already, or can be modified eventually to support another version of a similar chip. Otherwise, you may need to write a simple driver yourself.

{{note|'''Note:''' if you are really unlucky (this is probably quite unlikely for a device with MSM8916), you may have a so-called incell touchscreen, that is, a touchscreen that is integrated into the panel. That is not necessarily an issue, but sadly these often have some integration with the display (e.g being configured using dsi commands), that is probably quite ugly and doesn't as of yet have a solution in mainline. If you suspect this to be the case, don't hesitate to ask for help in the chat!}}

=== Sensors ===
Like the touchscreen, sensors are usually connected to an [[wikipedia:I²C|I²C bus]], e.g. <code>blsp_i2c2</code> and usually supplied by L17. This is where similarities end. There can be all sorts of sensors connected to the I²C bus and manufacturers like to use many different ones. Look through the supported sensors in mainline to see if a driver exists already, or can be modified eventually to support another version of a similar chip. Otherwise, you may need to write a simple driver yourself.

Some devices have a [[wikipedia:Hall effect sensor|Hall effect sensor]] that is used to detect if the screen is covered by some kind of flip cover. It is usually a simple GPIO like the [[#Buttons|buttons]], and can be set up easily using [https://www.kernel.org/doc/Documentation/devicetree/bindings/input/gpio-keys.txt gpio-keys].

==== Testing ====
* The sensor should show up in sysfs (<code>/sys/bus/iio/devices/...</code>). Usually there are some <code>in_*_raw</code> files you can read for initial testing.
* '''Accelerometer:''' The accelerometer is used for automatic screen rotation, so let's make sure that works correctly.
** Install <code>iio-sensor-proxy</code> (note: need to restart after installing it), start it (<code>rc-service iio-sensor-proxy start</code>) and use <code>monitor-sensor</code>.
** Check if the orientations are reported correctly. See [[:File:PhoneRotation.png]] for a visualization of the different orientations.
** If the orientations are not correct you need a <code>mount-matrix</code> in your device tree. See e.g. [this explanation https://michael-srba.cz/mainline4life/mount-matrix.html].
* '''Interrupts:''' If your sensor has interrupts listed downstream you should also add them on mainline and check if they work correctly.
** Install <code>linux-tools-iio</code>, then use <code>sudo iio_generic_buffer -a -c -1 -N <num></code> where <code><num></code> is the number of the IIO device in sysfs (e.g. <code>/sys/bus/iio/devices/iio:deviceN</code>.
** It should continuously print weird values.
** Check <code>/proc/interrupts</code> if some interrupts were triggered for the sensor.

=== Audio ===
Audio functionality is split up in 3 main blocks (4 when using the modem):

* '''LPASS''' ('''L'''ow '''P'''ower '''A'''udio '''S'''ub'''s'''ystem)''':''' The "sound card". Reads/writes digital audio data from/to memory and transmits it via [[wikipedia:I²S|I²S]].
* '''Digital Codec''' (<code>msm8916-wcd-digital</code>)''':''' Accepts digital audio via [[wikipedia:I²S|I²S]], post-processes it (e.g. to change volume). Transmits it via PDM to analog codec. (A proprietary Qualcomm protocol but perhaps there are similiarities to [[wikipedia:Pulse-density modulation|Pulse-density modulation]]...)
* '''Analog Codec''' (<code>msm8916-wcd-analog</code>)''':''' Integrated into [[wikipedia:Power management integrated circuit|PMIC]] (PM8916). Contains [[wikipedia:Digital-to-analog converter|DACs]] (playback) and [[wikipedia:Analog-to-digital converter|ADCs]] (capture) and amplifiers for Speaker, Earpiece and Headphones.
* '''Audio [[wikipedia:Digital signal processor|DSP]]''' (<code>qdsp6</code>)''':''' Can optionally sit between CPU and LPASS to offload audio decoding (e.g. MP3, AC, ...). Also handles voice call audio.

Here is diagram how the blocks relate to each other:

 <nowiki>
+---------------------------------------------+                                     
|                   MSM8916               GPIO|              Some devices use       
|                                             |              a custom speaker       
|              +--------+                     |Quaternary        amplifier!         
| +-------+    | Modem  |    +-------+        | MI²S +-------------+                
| |       |<-->| (ADSP) |<-->|       |<--> 112|<---->| Digital     |                
| | Linux |    +--------+    | LPASS |<--> 117|<---->| Audio Codec |--> Speaker     
| | (CPU) |      Memory      |       |<--> 118|<---->| + Amplifier |                
| |       |<---------------->|       |<--> 119|<---->+-------------+                
| +-------+                  +-------+        |                                     
|                      Primary |   ^ Tertiary |    +---------------+                
|                        MI²S  |   |   MI²S   |    |    PM8916     |                
|                              v   |          |PDM |               |                
|                            +-------+<---> 63|<-->|CLK  +--------+|                
|                            |       |<---> 64|<-->|SYNC |        ||<-- Microphone 1
|                            |Digital|<---> 65|<---|TX   | Analog ||<-- Microphone 2
|                            | Codec |<---> 66|--->|RX0  | Codec  ||--> Earpiece    
|                            |       |<---> 67|--->|RX1  |        ||--> Headphones  
|                            +-------+<---> 68|--->|RX2  +--------+|--> Speaker     
+---------------------------------------------+    +---------------+                </nowiki>

'''Note:''' MI²S (multichannel [[wikipedia:I²S|I²S]]) is basically [[wikipedia:I²S|I²S]] with multiple data lines. This allows using more audio channels, although this is typically not used on MSM8916.

{{todo|For now, check related commits for other devices and ask on Matrix if you have any questions :)}}

==== No Custom Speaker Amplifier ====
Example with no custom amplifier: <code>arm64: dts: qcom: msm8916-samsung-serranove: Add sound and modem</code>

==== Custom Speaker Amplifier ====
The analog codec in PM8916 contains a mono [[wikipedia:Class-D amplifier|Class-D (audio) amplifier]]. However, there are several devices that do not make use of it. Perhaps because the manufacturer thought it's not powerful enough, or because they wanted to include stereo speakers. In that case they have likely included a ''custom speaker amplifier''. There are two types of this:

* '''Digital codec + speaker amplifier:''' This can be seen in the diagram above. It's connected via the Quaternary MI²S interface and contains it's own [[wikipedia:Digital-to-analog converter|digital-to-analog converter (DAC)]].
** Sometimes they are just enabled with a GPIO, sometimes (especially TFA* amplifiers from NXP/Goodix) they are controlled via [[wikipedia:I²S|I²S]] and may even include their own audio [[wikipedia:Digital signal processor|DSP]].
** A typical property of these is that it's difficult to control the audio volume in hardware. Instead, the digital audio data needs to be adjusted in software (typically handled by [[wikipedia:PulseAudio|PulseAudio]].)
** You need to add the amplifier to the device tree and then set up a DAI link for Quaternary MI²S.
*** Example with enable GPIO: <code>arm64: dts: qcom: apq8016-samsung-matissevewifi: add sound</code>
*** Example with I²C: <code>arm64: dts: msm8916-samsung-a2015: Add sound card</code> (Note: You typically need a driver specific to your amplifier in this case...)

* '''Analog speaker amplifier:''' This one sits behind the analog codec in PM8916 and just further amplifies the (analog) output signal. The speaker output is not suitable for this so most devices have the speaker amplifier connected additionally to one of the headphones output, i.e. <code>HPH_L</code> (left channel) or <code>HPH_R</code> (right channel). Typically they are activated using a GPIO. Sometimes there is an extra switch for the headphones as well, to make sure they don't activate when we just want to activate the speaker.
** You need to add the amplifier to the device tree (and eventually fixed regulators required for it). Then add it as <code>aux-devs</code> to the sound card and set up <code>audio-routing</code>.
*** Example with amplifier connected to HPH_R and extra headphones switch: <code>arm64: dts: qcom: msm8916-wingtech-wt88047: Add sound</code>
*** Example with special amplifier connected to HPH_R without headphones switch: <code>arm64: dts: qcom: msm8916-bq-paella: Add sound</code>

==== Testing ====
Please try to test as many of the features below as possible. If you don't have headphones or a headset you can obviously not test that.

* Speaker/Earpiece/Headphones audio playback
* Mic1/Mic2/Headset audio recording
** '''Note:''' Some devices only have one integrated microphone.
** Enable microphone via ALSA UCM / PulseAudio, then use (for example): <code>arecord -f dat -Dplughw:0,1 test.wav</code>, then <code>aplay test.wav</code> and check if you hear something.
* Audio jack detection
** Plug in/out headphones and check for <code>SW_HEADPHONE_INSERT</code> in <code>evtest</code>. It should be 1 when inserted, 0 otherwise. (Make sure it's not inverted!)
** Plug in/out headset and check for <code>SW_MICROPHONE_INSERT</code> additionally.
*** If the headset has buttons, try pressing them and see if they show up in <code>evtest</code>.

=== Modem ===
{{todo|For now, check related commits for other devices and ask on Matrix if you have any questions :)}}

=== Camera ===
There is the [https://www.kernel.org/doc/html/latest/admin-guide/media/qcom_camss.html qcom-camss] subsystem([https://memcpy.io/upstream-camera-support-for-qualcomm-platforms.html 1]), but per device camera drivers are needed too. For use with Megapixels, support for pipelines aren't implemented yet.

{{todo|Ping Mis012 on Matrix!}}

=== Battery/Charging ===
Although PM8916 provides a linear charger for batteries only few devices actually make use of it. This might be because it is comparatively inefficient compared to a switching mode battery charger. Therefore most devices actually have custom chips that manage the battery and charging. Usually it is separated in:
* '''Fuel gauge:''' Measures the remaining charge of the battery.
* '''Charger:''' Handles charging the battery.

Similar to the sensors, battery/charger are usually connected to an [[wikipedia:I²C|I²C bus]]. Look through the supported drivers in mainline (<code>drivers/power/supply</code>) to see if a driver exists already, or can be modified eventually to support another version of a similar chip. Compare with the downstream driver just to be sure.

{{note|'''Warning:''' Incorrect usage of battery/charger drivers might cause the battery to malfunction. In worst case, incorrect charging of batteries might even cause the battery to explode. Be careful.}}

==== SMB1360 ====
SMB1360 is a combined fuel gauge/charger chip from Qualcomm that was used in some MSM8916 devices. The msm8916-mainline/linux fork has an experimental driver for it that allows checking the battery status and basic charging.

* There are lots of configuration parameters that should be set correctly to ensure correct battery status reporting and in particular correct and safe charging. Most parameters can be taken over from the downstream device tree, but please check the DT schema (<code>Documentation/devicetree/bindings/power/supply/smb1360.yaml</code>) to be sure. Also check the existing devices for real examples.
* Battery detection is often device-specific and is implemented in lk2nd to keep the Linux kernel simple. lk2nd updates the <code>qcom,smb1360</code> entry in the device tree with the detected values and enables the smb1360 device node.
* The following properties are currently intentionally not supported by the mainline driver:
** <code>qcom,batt-profile-select</code>: This is implemented in lk2nd. The battery profile related properties should be added to the lk2nd device tree instead.
** <code>qcom,stat-pulsed-irq</code>: Please remove the property and use <code>IRQ_TYPE_LEVEL_LOW</code> instead.
** <code>qcom,config-hard-thresholds</code>: Use <code>qcom,otp-hard-jeita-config</code> instead.
** <code>qcom,soft-jeita-supported</code>: Use <code>qcom,soft-jeita-config</code> instead.

Manufacturers like to make random changes directly to the C code of the charging driver. Therefore it is possible that even the same device tree configuration can lead to different (and possibly incorrect) behavior when using the mainline driver. To verify that all registers are set as intended it is helpful to compare a register dump of the downstream and the mainline driver (after configuration changes have been applied):

* '''Mainline:''' Enable <code>CONFIG_SMB1360_DEBUG</code>: During startup the whole register content will be dumped twice to dmesg, once before the driver makes any changes and once after.
* '''Downstream:'''
** '''Modified downstream kernel:''' The smb1360-dump code can be integrated in the downstream kernel, which will produce a register dump identical to the one produced by the mainline driver. See e.g. {{Kernel package|bq-paella-downstream|archived}} for an example.
** '''DebugFS:''' This avoids having to recompile the downstream kernel (just need root, e.g. in TWRP), but is less complete. Also it needs post processing to allow for easier comparison with the mainline register dump.

'''NOTE:''' SMB1360 remembers configuration changes across reboots as long as the battery is still sufficiently charged. This means that some registers may appear to be set correctly when booting into mainline after downstream, while they would be set incorrectly if mainline was directly booted after a battery removal. To ensure clean results you should reset the SMB1360 immediately before taking each register dump:

* Temporarily remove/disconnect the battery for a few seconds ('''PREFERRED if possible'''). This will reset the SMB1360 completely (both charger and fuel gauge registers).
* Using <code>fastboot oem smb1360-reload</code> in lk2nd. This will only reset the charger related registers.

Take a clean register once using downstream and once using mainline. Using some post processing you can compare the register dumps with a simple diff tool (TBD: Maybe document the post processing a bit more). For most registers, the downstream register dump should be identical to the second register dump printed in dmesg when using the mainline driver. However, there are some expected differences:

* Differences in IRQ/status related registers can generally be ignored. They are most likely caused by taking the register dumps in slightly different situations (e.g. USB cable inserted vs not inserted, currently fast charging vs slow charging, ...).
* The <code>fg shadow</code> and <code>fg scratch pad</code> registers change depending on the battery status (remaining percentage, temperature etc). However, look closely at registers in full caps (e.g. FG_THERM_C1_COEFF_REG). Most of these are written by the driver.
* The <code>fg otp backup</code> register contents may differ because the mainline driver writes them in a different order. The values in <code>OTP_BACKUP_REG</code> should be identical, however.
* <code>CFG_BATT_CHG_ICL_REG</code> bits 0-3 contain the input current limit (more current may be taken from a wall charger compared to an USB port on a PC). Unfortunately, the mainline driver does not implement this functionality at the moment and keeps the input current limit as-is.
* <code>CHG_CMP_CFG</code> is used by the <code>qcom,soft-jeita-config</code> code, it may be different if downstream had both <code>qcom,config-hard-thresholds</code> and <code>qcom,soft-jeita-supported</code> (in which case the temperature monitoring was implemented in software rather than programming the hardware registers).
* <code>IRQX_CFG_REG</code> may have additional bits for more interrupt types enabled by the mainline driver.
* <code>SOC_MIN_REG</code> may be off-by-one because the mainline driver rounds differently.

=== [[Haptics|Vibrator]] ===

To enable the vibrator simply add

<syntaxhighlight lang="c">
&pm8916_vib {
	status = "okay";
};
</syntaxhighlight>

To find out which input device the vibrator is, run <code>cat /proc/bus/input/devices</code> and look for something like <code>pm8xxx_vib</code>.  
Run <code>fftest /dev/input/eventX</code> (<code>linuxconsoletools</code>) with the event number from the command before (i.e. "Handlers=event3" so /dev/input/event3).

Useful page when messing with haptics: https://www.kernel.org/doc/html/v4.15/input/ff.html

=== [[NFC]] ===
{{todo|For now, check related commits for other devices and ask on Matrix if you have any questions :)}}

=== Regulators (OBSOLETE) ===
{{note|'''NOTE''': <code>msm8916-pm8916.dtsi</code> now contains safe default voltages for most standard regulators. Most of this section is now '''obsolete'''!}}

As next steps we would like to enable internal/external storage ([[wikipedia:MultiMediaCard#eMMC|eMMC]]/[[wikipedia:SD card|SD card]]) and make USB gadget mode work. This will allow you to attach your device to a PC and get a debugging console.

However, a requirement of both of these hardware components is that you have some [[wikipedia:Voltage regulator|voltage regulators]] set up properly. Hardware components have different power supply requirements, so usually the [[wikipedia:Power management integrated circuit|PMIC]](s) used together with a [[wikipedia:System on chip|SoC]] provide several different regulators that can be configured to produce a voltage level within a specified range.

Let's take a quick look at <code>msm8916-pm8916.dtsi</code>. As a reminder: This include contains common definitions that apply to most devices that pair MSM8916 (the [[wikipedia:System on chip|SoC]]) with PM8916 (the [[wikipedia:Power management integrated circuit|PMIC]]). Most interesting for us are the references to <code>sdhc_1/2</code> (the SD Host Controller) and <code>usb_hs_phy</code> (a part of the USB controller):

<syntaxhighlight lang="c">
&sdhc_1 {
	vmmc-supply = <&pm8916_l8>;
	vqmmc-supply = <&pm8916_l5>;
};

&sdhc_2 {
	vmmc-supply = <&pm8916_l11>;
	vqmmc-supply = <&pm8916_l12>;
};

&usb_hs_phy {
	v1p8-supply = <&pm8916_l7>;
	v3p3-supply = <&pm8916_l13>;
};
</syntaxhighlight>

<code>pm8916_l5</code>, <code>pm8916_l11</code> etc are some of the regulators that are provided by PM8916. As you can see they are configured as supplies for both eMMC/SD card and USB.

All these regulators can be configured to a large voltage range, so it is '''very important that we set up voltage constraints carefully!''' The voltage constraints specify the allowed voltage range that can be set for a particular regulator. Once we have set them up properly, we can be reasonably sure that accidental mistakes cannot easily destroy one of the hardware components in a device.

{{note|'''WARNING:''' Hardware components usually only work correctly within a limited voltage range. Exceeding the specified voltage range for a component may cause it to malfunction, or even break physically. '''Please go through this section carefully and ask for feedback in the chat.'''}}

Let's get back to the device tree for your device. We extend the initial UART example from the previous section with new device tree nodes to set up the voltage constraints. A typical set of voltage constraints for PM8916 looks like this:

<syntaxhighlight lang="c">
// FIXME: Verify all regulators with downstream
&smd_rpm_regulators {
	vdd_l1_l2_l3-supply = <&pm8916_s3>;
	vdd_l4_l5_l6-supply = <&pm8916_s4>;
	vdd_l7-supply = <&pm8916_s4>;

	s3 {
		regulator-min-microvolt = <1200000>;
		regulator-max-microvolt = <1300000>;
	};

	s4 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <2100000>;
	};

	l1 {
		regulator-min-microvolt = <1225000>;
		regulator-max-microvolt = <1225000>;
	};

	l2 {
		regulator-min-microvolt = <1200000>;
		regulator-max-microvolt = <1200000>;
	};

	l4 {
		regulator-min-microvolt = <2050000>;
		regulator-max-microvolt = <2050000>;
	};

	l5 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <1800000>;
	};

	l6 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <1800000>;
	};

	l7 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <1800000>;
	};

	l8 {
		regulator-min-microvolt = <2850000>;
		regulator-max-microvolt = <2900000>;
	};

	l9 {
		regulator-min-microvolt = <3300000>;
		regulator-max-microvolt = <3300000>;
	};

	l10 {
		regulator-min-microvolt = <2700000>;
		regulator-max-microvolt = <2800000>;
	};

	l11 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <2950000>;
		regulator-allow-set-load;
		regulator-system-load = <200000>;
	};

	l12 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <2950000>;
	};

	l13 {
		regulator-min-microvolt = <3075000>;
		regulator-max-microvolt = <3075000>;
	};

	l14 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <3300000>;
	};

	l15 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <3300000>;
	};

	l16 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <3300000>;
	};

	l17 {
		regulator-min-microvolt = <2850000>;
		regulator-max-microvolt = <2850000>;
	};

	l18 {
		regulator-min-microvolt = <2700000>;
		regulator-max-microvolt = <2700000>;
	};
};
</syntaxhighlight>

You should paste this at the end of your device tree and then go through all the voltages and compare them what you see in your downstream reference kernel. Here are some tips and tricks:

* Downstream regulators look a bit different, but they are usually defined in <code>msm8916-regulators.dtsi</code>. Note that some device trees may override the regulator voltages in a device-specific <code>.dts(i)</code>.
* <code>s1</code> (<code>smpa1</code> and <code>l3</code> (<code>ldoa3</code>) are missing in the voltage constraints because they are managed separately as ''power domains''. Simply put, these two regulators are used by many components in the [[wikipedia:System on chip|SoC]]. Instead of picking an exact voltage, we vote for a particular ''performance state''. On downstream this is called ''voltage corner''. There is a special Resource Power Manager (RPM) co-processor that collects all the votes and then decides for a particular voltage to use.
** There is no need to set up voltage constraints for these because there is no way to set an dangerous voltage through the corner interface.
* <code>s2</code> (<code>spm-regulator</code>) is special (used to control the voltage for the CPU cores) and is not touched by mainline at the moment. Just ignore it.
* Also ignore the <code>-ao</code> and <code>-so</code> regulators (usually just on <code>l7</code>)
* <code>regulator-system-load</code> on <code>l11</code> in the snippet above is for stability of the SD card<ref>[https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=af61bef513ba179559e56908b8c465e587bc3890 arm64: dts: apq8016-sbc: Increase load on l11 for SDCARD]</ref>. This is declared in a different place on downstream, but most likely you can just keep it as-is.
* <code>vdd_l1_l2_l3-supply</code> etc does not exist on downstream, but you can also keep it as-is. It is extremely unlikely that this is different for a particular device. Most important is validating the voltages.

{{note|'''Note:''' Usually most of the voltage ranges should stay as-is. If you are unsure, better be safe and ask for feedback in the chat!}}

== Glossary ==
;PM8916
:The [[wikipedia:Power management integrated circuit|PMIC]] used together with MSM8916 (and MSM8939).
;BAM
:Bus Access Manager, some kind of DMA (Direct Memory Access) engine. Basically it can copy data from memory to devices (e.g. UART, USB, ...) more quickly.
;BAM DMUX
:BAM Data Mux, a network protocol built using BAM that is used as a network interface to the modem.
;BLSP
:"BAM-enabled low-speed peripheral". Controllers for UART, I²C and SPI.
;LPASS
:Low-power audio subsystem
;MDSS
:Mobile Display Subsystem - the hardware that manages the display.
;MDP
:Mobile Display Processor - Part of MDSS that manages display panels.
;DSI
:[[wikipedia:Display Serial Interface|MIPI Display Serial Interface (DSI)]] - used for communication with display panel.
;CAMSS
:Camera Subsystem
;SPMI
:System Power Management Interface - used for communication between MSM8916 and PM8916.

=== Remote Processors ===
;WCNSS
:Wireless Connectivity Subsystem (WCNSS). Provides WiFi and Bluetooth.
;Venus
:Provides hardware-accelerated video encoding/decoding.
;Modem
:Provides mobile connectivity and Audio DSP (ADSP). '''Note:''' On MSM8916, modem and ADSP are one remote processor, but the ADSP is usually separate on Qualcomm SoCs.
;Hexagon
:Microarchitecture for DSPs, used for ADSP/Modem/Sensor Hub/...

== Documentation ==
* Specifications
** [https://developer.qualcomm.com/download/sd410/snapdragon-410-processor-device-specification.pdf Snapdragon 410E (APQ8016E) Processor Device Specification]
** [https://developer.qualcomm.com/download/sd410/pm8916pm8916-1-power-management-ic-device-specification.pdf PM8916/PM8916-1 Power Management IC Device Specification]
** [https://developer.qualcomm.com/download/sd410/wcn3620-wireless-connectivity-ic-device-specification.pdf WCN3620 Wireless Connectivity IC Device Specification]
** [https://developer.qualcomm.com/download/sd410/wcn3680b-wcn3660b-device-spec.pdf WCN3680B/WCN3660B Device Specification]
* Register Descriptions
** [https://developer.qualcomm.com/download/sd410/hardware-register-description-qualcomm-snapdragon-410.pdf Qualcomm Snapdragon 410E Processor (APQ 8016E) Hardware Register Description] 
** [https://developer.qualcomm.com/download/sd410/pm8916-hardware-register-description.pdf PM8916 Hardware Register Description]
* [https://developer.qualcomm.com/download/sd410/snapdragon-410e-technical-reference-manual.pdf Snapdragon 410E Technical Reference Manual] (provides explanations for many hardware blocks)
* More documentation: [https://developer.qualcomm.com/hardware/apq-8016e/tools APQ8016E Tools & Resources]

== References ==
<references />

[[Category:Guide]]
[[Category:Technical Reference]]
