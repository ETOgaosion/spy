#
# Created by 蓝色空间 on 2022/8/31.
#

base_dir := $(readlink -f ../)
base_include_dir := $(base_dir)/include
lib_dir := $(base_dir)/lib

obj-m += spy.o
spy-objs := patch.o ioctl.o device.o
EXTRA_CFLAGS = -I/lib/modules/$(shell uname -r)/build/arch/x86/include/ -I$(base_dir)/ -I$(lib_dir) -I$(base_include_dir)/spy/

patch:
	CONFIG_MODULE_SIG=n
	CONFIG_MODULE_SIG_ALL=n
	make ARCH=x86 -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	rm -f .*.cmd *.cmd *.mod *.mod.c *.o *.symvers *.order
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	# We put a — in front of the rmmod command to tell make to ignore
	# an error in case the module isn't loaded.
	-sudo rmmod spy
	# Clear the kernel log without echo
	sudo dmesg -C
	# Insert the module
	sudo insmod spy.ko
	# Display the kernel log
	# dmesg