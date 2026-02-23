savedcmd_top_int.mod := printf '%s\n'   top_int.o | awk '!x[$$0]++ { print("./"$$0) }' > top_int.mod
