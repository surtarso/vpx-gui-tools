#!/bin/bash

# ===========================================================================================
# List of required packages
packages=(
    build-essential
    libsdl2-dev
    libgl1-mesa-dev
    jq
)

# Check and install missing packages
missing=()
for pkg in "${packages[@]}"; do
    if ! dpkg -s "$pkg" &>/dev/null; then
        missing+=("$pkg")
    fi
done

if [ ${#missing[@]} -eq 0 ]; then
    echo "Dependencies satisfied."
else
    echo "Installing missing packages: ${missing[*]}"
    sudo apt-get update && sudo apt-get install -y "${missing[@]}"
fi

# ===========================================================================================
# Check and update submodules (imgui)
git submodule update --init --recursive

# ===========================================================================================
# Fetch the latest vpxtool release info from GitHub API
echo "Fetching latest vpxtool release..."

LATEST_RELEASE=$(curl -s https://api.github.com/repos/francisdb/vpxtool/releases/latest)

# Extract the download URL for the Linux binary
DOWNLOAD_URL=$(echo $LATEST_RELEASE | jq -r '.assets[] | select(.name | test("Linux-x86_64")) | .browser_download_url')

# If the download URL is found, download the binary
if [ -z "$DOWNLOAD_URL" ]; then
    echo "Error: Could not find the latest vpxtool Linux binary URL!"
    exit 1
fi

echo "Downloading vpxtool from: $DOWNLOAD_URL"
wget $DOWNLOAD_URL -O vpxtool.tar.gz

# Extract the binary and move it to /src
echo "Extracting vpxtool..."
tar -xzvf vpxtool.tar.gz
mv vpxtool src/vpxtool
chmod +x src/vpxtool
rm vpxtool.tar.gz

# ===========================================================================================
# Compile
echo "Compiling configuration..."
g++ src/vpx_config.cpp imgui/*.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_opengl3.cpp -std=c++17 -I/usr/include/SDL2 -D_REENTRANT -Iimgui -Iimgui/backends -lSDL2 -lGL -o vpx_config

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
else
    echo "Compilation failed!"
    exit 1
fi
