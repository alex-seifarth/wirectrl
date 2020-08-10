#!/usr/bin/env bash

systemctl stop wirectrld
systemctl disable wirectrld

rm /usr/sbin/wirectrld
rm /etc/dbus-1/system.d/de.titnc.pi.wirectrl.conf
rm /etc/systemd/system/wirectrl.service
rm -rf /etc/wirectrl/

