obj-m+=po_se_dri.o

KDIR = /lib/modules/$(shell uname -r)/build

PWD = $(shell pwd)

make:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean


