savedcmd_keybo_driver.mod := printf '%s\n'   keybo_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > keybo_driver.mod
