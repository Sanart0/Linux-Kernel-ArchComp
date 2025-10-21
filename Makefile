ifneq ($(KERNELRELEASE),)
obj-m := hello1.o hello2.o
ccflags-y := -I$(src)/include
else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
