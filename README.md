eqnx
====

Updated Equinox SuperSerial driver for Linux 3.7 and higher kernels

This driver is a mess however I was able to make it work with the tty changes
in the 3.7 and higher linux kernel.  I updated the references to ktermios
and added tty ports.  


BUGS:
systemd and/or agetty seem keep the driver in use if you have agetty setup
on a serial port at boot.  This makes lsmod show the module as in use when
lsof shows nothing using the ttyQ drives.  This does not happen when the 
module is not loaded at boot or agetty is not configured to use the port.
This bug seems to affect the 12d driver which was the last one released
by the Equinox before they got bought out too many times.

