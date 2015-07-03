#!/bin/sh
hciconfig hci0
hciconfig hci0 up
hciconfig hci0 piscan
hciconfig hci0

ps -edf | grep bluetooth
/etc/init.d/bluetooth stop
/usr/lib/bluetooth/bluetoothd -C &
ps -edf | grep bluetooth
sleep 1
sdptool browse local
sdptool add --channel=22 SP
sdptool browse local
