#!/usr/bin/env bash

rm -f /var/run/rsyslogd.pid

supervisorctl start rsyslogd

# Router advertiser should only run on ToR (T0) devices
DEVICE_ROLE=$(sonic-cfggen -d -v "DEVICE_METADATA.localhost.type")
if [ "$DEVICE_ROLE" != "ToRRouter" ]; then
    echo "Device role is not ToRRouter. Not starting router advertiser process."
    exit 0
fi

# Generate /etc/radvd.conf config file
sonic-cfggen -d -t /usr/share/sonic/templates/radvd.conf.j2 > /etc/radvd.conf

# Enusre at least one interface is specified in radvd.conf
NUM_IFACES=$(grep -c "^interface " /etc/radvd.conf)
if [ $NUM_IFACES -eq 0 ]; then
    echo "No interfaces specified in radvd.conf. Not starting router advertiser process."
    exit 0
fi

# Start the router advertiser
supervisorctl start radvd
