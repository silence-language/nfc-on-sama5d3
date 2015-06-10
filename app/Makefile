
export ARCH=arm
export CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-

AR=$(CROSS_COMPILE)ar
ARCH=arm
LD=$(CROSS_COMPILE)ld
CC=$(CROSS_COMPILE)gcc
DEBFLAGS=-O2
PWD:=$(shell pwd)

objects=pcda.o
pcda.o:
	$(CC) -Wall  -lpthread  -o  pcda pcda.c
	@echo 'pcda was already generated'
#	@echo 'copy pcda to share files folder'
#	@cp pcda /mnt/hgfs/sharefiles
clean:
	rm -rf $(objects)cd
