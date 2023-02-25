The nesl-qot-4.1.12-bone-rt-r16-4gb image is available at the following link
[link](https://drive.google.com/file/d/1BbqrlW7soqthktslKXc-ADzNKqw2H8XY/view?usp=share_link).


# UCLA NESL PRU System Images

This repository holds the OS images for working on the NESL PRU library.
The images have all the necessary software dependencies installed and configured.
This is aimed to provide a reproducible environment for anyone who wants wants to get involved in the project.

## How to Flash to SD Card

Download [nesl-qot-4.1.12-bone-rt-r16-4gb.img.xz](https://github.com/nesl/Cyclops-PRU/blob/master/FullImage/images/nesl-qot-4.1.12-bone-rt-r16-4gb.img.xz).


After decompressing the image with `xz`, copy the image to an SD card with the `dd` command. Find the device node of your SD card. In my case it happens to be `/dev/rdisk2`, but this most likely will **NOT** be the case for you. If you copy to the wrong device node, you may **erase your hard disk**.

```
xz -d nesl-qot-4.1.12-bone-rt-r16-4gb.img.xz
sudo dd bs=1m if=nesl-qot-4.1.12-bone-rt-r16-4gb.img of=/dev/rdisk2
```
If you want to create your own image or understand how we built our image, see [here](https://github.com/nesl/Cyclops-PRU/blob/master/FullImage/images/README.md).
