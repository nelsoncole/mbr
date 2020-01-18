PWD := $(shell pwd)

.PHONY: compiler create mount test clearscreen


compiler:
	gcc -C kmbr.c -o mkmbr
	gcc -C pcopy.c -o pcopy
	 
create:
	qemu-img create -f raw part.img 48M
	
mount:
	mkdosfs part.img
	sudo mount part.img /mnt
	sudo cp bin/stage1.bin /mnt/
	sudo cp bin/logo.bmp /mnt/
	sudo cp bin/kernel.bin /mnt/
	sudo cp bin/task.sys /mnt/
	sudo cp bin/gserver.sys /mnt/
	sudo cp bin/console.sys /mnt/
	sudo cp bin/files.sys /mnt/
	sudo umount /mnt

	./pcopy bin/stage0.bin part.img sirius.vhd

test:
	sudo qemu-system-i386 -m 64 -drive file=sirius.vhd,format=raw

clearscreen:
	clear

