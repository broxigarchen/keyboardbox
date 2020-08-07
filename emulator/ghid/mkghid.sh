#!/bin/bash
#https://www.kernel.org/doc/Documentation/usb/gadget_configfs.txt

CONFIGS_ROOT=/sys/kernel/config
cd $CONFIGS_ROOT/usb_gadget
mkdir -p kbdbox
cd kbdbox

#USB ids
echo 0x1d6b > idVendor  # Linux Foundation
echo 0x0104 > idProduct # Multifunction Composite Gadget
echo 0x0100 > bcdDevice    # v1.0.0
echo 0x0200 > bcdUSB       # USB2
echo 0x03 > bDeviceClass   #hid
echo 0x00 > bDeviceSubClass
echo 0x00 > bDeviceProtocol

#USB strings
mkdir -p strings/0x409
echo "fedcba9876543210" > strings/0x409/serialnumber
echo "broxigarchen" > strings/0x409/manufacturer
echo "Keyboard box" > strings/0x409/product

#USB configuration
mkdir -p configs/c.1/strings/0x409
echo "Config 1: keyboard" > configs/c.1/strings/0x409/configuration
echo 250 > configs/c.1/MaxPower

#USB kbd function
mkdir -p functions/hid.usb0
echo 0 > functions/hid.usb0/protocol
echo 0 > functions/hid.usb0/subclass
echo 8 > functions/hid.usb0/report_length
echo 63 > functions/hid.usb0/report_desc_length
echo -ne \\x05\\x01\\x09\\x06\\xA1\\x01\\x05\\x07\\x19\\xE0\\x29\\xE7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xC0 > functions/hid.usb0/report_desc

#bind function to config
ln -s functions/hid.usb0 configs/c.1

#bound to UDC
ls /sys/class/udc > UDC
