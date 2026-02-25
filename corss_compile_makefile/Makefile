ARCH ?= arm64                            # arm for 32-bit Pi, arm64 for 64-bit
CROSS_COMPILE ?= aarch64-linux-gnu-      # arm-linux-gnueabihf- for 32-bit

KDIR ?= $(HOME)/rpi-headers            

obj-m += hello_driver.o

all:
	make -C $(KDIR) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

clean:
	make -C $(KDIR) M=$(PWD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean