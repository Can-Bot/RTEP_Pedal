#!/bin/bash

set -e

PROJECT_ROOT=$(cd "$(dirname "$0")" && pwd)
ANGULAR_PROJECT="$PROJECT_ROOT/shred-web"
SERVER_BINARY="$PROJECT_ROOT/build/server/shred_server"
PEDAL_BINARY="$PROJECT_ROOT/build/src/shred_pedal"
NGINX_WEBROOT="/var/www/shred.local"

# Run the C++ pedal binary
if [ -f "$PEDAL_BINARY" ]; then
  echo "Running pedal processor: $PEDAL_BINARY"
  "$PEDAL_BINARY" &
else
  echo "Pedal binary not found: $PEDAL_BINARY"
fi
