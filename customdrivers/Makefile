obj-m += mycuscalculator.o

KDIR := /lib/modules/$(shell uname -r)/build
CDIR := /home/mirafra/drivers/customdrivers/linux
PWD  := $(shell pwd)

ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-

all:
	make -C $(CDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(CDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
