savedcmd_/home/mirafra/drivers/ioctl/ioctl_cal.mod := printf '%s\n'   ioctl_cal.o | awk '!x[$$0]++ { print("/home/mirafra/drivers/ioctl/"$$0) }' > /home/mirafra/drivers/ioctl/ioctl_cal.mod
