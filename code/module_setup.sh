# code/module_setup.sh
# ------------------------------
# Prepare Libraries via Submodules
# ------------------------------

repo_root=$(git rev-parse --show-toplevel 2>/dev/null || echo "")
script_directory= "$repo_root/code"

cd "$script_directory"
mkdir -p lib && cd ..

# Function to check and add a submodule only if needed
ensure_submodule() {
  local path="$1"
  local url="$2"

  if ! git config --file "$repo_root/.gitmodules" --get-regexp "^submodule\..*\.path$" | grep -q "$path"; then
    echo "Adding submodule: $path"
    git submodule add "$url" "$path"
  fi
}

ensure_submodule "$script_directory/lib/signalsmith-stretch" "https://github.com/Signalsmith-Audio/signalsmith-stretch.git"
ensure_submodule "$script_directory/lib/uSockets" "https://github.com/uNetworking/uSockets.git"
ensure_submodule "$script_directory/lib/vcpkg" "https://github.com/microsoft/vcpkg.git"


git submodule update --init --recursive
# ------------------------------
# Build uSockets
# ------------------------------
cd "$script_directory"/lib/uSockets
make
cd ..

# ------------------------------
# Bootstrap vcpkg and install libs
# ------------------------------
cd "$script_directory"/lib/vcpkg
./bootstrap-vcpkg.sh
./vcpkg install nlohmann-json
./vcpkg install uwebsockets
cd "$script_directory"