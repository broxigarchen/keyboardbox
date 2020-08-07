CONFIGS_ROOT=/sys/kernel/config
cd $CONFIGS_ROOT/usb_gadget/kbdbox

rm configs/c.1/hid.usb0
rmdir configs/c.1/strings/0x409
rmdir configs/c.1
rmdir functions/hid.usb0
rmdir strings/0x409
cd ..
rmdir kbdbox
