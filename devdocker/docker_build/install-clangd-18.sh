#!/bin/bash

set -eux

LLVM_VERSION=18

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root!"
    exit 1
fi

if ! which add-apt-repository &>/dev/null; then
    echo "add-apt-repository is required but not installed."
    exit 1
fi

if ! which wget &>/dev/null; then
    echo "wget is required but not installed."
    exit 1
fi

if ! which apt-get &>/dev/null; then
    echo "apt-get is required but not installed."
    exit 1
fi

BASE_URL="http://apt.llvm.org"

CODENAME=$(lsb_release -cs)

REPO_NAME="deb ${BASE_URL}/${CODENAME}/ llvm-toolchain-${CODENAME}-18 main"

if ! wget -q --method=HEAD ${BASE_URL}/${CODENAME} &>/dev/null; then
    echo "Distribution '${CODENAME}' is not supported by this script."
    exit 2
fi

if [[ ! -f /etc/apt/trusted.gpg.d/apt.llvm.org.asc ]]; then
    wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
fi

add-apt-repository -y "${REPO_NAME}"
apt-get update
apt-get install -y clangd-$LLVM_VERSION

update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-$LLVM_VERSION 100
