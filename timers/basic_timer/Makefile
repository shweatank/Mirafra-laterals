obj-m+=timer_dri.o

KDIR = /lib/modules/$(shell uname -r)/build

PWD = $(shell pwd)

make:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean