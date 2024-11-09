#!/bin/bash

# Directory to store tarballs
pkg_source_dir="./docker_build/pkg_source"
echo "Downloading package sources to folder $pkg_source_dir ..."
mkdir -p "${pkg_source_dir}"

declare -A packages
packages["https://github.com/opencv/opencv/archive/refs/tags/4.10.0.tar.gz"]="opencv-4.10.0.tar.gz"

# Function to download a tarball if it doesn't already exist
download_tarball() {
    local url=$1
    local custom_name=$2

    # Check if the file already exists
    if [ ! -f "${pkg_source_dir}/${custom_name}" ]; then
        echo "Downloading ${custom_name}..."
        curl -L $url -o "${pkg_source_dir}/${custom_name}"
    else
        echo "File ${custom_name} already exists, skipping download."
    fi
}

# Download each tarball if not already downloaded
for url in "${!packages[@]}"; do
    download_tarball "$url" "${packages[$url]}"
done
