savedcmd_po_se_dri.mod := printf '%s\n'   po_se_dri.o | awk '!x[$$0]++ { print("./"$$0) }' > po_se_dri.mod
