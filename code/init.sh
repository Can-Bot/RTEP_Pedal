original_directory=$(pwd)
script_directory=$(dirname "$0")

sudo apt update
sudo apt upgrade
sudo apt install build-essential
sudo apt install cmake
sudo apt install clang clang-tidy

sudo apt install avahi-daemon
sudo apt install nginx fcgiwrap

sudo apt install libsndfile1-dev pkg-config
sudo apt install libasound2-dev

sudo hostnamectl set-hostname shred

cd "$script_directory"
cd "lib"

git clone https://github.com/Signalsmith-Audio/signalsmith-stretch.git

cd "$original_directory"

