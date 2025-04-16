#!/bin/bash

set -e

PROJECT_ROOT=$(cd "$(dirname "$0")" && pwd)
ANGULAR_PROJECT="$PROJECT_ROOT/src/shred-web"
SERVER_BINARY="$PROJECT_ROOT/build/src/server/shred_server"
PEDAL_BINARY="$PROJECT_ROOT/build/src/shred_pedal"
NGINX_WEBROOT="/var/www/shred.local"
NGINX_CONFIG="/etc/nginx/sites-available/shred.local"
WEBSOCKET_SERVICE="shred-server.service"

# Build Angular frontend
cd "$ANGULAR_PROJECT"
echo "Building Angular project..."
ng build --configuration production

# Deploy Angular files to web server root
BUILD_OUT=$(find dist/shred-web -type d -name browser | head -n 1)
if [ -z "$BUILD_OUT" ]; then
  echo "Angular build output not found."
  exit 1
fi

echo "Deploying Angular build from $BUILD_OUT to $NGINX_WEBROOT"
sudo mkdir -p "$NGINX_WEBROOT"
sudo cp -r "$BUILD_OUT"/* "$NGINX_WEBROOT/"


# Configure nginx if needed
if [ ! -f "$NGINX_CONFIG" ]; then
  echo "Creating nginx config for shred.local"
  sudo bash -c "cat > $NGINX_CONFIG" <<EOF
server {
    listen 80;
    server_name shred.local;

    root $NGINX_WEBROOT;
    index index.html;

    location / {
        try_files \$uri \$uri/ /index.html;
    }
}
EOF
  sudo ln -sf "$NGINX_CONFIG" /etc/nginx/sites-enabled/shred.local
  sudo rm -f /etc/nginx/sites-enabled/default
fi

# Reload nginx to apply changes
echo "Reloading nginx"
sudo nginx -t && sudo systemctl reload nginx

# Set up and restart WebSocket server as a systemd service
if [ -f "$SERVER_BINARY" ]; then
  echo "Restarting WebSocket server via systemd"
  sudo bash -c "cat > /etc/systemd/system/$WEBSOCKET_SERVICE" <<EOF
[Unit]
Description=Shred WebSocket Server
After=network.target

[Service]
ExecStart=$SERVER_BINARY
WorkingDirectory=$PROJECT_ROOT
Restart=always
RestartSec=2
StandardOutput=journal
StandardError=journal
User=$USER

[Install]
WantedBy=multi-user.target
EOF

  sudo systemctl daemon-reexec
  sudo systemctl daemon-reload
  sudo systemctl enable $WEBSOCKET_SERVICE
  sudo systemctl restart $WEBSOCKET_SERVICE
else
  echo "Server binary not found: $SERVER_BINARY"
fi
