#!/bin/bash
# Configuration
ESP8266_IP="192.168.0.52:2323"
IFACE_OUT="enp0s3"
LINUX_MACHINE_IP="192.168.0.122"
STM32_IP="192.168.0.250"

echo "Enabling IP Forwarding..."
sudo sysctl -w net.ipv4.ip_forward=1

echo "Cleaning up old sessions..."
ps aux | grep socat
sudo killall socat
echo "Starting Socat tunnel to $ESP8266_IP..."
sudo socat -d -d pty,link=/dev/ttyV0,raw,echo=0 tcp:$ESP8266_IP >/tmp/socat.log 2>&1 &

sleep 2

sudo pkill pppd
echo "Starting PPPD..."
sudo pppd persist holdoff 0 maxfail 0 \
    /dev/ttyV0 115200 $LINUX_MACHINE_IP:$STM32_IP \
    noauth debug updetach local nocrtscts novj noccp noipv6 proxyarp

echo "Setting up NAT via $IFACE_OUT..."
sudo iptables -t nat -A POSTROUTING -s $STM32_IP -o $IFACE_OUT -j MASQUERADE