savedcmd_usb_driver.mod := printf '%s\n'   usb_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > usb_driver.mod
