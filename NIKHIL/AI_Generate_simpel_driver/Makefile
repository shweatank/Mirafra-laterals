KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
DRIVER_NAME := reverse_driver
USER_APP := user_app

# Variable for module compilation
obj-m += $(DRIVER_NAME).o

# Default target
all: module user_app

# Build kernel module
module:
	@echo "Building kernel module: $(DRIVER_NAME)"
	make -C $(KERNEL_DIR) M=$(PWD) modules

# Build user space application
user_app:
	@echo "Building user space application: $(USER_APP)"
	gcc -o $(USER_APP) $(USER_APP).c

# Load module (requires root)
insmod:
	@echo "Loading module $(DRIVER_NAME).ko"
	sudo insmod $(DRIVER_NAME).ko
	@echo "Device created at /dev/$(DRIVER_NAME)"
	@ls -l /dev/reverse_char_dev 2>/dev/null || echo "Device file not yet visible, may need to check /dev directory"

# Unload module (requires root)
rmmod:
	@echo "Removing module $(DRIVER_NAME)"
	sudo rmmod $(DRIVER_NAME) || true

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts"
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.* $(USER_APP)
	rm -f Module.symvers modules.order
	rm -f .$(DRIVER_NAME).* .test*

# Display dmesg for driver messages
dmesg:
	@echo "Recent kernel messages:"
	dmesg | tail -20

# Run test (execute user app)
run: user_app
	@echo "Running user application with default input 'Nikhil'"
	./$(USER_APP)

# Run with custom input
run_custom: user_app
	@if [ -z "$(INPUT)" ]; then \
		echo "Usage: make run_custom INPUT=\"your_string\""; \
		exit 1; \
	fi
	./$(USER_APP) "$(INPUT)"

# Show help
help:
	@echo "Makefile targets:"
	@echo "  make all           - Build kernel module and user app (default)"
	@echo "  make module        - Build kernel module only"
	@echo "  make user_app      - Build user space application only"
	@echo "  make insmod        - Load kernel module (requires sudo)"
	@echo "  make rmmod         - Unload kernel module (requires sudo)"
	@echo "  make run           - Build and run user app with default 'Nikhil'"
	@echo "  make run_custom    - Run user app with custom input (e.g., make run_custom INPUT=\"hello\")"
	@echo "  make dmesg         - Show kernel messages"
	@echo "  make clean         - Remove build artifacts"
	@echo "  make help          - Show this help message"

.PHONY: all module user_app insmod rmmod run run_custom clean dmesg help
