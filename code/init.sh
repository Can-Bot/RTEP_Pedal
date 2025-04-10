#!/bin/bash

set -e  # Stop on error

original_directory=$(pwd)
script_directory=$(cd "$(dirname "$0")" && pwd)

# Update system and install essentials
sudo apt update
sudo apt upgrade -y
sudo apt install -y build-essential cmake clang clang-tidy git curl

# Networking & server tools
sudo apt install -y avahi-daemon nginx fcgiwrap

# Audio libs
sudo apt install -y libsndfile1-dev pkg-config libasound2-dev libssl-dev

# Set hostname
sudo hostnamectl set-hostname shred

# Go to script dir and setup libs
cd "$script_directory"
mkdir -p lib && cd lib

# Clone DSP lib
git clone https://github.com/Signalsmith-Audio/signalsmith-stretch.git || true

# Clone and build uSockets manually
git clone https://github.com/uNetworking/uSockets.git || true
cd uSockets
make
cd ..

# Clone and bootstrap vcpkg
git clone https://github.com/microsoft/vcpkg.git || true
cd vcpkg
./bootstrap-vcpkg.sh

# Install C++ JSON and uWebSockets via vcpkg
./vcpkg install nlohmann-json
./vcpkg install uwebsockets

cd "$original_directory"

# Install Node.js and Angular CLI
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt install -y nodejs
sudo npm install -g @angular/cli

# Create Angular project if not already present
if [ ! -d "shred-web" ]; then
    ng new shred-web --routing=true --style=css
    cd shred-web
    ng generate service services/websocket
    cd ..
fi

sudo apt autoremove

# Done. Build with ./build.sh
