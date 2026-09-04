#!/bin/bash

# Robotplaeneklipper - basic motor control
# Raspberry Pi 3 + L9110S

# Left motor:  GPIO17 / GPIO27
# Right motor: GPIO22 / GPIO23

stop() {
    pinctrl set 17 op dl
    pinctrl set 27 op dl
    pinctrl set 22 op dl
    pinctrl set 23 op dl
}

forward() {
    pinctrl set 17 op dh
    pinctrl set 27 op dl
    pinctrl set 22 op dh
    pinctrl set 23 op dl
}

backward() {
    pinctrl set 17 op dl
    pinctrl set 27 op dh
    pinctrl set 22 op dl
    pinctrl set 23 op dh
}

left() {
    pinctrl set 17 op dl
    pinctrl set 27 op dh
    pinctrl set 22 op dh
    pinctrl set 23 op dl
}

right() {
    pinctrl set 17 op dh
    pinctrl set 27 op dl
    pinctrl set 22 op dl
    pinctrl set 23 op dh
}

# Start altid i STOP
stop

case "$1" in
    forward)  forward ;;
    backward) backward ;;
    left)     left ;;
    right)    right ;;
    stop)     stop ;;
    *)
        echo "Brug: $0 {forward|backward|left|right|stop}"
        exit 1
        ;;
esac
