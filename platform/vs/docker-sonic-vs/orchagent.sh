#!/usr/bin/env bash

export platform=vs

MAC_ADDRESS=`ip link show eth0 | grep ether | awk '{print $2}'`

# Create a folder for SwSS record files
mkdir -p /var/log/swss
ORCHAGENT_ARGS="-d /var/log/swss "

# Set orchagent pop batch size to 8192
ORCHAGENT_ARGS+="-b 8192 "

# Set mac address
ORCHAGENT_ARGS+="-m $MAC_ADDRESS"

exec /usr/bin/orchagent ${ORCHAGENT_ARGS}
