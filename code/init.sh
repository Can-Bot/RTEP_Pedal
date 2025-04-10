original_directory=$(pwd)
script_directory=$(dirname "$0")

sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install cmake gdb
sudo apt-get install clang clang-tidy

sudo apt-get install avahi-daemon
sudo apt-get install nginx fcgiwrap

sudo apt-get install libsndfile1-dev pkg-config
sudo apt-get install libasound2-dev

sudo apt-get install libgtest-dev
sudo apt-get install libsndfile1-dev pkg-config
sudo apt-get install sndfile-programs
sudo apt-get install sox
sudo apt-get install libasound2-dev
sudo apt-get autoremove

sudo hostnamectl set-hostname shred

cd "$script_directory/lib"

# If directory exists and is not a git repo, remove it
if [ -d "signalsmith-stretch" ]; then
  if [ ! -d "signalsmith-stretch/.git" ]; then
    rm -rf signalsmith-stretch
  fi
fi

# Clone if not already present
if [ ! -d "signalsmith-stretch" ]; then
  git submodule add -f https://github.com/Signalsmith-Audio/signalsmith-stretch.git ./signalsmith-stretch
fi

cd "$original_directory"

