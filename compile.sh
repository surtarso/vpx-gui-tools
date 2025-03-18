#!/bin/bash

# List of required packages
packages=(
    build-essential
    libsdl2-dev
    libgl1-mesa-dev
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
    sudo apt update && sudo apt install -y "${missing[@]}"
fi

# check submodule updates
git submodule update --init --recursive

# Compile
echo "Compiling..."
g++ src/vpx_config.cpp imgui/*.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_opengl3.cpp -std=c++17 -I/usr/include/SDL2 -D_REENTRANT -Iimgui -Iimgui/backends -lSDL2 -lGL -o vpx_config
