from board import *
import digitalio
import storage

# disable MSC
pin = digitalio.DigitalInOut(GP0)
pin.switch_to_input(pull=digitalio.Pull.DOWN)

if pin.value:
    # don't show USB drive to host PC
    storage.disable_usb_drive()
    print("Disabling USB drive")
else:
    # normal boot
    print("USB drive enabled")
