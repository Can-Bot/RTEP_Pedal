#!/bin/bash

set -e
repo_root=$(git rev-parse --show-toplevel 2>/dev/null || echo "")

cd "$repo_root"
PROJECT_ROOT="$repo_root/code"
ANGULAR_PROJECT="$PROJECT_ROOT/shred-web"
SERVER_BINARY="$PROJECT_ROOT/build/src/server/shred_server"
PEDAL_BINARY="$PROJECT_ROOT/build/src/shred_pedal"
NGINX_WEBROOT="/var/www/shred.local"

# Run the C++ pedal binary
if [ -f "$PEDAL_BINARY" ]; then
  echo "Running pedal processor: $PEDAL_BINARY"
  "$PEDAL_BINARY" &
else
  echo "Pedal binary not found: $PEDAL_BINARY"
fi
