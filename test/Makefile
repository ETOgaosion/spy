#
# Created by 蓝色空间 on 2022/8/31.
#

obj-m += test.o
CARGS = -I /lib/modules/$(shell uname -r)/build/arch/x86/include/
all:
	make ARCH=x86 -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	rm -f .*.cmd *.cmd *.mod *.mod.c *.o *.symvers *.order
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	# We put a — in front of the rmmod command to tell make to ignore
	# an error in case the module isn't loaded.
	-sudo rmmod test
	# Clear the kernel log without echo
	sudo dmesg -C
	# Insert the module
	sudo insmod test.ko
	# Display the kernel log
	# dmesg