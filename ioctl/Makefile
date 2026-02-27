obj-m := ioctl.o

KDIR := ~/linux
PWD := $(shell pwd)

all:
	$(MAKE) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- \
	-C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
