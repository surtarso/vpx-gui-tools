#!/bin/bash

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Status indicators
OK="${GREEN}[ OK ]${NC}"
FAIL="${RED}[ FAIL ]${NC}"

# Exit on any error
set -e

# Trap errors and cleanup
trap 'echo -e "${RED}Script terminated unexpectedly${NC}"; exit 1' ERR

# ===========================================================================================
echo -e "${YELLOW}Checking dependencies...${NC}"

# List of required packages
packages=(
    build-essential
    libsdl2-dev
    libgl1-mesa-dev
    jq
    curl
)

# Check and install missing packages
missing=()
for pkg in "${packages[@]}"; do
    if ! dpkg -s "$pkg" &>/dev/null; then
        missing+=("$pkg")
    fi
done

if [ ${#missing[@]} -eq 0 ]; then
    echo -e "Dependencies satisfied. $OK"
else
    echo -e "Installing missing packages: ${missing[*]}"
    if ! sudo apt-get update; then
        echo -e "Failed to update package lists $FAIL"
        exit 1
    fi
    if ! sudo apt-get install -y "${missing[@]}"; then
        echo -e "Failed to install packages $FAIL"
        exit 1
    fi
    echo -e "Package installation complete $OK"
fi

# ===========================================================================================
echo -e "${YELLOW}Updating submodules...${NC}"
if ! git submodule update --init --recursive; then
    echo -e "Failed to update submodules $FAIL"
    exit 1
fi
echo -e "Submodules updated $OK"

# ===========================================================================================
echo -e "${YELLOW}Fetching latest vpxtool release...${NC}"

LATEST_RELEASE=$(curl -s https://api.github.com/repos/francisdb/vpxtool/releases/latest || {
    echo -e "Failed to fetch release info $FAIL"
    exit 1
})

# Extract the download URL for the Linux binary
DOWNLOAD_URL=$(echo "$LATEST_RELEASE" | jq -r '.assets[] | select(.name | test("Linux-x86_64")) | .browser_download_url')

if [ -z "$DOWNLOAD_URL" ]; then
    echo -e "Error: Could not find the latest vpxtool Linux binary URL! $FAIL"
    exit 1
fi

echo -e "Downloading vpxtool from: $DOWNLOAD_URL"
if ! wget "$DOWNLOAD_URL" -O vpxtool.tar.gz &>/dev/null; then
    echo -e "Download failed $FAIL"
    exit 1
fi

echo -e "Extracting vpxtool..."
if ! tar -xzvf vpxtool.tar.gz &>/dev/null; then
    echo -e "Extraction failed $FAIL"
    exit 1
fi

# Move and set permissions with error checking
if [ ! -f vpxtool ]; then
    echo -e "vpxtool binary not found after extraction $FAIL"
    exit 1
fi

mkdir -p src || {
    echo -e "Failed to create src directory $FAIL"
    exit 1
}
mv vpxtool src/vpxtool || {
    echo -e "Failed to move vpxtool $FAIL"
    exit 1
}
chmod +x src/vpxtool || {
    echo -e "Failed to set execute permissions $FAIL"
    exit 1
}
rm -f vpxtool.tar.gz
echo -e "vpxtool setup complete $OK"

# ===========================================================================================
echo -e "${YELLOW}Compiling configuration...${NC}"

# Check if g++ is installed
if ! command -v g++ &>/dev/null; then
    echo -e "g++ is required but not installed $FAIL"
    exit 1
fi

# Compile with error checking
if ! g++ src/vpx_config.cpp imgui/*.cpp imgui/backends/imgui_impl_sdl2.cpp \
    imgui/backends/imgui_impl_opengl3.cpp -std=c++17 -I/usr/include/SDL2 \
    -D_REENTRANT -Iimgui -Iimgui/backends -lSDL2 -lGL -o vpx_config; then
    echo -e "Compilation failed $FAIL"
    exit 1
fi

echo -e "Compilation successful! $OK"
