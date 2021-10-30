#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""
This script packages multiple fdt blobs into a container for multi-dtb images.

The same FDT is used for the container storage as it's assumed that the
bootloader implementation already has libfdt available. The format is loosly
inspired by u-boot FIT uImage format but it's only intended for DTB packaging.
The example of the generated format is as follows:

/dts-v1/;

/ {
    grove,version = 1;
    grove,label = "Multi-DTB container";

    apq8016-sbc {
        compatible = "qcom,apq8016-sbc", "qcom,apq8016";
        compression = "none";
        dtb = /incbin/("arch/arm64/dts/qcom/apq8016-sbc.dtb");
    };

    msm8916-samsung-gt58 {
        compatible = "samsung,gt58", "qcom,msm8916";
        compression = "gzip";
        dtb = /incbin/("arch/arm64/dts/qcom/msm8916-samsung-gt58.dtb.gz");
    };
};

"""
import argparse
import pathlib
import gzip

import libfdt

class FdtBlob(libfdt.FdtRo):

    """Class for a device-tree for inclusion to the container."""

    def __init__(self, data, name=None):
        libfdt.FdtRo.__init__(self, data)

        if name is None:
            name = self.get_compatible_list()[0].replace(",", "-")
        self.name = name

    def get_compatible(self):
        """Get top level compatible for the device-tree.
        :returns: compatible: property

        """
        root = self.path_offset('/')
        return self.getprop(root, "compatible")

    def get_compatible_list(self):
        """Get a list of values in a top level compatible.
        :returns: compatibles, list

        """
        ret = []
        for val in self.get_compatible()[:-1].split(b"\0"):
            ret.append(val.decode('utf-8'))

        return ret

    def is_compatible_with(self, compats):
        """Check if top-level compatible contains a value from the list.
        Returns True if the list is None or empty.

        :compats: compatible, list
        :returns: bool

        """
        if compats is None or len(compats) == 0:
            return True

        # Do the lists overlap?
        return not set(compats).isdisjoint(self.get_compatible_list())

    def get_model(self):
        """Get model property from the top level node.
        :returns: Model, string

        """
        root = self.path_offset('/')
        return self.getprop(root, "model").as_str()


class FdtGrove(libfdt.FdtSw):

    """Class for a generated multi-dtb container"""

    def __init__(self, dtbs, label=None, compress=True):
        """Create and fill the container.
        :dtbs: list of tuples (name, blob)
        :label: A label to add to the container
        """
        # About 45K ~ 55K for a normal msm8916 dtb
        libfdt.FdtSw.__init__(self, size_hint=len(dtbs) * 64 * 1024)
        if label is None:
            label = "Multi-DTB container"
        self.label = label

        self.finish_reservemap()
        with self.add_node(""):
            self.property_u32("grove,version", 1)
            self.property_string("grove,label", self.label)

            for blob_tuple in dtbs:
                name, blob = blob_tuple
                with self.add_node(name):
                    compat = blob.get_compatible()
                    self.property("compatible", bytes(compat))
                    dtb_data = bytes(blob.as_bytearray())
                    if compress:
                        self.property_string("compression", "gzip")
                        dtb_data = gzip.compress(dtb_data, mtime=0)
                    else:
                        self.property_string("compression", "none")
                    self.property("dtb", dtb_data)


def parse_args():
    """Create ArgumentParser object for the programm and parse the arguments.
    :returns: args

    """
    parser = argparse.ArgumentParser(description="Generate the multi-dtb container.")
    parser.add_argument("dtb", nargs="+", type=argparse.FileType("br"),
                        help="Device-Tree blobs to add to the container")
    parser.add_argument("-o", "--output", type=str,
                        required=True, help="Output file to store the container")
    parser.add_argument("-l", "--label", type=str,
                        help="specify the container label")
    parser.add_argument("-c", "--compatible", action="append", type=str,
                        help="Only append device-trees compatible with this value")
    parser.add_argument("-C", "--compress", action="store_true",
                        help="Compress the DTBs in the container")
    parser.add_argument("-v", "--verbose", action="count", default=0,
                        help="increase output verbosity")
    return parser.parse_args()

def debug_print(args, text, verbosity=1):
    """Print the message if verbosity exceeds the given level

    :args: args namesace from argparse
    :text: message
    :verbosity: level of verbosity required

    """
    if args.verbose >= verbosity:
        print(text)

def main():
    """Use application arguments to generate the container."""
    args = parse_args()
    dtbs = []
    for dtb in args.dtb:
        blob = FdtBlob(dtb.read())
        if not blob.is_compatible_with(args.compatible):
            debug_print(args, f"Ignoring: {blob.get_model()}", 2)
            continue
        debug_print(args, f"Appending: {blob.get_model()}")
        dtbs.append((pathlib.Path(dtb.name).stem, blob))

    grove = FdtGrove(dtbs, args.label, args.compress).as_fdt()
    grove.pack()

    with open(args.output, "wb") as ofile:
        ofile.write(grove.as_bytearray())


if __name__ == "__main__":
    main()
