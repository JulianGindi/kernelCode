obj-m += hello_sys.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
