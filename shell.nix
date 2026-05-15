{ pkgs ? import <nixpkgs> {} }:

let
  crossPkgs = pkgs.pkgsCross.arm-embedded;
in
pkgs.mkShell {
  buildInputs = [
    pkgs.gnumake
    crossPkgs.buildPackages.gcc
    pkgs.git
    pkgs.python3
    pkgs.dtc

  ];

  shellHook = ''
    echo "Ready to build lk2nd. Use: make TOOLCHAIN_PREFIX=arm-none-eabi- lk2nd-msmXXXX"
  '';
}
