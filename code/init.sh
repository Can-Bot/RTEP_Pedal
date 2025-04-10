#!/bin/bash

set -e  # Exit on any error

original_directory=$(pwd)
script_directory=$(cd "$(dirname "$0")" && pwd)

# ------------------------------
# System Update & Essentials
# ------------------------------
sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get install -y \
  build-essential \
  cmake \
  gdb \
  clang \
  clang-tidy \
  git \
  curl

# ------------------------------
# Networking & Server Tools
# ------------------------------
sudo apt-get install -y \
  avahi-daemon \
  nginx \
  fcgiwrap

# ------------------------------
# Audio Libraries
# ------------------------------
sudo apt-get install -y \
  libsndfile1-dev \
  libasound2-dev \
  pkg-config \
  libssl-dev \
  sndfile-programs \
  sox

# ------------------------------
# Google Test (headers only)
# ------------------------------
sudo apt-get install -y libgtest-dev

# ------------------------------
# Hostname Setup
# ------------------------------
# sudo hostnamectl set-hostname shred

# ------------------------------
# Prepare Libraries via Submodules
# ------------------------------
sudo ./module_setup.sh

# ------------------------------
# Node.js and Angular CLI
# ------------------------------
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs
sudo npm install -g @angular/cli

# ------------------------------
# Set Up Angular Project (if missing)
# ------------------------------
cd "$script_directory"
if [ ! -d "src/shred-web" ]; then
  ng new shred-web --routing=true --style=css
  cd src/shred-web
  ng generate service services/websocket
  cd "$script_directory"
fi

# ------------------------------
# Final Cleanup
# ------------------------------
sudo apt-get autoremove -y

cd "$original_directory"
echo "Setup complete. Build with ./build.sh"
