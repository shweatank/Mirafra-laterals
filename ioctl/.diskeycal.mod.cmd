savedcmd_/home/mirafra/drivers/ioctl/diskeycal.mod := printf '%s\n'   diskeycal.o | awk '!x[$$0]++ { print("/home/mirafra/drivers/ioctl/"$$0) }' > /home/mirafra/drivers/ioctl/diskeycal.mod
