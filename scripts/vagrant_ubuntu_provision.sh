#!/bin/sh

# Script to provision an ubuntu box for compiling and releasing breakhack.

sudo apt-get update
sudo apt-get install -y \
    build-essential \
    git \
    cmake \
    mingw-w64 \
    libflac-dev \
    libfluidsynth-dev \
    libgme-dev \
    libmpg123-dev \
    libopusfile-dev \
    libvorbis-dev \
    libxmp-dev \
    libwavpack-dev \
    ninja-build \
    pkg-config \
    libopusfile-dev \
    libxmp-dev \
    fluidsynth

# Install steamcmd
sudo add-apt-repository multiverse; sudo dpkg --add-architecture i386; sudo apt update
sudo apt install steamcmd

[[ -d breakhack ]] || git clone --recursive https://github.com/oliveshark/breakhack


