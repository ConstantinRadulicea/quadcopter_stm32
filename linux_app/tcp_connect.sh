#!/bin/bash
# Configuration
# ESP8266_IP="192.168.0.52"
# ESP8266_PORT="2323"         # port for the esp8266
# STM32_IP="192.168.0.250"
# IFACE_OUT="enp0s3"

ESP8266_IP="10.72.62.157"   # ipv4 for the esp8266 
ESP8266_PORT="2323"         # port for the esp8266
STM32_IP="10.72.62.250"     # ipv4 that you want to assign to the stm32. Must be on the same network as the esp8266
IFACE_OUT="enp0s3"


LINUX_MACHINE_IP=$(ip -4 addr show "$IFACE_OUT" | grep -oP '(?<=inet\s)\d+(\.\d+){3}')
# LINUX_MACHINE_IP=$(ip -4 -o addr show dev "$IFACE_OUT" | awk '{split($4,a,"/"); print a[1]}')
echo $LINUX_MACHINE_IP

echo "Enabling IP Forwarding..."
sudo sysctl -w net.ipv4.ip_forward=1

echo "Cleaning up old sessions..."
ps aux | grep socat
sudo killall socat
echo "Starting Socat tunnel to $ESP8266_IP:$ESP8266_PORT..."
sudo socat -d -d pty,link=/dev/ttyV0,raw,echo=0 tcp:$ESP8266_IP:$ESP8266_PORT >/tmp/socat.log 2>&1 &

sleep 2

sudo pkill pppd
echo "Starting PPPD..."
sudo pppd persist holdoff 0 maxfail 0 \
    /dev/ttyV0 115200 $LINUX_MACHINE_IP:$STM32_IP \
    noauth debug updetach local nocrtscts novj noccp noipv6 proxyarp

echo "Setting up NAT via $IFACE_OUT..."
sudo iptables -t nat -A POSTROUTING -s $STM32_IP -o $IFACE_OUT -j MASQUERADE