original_directory=$(pwd)
script_directory=$(dirname "$0")

sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install cmake
sudo apt-get install gdb
sudo apt-get install clang clang-tidy
sudo apt-get install libgtest-dev
sudo apt-get install libsndfile1-dev pkg-config
sudo apt-get install sndfile-programs
sudo apt-get install sox
sudo apt-get install libasound2-dev
sudo apt-get install libgpiod-dev
sudo apt-get autoremove

mkdir -p "$script_directory/lib"
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
  cd signalsmith-stretch
  git checkout f2294a5
  cd ..
fi

# Clone and build lgpio if not already done
if [ ! -d "lg" ]; then
  git submodule add -f https://github.com/joan2937/lg
  cd lg
  make
  sudo make install
  cd ..
fi

cd "$original_directory"

# Enable SPI, I2C
sudo raspi-config nonint do_spi 0
sudo raspi-config nonint do_i2c 0

# Enable I2S if not already enabled
if ! grep -q "^dtparam=i2s=on" /boot/firmware/config.txt; then
  echo "dtparam=i2s=on" | sudo tee -a /boot/firmware/config.txt > /dev/null
fi

# Check if Rpi is active to ensure CI/CD compatibility on Git
if grep -q "Raspberry Pi" /proc/cpuinfo; then
  sudo raspi-config nonint do_spi 0
  sudo raspi-config nonint do_i2c 0
  if ! grep -q "^dtparam=i2s=on" /boot/firmware/config.txt; then
    echo "dtparam=i2s=on" | sudo tee -a /boot/firmware/config.txt > /dev/null
  fi
fi
