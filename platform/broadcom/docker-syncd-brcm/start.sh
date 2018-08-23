#!/usr/bin/env bash

PLATFORM_DIR=/usr/share/sonic/platform
HWSKU_DIR=/usr/share/sonic/hwsku

rm -f /var/run/rsyslogd.pid

supervisorctl start rsyslogd

mkdir -p /etc/sai.d/

# Create/Copy the sai.profile to /etc/sai.d/sai.profile
if [ -f $HWSKU_DIR/sai.profile.j2 ]; then
    sonic-cfggen -d -t $HWSKU_DIR/sai.profile.j2 > /etc/sai.d/sai.profile
else
    if [ -f $HWSKU_DIR/sai.profile ]; then
        cp $HWSKU_DIR/sai.profile /etc/sai.d/sai.profile
    fi
fi

rm -f /var/run/sswsyncd/sswsyncd.socket
supervisorctl start syncd

# Function: wait until syncd has created the socket for bcmcmd to connect to
wait_syncd() {
    while true; do
        if [ -e /var/run/sswsyncd/sswsyncd.socket ]; then
            break
        fi
        sleep 1
    done

    # wait until bcm sdk is ready to get a request
    sleep 3
}

# If this platform has an initialization file for the Broadcom LED microprocessor, load it
if [ -r ${PLATFORM_DIR}/led_proc_init.soc ]; then
    wait_syncd
    supervisorctl start ledinit
fi
