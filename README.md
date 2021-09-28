# Beaglebone-Device-Driver-Module
Boot Start Device Driver Module which directly controls GPIO pins f/ BeagleBone Black MCU

####Steps
    1. Insert your SD card into your computer
    2. Download latest BBB Debian distro, it will be a compressed .img
https://beagleboard.org/latest-images, uncompress it
    3. Run sha256sum on uncompressed .img to verify checksum (checksum is on the website prior)
    4. run > sudo dd if=bone-*.img of=/dev/sdb bs=1M
*****very important verify output file /sdb is correctly matched to the SD card slot!!!!
    5. You should see a rootfs Device if everything went well
    6. Need to shut off the pre-loaded LED Driver so as not interfere with ours by manually configuring uEnv.txt and modifying the default set dtb file: 
1. with SD card still in computer, open root/boot/uEnv.txt
2. add line: dtb=am335x-boneblack.dtb
3. open root/boot/dtbs/$(uname -r)/  
   then run > sudo cp am335x-boneblack.dtb ~/Desktop (or    	different location where file modification is possible) 
4. install dtc (Device Tree Compiler) in terminal and then
 run > dtc -I dtb -0 dts -f am335x-boneblack.dtb -o am335x-boneblack.dts  (This will create a modifiable DTS file)
5. Enter the newly created am335x-boneblack.dts file
6. Search for ‘leds {’ and observe that this is where the option to set LED gpio address, default trigger and default state is.
7. set all (2-5) leds default-trigger= “none”;
   Make sure not to touch anything else, verify with original file and diff command
8. save this file, then in terminal
 run >   dtc -I dts -0 dtb -f am335x-boneblack.dts -o am335x-boneblack.dtb
9. This will recreate the DTB file with the chosen options
10. run > sudo mv am335x-boneblack.dtb root/boot/dtbs/$(uname -r)/      (to put the modified file back in original location)
    7.  For the next steps I safely removed SD card from PC and inserted into BBB SD slot with power off, then I connected BBB w/ 9v power/Ethernet while holding power button. Once plugged in release power button. It should start the lights but then soon after shut off all the LEDs. 
    8. Make sure BBB is using SD card to boot from, and that the BBB has network connectivity
    9. You need to install BBB-specific kernel header files with:
sudo apt update
apt search linux-headers-$(uname -r)      (copy the filename)
sudo apt install filename
    10. Transfer Makefile, Special.c,testSpecial.c and testStarter.c to a convenient new directory on BBB  
    11. Run > make to create all required files: 
Special.ko, test, start
    12.  open root/etc/modules file: add a new line with ‘Special’
 Add nothing else especially no # or special characters
 modules auto-edits a .conf configuration file, this .conf file is responsible for auto-loading Kernel Modules during boot time
    13. You must now move to Special.ko file into /lib/modules/(uname -r)/extra
    14. Save everything and reboot the board. Run > lsmod once it hopefully boots up to verify your Special module is in there
    15. Now its time to set auto-load for the start executable which will command Special module driver to blink “Welcome to Embedded Linux”
    16. Since this needs to be set up to run during boot, you will need to understand Systemd service files.
    17. Open root/etc/systemd/system, > sudo touch special.service (or w/e name)
    18. sudo nano (or favorite editor) special.service
    19.  Paste the below into the file:
[Unit]
Description=Start Special Project starter

[Service]
Type=oneshot
ExecStart=/usr/bin/start

[Install]
WantedBy=multi-user.target
     ****ExecStart is most important, it points to location where the start executable will be placed.
20. go back to your Project files folder
 and run > sudo mv start root/usr/bin   (verify it moved here)
21. now run > sudo systemctl start Special
 ***it should start blinking the LEDs to “Welcome to Embedded Linux”
22. If the lights worked as they should, 
now run > sudo systemctl enable Special    (This enables Special.service we just created to run at boot time)
23. Hopefully everything worked, now reboot once more to see the action!!!
24. Alternatively, back in the Project files folder you can run > sudo ./test 
This will run a separate program that allows control of the LEDS in morsecode from userspace. 
