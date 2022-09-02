#
# Created by 蓝色空间 on 2022/8/31.
#

base_dir = ../
base_include_dir = $(base_dir)/include
lib_dir = ../lib
linux_lib_dir = /lib/modules/$(shell uname -r)/build
cur_include_dir = $(base_dir)/module/include

obj-m += spy.o
spy-objs := patch.o ioctl.o device.o ../lib/string.o
EXTRA_CFLAGS = -I$(linux_lib_dir)/arch/x86/include/ -I$(base_dir)/ -I$(lib_dir) -I$(base_include_dir)/spy/ -I$(cur_include_dir)

../lib/string.o:
	make -C $(lib_dir) string

patch: ../lib/string.o
	CONFIG_MODULE_SIG=n
	CONFIG_MODULE_SIG_ALL=n
	make ARCH=x86 -C $(linux_lib_dir) M=$(PWD) modules
	rm -f $(lib_dir)/.*.cmd .*.cmd *.cmd *.mod *.mod.c *.o *.symvers *.order
clean:
	make -C $(lib_dir) clean
	make -C $(linux_lib_dir) M=$(PWD) clean
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