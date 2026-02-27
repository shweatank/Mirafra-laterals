savedcmd_/home/mirafra/linux/hello/hello_driver.mod := printf '%s\n'   hello_driver.o | awk '!x[$$0]++ { print("/home/mirafra/linux/hello/"$$0) }' > /home/mirafra/linux/hello/hello_driver.mod
