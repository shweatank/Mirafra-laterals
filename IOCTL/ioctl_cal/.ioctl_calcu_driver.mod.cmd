savedcmd_ioctl_calcu_driver.mod := printf '%s\n'   ioctl_calcu_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > ioctl_calcu_driver.mod
