all:
	mkdir bin && \
	nasm -f bin ./bootloader/boot.asm -o ./bin/boot.bin
	dd if=./test_message.txt >> ./bin/boot.bin
	dd if=/dev/zero bs=512 count=1 >> ./bin/boot.bin

test:
	mkdir bin && \
	nasm -f bin ./bootloader/boot.asm -o ./bin/boot.bin && \
	cd bin && \
	dd if=../test_message.txt >> ./boot.bin
	dd if=/dev/zero bs=512 count=1 >> ./boot.bin
	qemu-system-x86_64 -hda boot.bin && \
	cd ..
