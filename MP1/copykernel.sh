sudo mount -o loop MP1.img /mnt/floppy
sudo cp kernel.bin /mnt/floppy/
sleep 1s
sudo umount /mnt/floppy/
