#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Check if the script is run as root.
# if [ "$EUID" -ne 0 ]; then
#   echo "Please run this script as root or with sudo."
#   exit 1
# fi

# --- Script now runs with root privileges ---

# Update the package list
echo "Updating package lists..."
apt update

# Install bridge-utils and isc-dhcp-client
echo "Installing ppp, bridge-utils and isc-dhcp-client..."
apt install -y ppp bridge-utils isc-dhcp-client

echo "Installation complete successfully!"