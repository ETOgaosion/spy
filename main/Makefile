obj-m += spy_run.o
run:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	rm -f .*.cmd *.cmd *.mod *.mod.c *.o *.symvers *.order
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	# We put a — in front of the rmmod command to tell make to ignore
	# an error in case the module isn't loaded.
	-sudo rmmod spy_run
	# Clear the kernel log without echo
	sudo dmesg -C
	# Insert the module
	sudo insmod spy_run.ko
	# Display the kernel log
	# dmesg