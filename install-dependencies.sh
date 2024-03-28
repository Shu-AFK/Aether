#!/bin/bash

# Function to install packages on Debian-based systems
install_debian() {
  sudo apt update
  sudo apt install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo curl qemu nasm make
}

# Function to install packages on Gentoo
install_gentoo() {
  sudo emerge --ask sys-devel/gcc sys-devel/make sys-devel/bison sys-devel/flex dev-libs/gmp dev-libs/mpc dev-libs/mpfr sys-apps/texinfo app-emulation/qemu dev-lang/nasm
}

# Function to install packages on Fedora
install_fedora() {
  sudo dnf install -y gcc gcc-c++ make bison flex gmp-devel libmpc-devel mpfr-devel texinfo curl qemu nasm
}

# Function to install packages on OpenBSD
install_openbsd() {
  doas pkg_add -v gmp libmpc mpfr curl qemu nasm gmake
}

# Function to install packages on Arch Linux
install_arch() {
  sudo pacman -Syu base-devel gmp libmpc mpfr curl qemu nasm make
}

# Detect the operating system and call the respective function
OS=$(uname -s)
case $OS in
  Linux)
    if [ -f /etc/os-release ]; then
      . /etc/os-release
      case $ID in
        debian|ubuntu|*mint*|*wsl*)
          install_debian
          ;;
        gentoo)
          install_gentoo
          ;;
        fedora)
          install_fedora
          ;;
        arch|manjaro)
          install_arch
          ;;
        *)
          echo "Unsupported Linux distribution"
          ;;
      esac
    else
      echo "Unable to detect the Linux distribution."
    fi
    ;;
  OpenBSD)
    install_openbsd
    ;;
  *)
    echo "Unsupported operating system."
    ;;
esac

# Install binutils and gcc
cd ~
curl -O https://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.xz
curl -O https://ftp.lip6.fr/pub/gcc/releases/gcc-10.2.0/gcc-10.2.0.tar.gz

tar -xzvf gcc-10.2.0.tar.gz
tar -xJvf binutils-2.35.tar.xz

mkdir src

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# Binutils
cd $HOME/src

mkdir build-binutils
cd build-binutils
../binutils-2.35/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install


# GCC
cd $HOME/src

# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
../gcc-10.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

echo "Dependency installation script has finished."

