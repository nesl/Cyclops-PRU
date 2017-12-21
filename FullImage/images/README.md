### 4.1.12-bone-rt with QoT
I recommend using the [nesl-qot-4.1.12-bone-rt-r16-4gb](https://drive.google.com/file/d/0B-N9bMQfpW6gdUFLUjVvVHR0ZFk/view?usp=sharing) image which has the QoT stack installed.
You will neet the QoT stack in order to run the time synchronization examples.

### 3.8.13-bone50 Vanilla
Below, I describe the steps for creating the [debian-7.5-kernel-3.8.13-bone50-16gb-nesl](https://github.com/yifanz/ucla-nesl-pru-sys-images/blob/master/images/debian-7.5-kernel-3.8.13-bone50-16gb-nesl.img.xz) image.
You only need to read this if you want to make your own image.

I built my image based on the official [`Debian 7.5 (BeagleBone, BeagleBone Black - 2GB SD) 2014-05-14`](https://debian.beagleboard.org/images/bone-debian-7.5-2014-05-14-2gb.img.xz) image which has the `3.8.13-bone50` kernel.
I don't recommend the newer versions for the following reasons: USB ethernet instability, significantly slower boot time and the latest TI kernels force you to use `remote_proc` instead of the better documented and tested `prussdrv` for interfacing with the PRUs. You can copy the image to an SD card with the `dd` command. Find the device node of your SD card. In my case it is `/dev/rdisk2`, but this most likely will NOT be the case for you. If you copy to the wrong device node, you may erase your hard disk.

```
sudo dd bs=1m if=bone-debian-7.5-2014-05-14-2gb.img of=/dev/rdisk2
```

You can terminal into the device as `root` without a password. I'm using USB over ethernet which defaults to `192.168.7.2`. You may need to use a different IP address.

```
ssh root@192.168.7.2
```

First, expand the available storage. Although you only need a 2GB SD card to hold the base image, I'm using a 16 GB SD card.

```
cd /opt/scripts/tools
./grow_partition.sh
```

I recommend disabling HDMI and eMMC because they occupy too many of the PRU's pins.
In `/boot/uboot/uEnv.txt` uncomment this:

```
##BeagleBone Black:
##Disable HDMI/eMMC
cape_disable=capemgr.disable_partno=BB-BONELT-HDMI,BB-BONELT-HDMIN,BB-BONE-EMMC-2G
```

Later, when you want to load your own custom overlays on bootup you can uncomment this and add the names of your overlays:

```
##Example
#cape_disable=capemgr.disable_partno=
cape_enable=capemgr.enable_partno=NESL-PRU,cape-universal
```

Then, `reboot` to apply the changes. You can check if it worked like this:

```
root@beaglebone:/sys/devices/bone_capemgr.9# cat slots
 0: 54:PF--- 
 1: 55:PF--- 
 2: 56:PF--- 
 3: 57:PF--- 
 4: ff:P-O-- Bone-LT-eMMC-2G,00A0,Texas Instrument,BB-BONE-EMMC-2G
 5: ff:P-O-- Bone-Black-HDMI,00A0,Texas Instrument,BB-BONELT-HDMI
 6: ff:P-O-- Bone-Black-HDMIN,00A0,Texas Instrument,BB-BONELT-HDMIN
```

Notice that the `L` is missing from `ff:P-O-L` which means that those overlays are not loaded.

If you are also using ethernet over USB, then you may need to run [`bbb-usb-net-gateway.sh`](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/scripts/bbb-usb-net-gateway.sh) on the device and [`host-usb-net-forwarding.sh`](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/scripts/host-usb-net-forwarding.sh) on the host to be able to access the internet.
Make sure you update the variables in those scripts to match your network interfaces.
If you run into DNS issues, you can try adding `nameserver 8.8.8.8` to `/etc/resolv.conf`.

Next, install the [PRU code generation tools](http://software-dl.ti.com/codegen/non-esd/downloads/beta.htm) from TI. I'm using `v2.0.0 Beta 2` for `ARM`. Newer versions are available, but this one has better documentation and more code examples.  You have to create a TI account and agree to their license. Once you do, you will be allowed to download a 40MB file called `ti_cgt_pru_2.0.0B2_armlinuxa8hf_installer.sh`. Executing it will install the tools in `pru_2.0.0B2/` under the current working directory.

It is helpful to have `git` setup so you can share code. Here is an example of how to setup your `.gitconfig`.

```
[http]
	sslverify = false
[user]
	name = Yi-Fan Zhang
	email = yifanzhang@engineering.ucla.edu
```

I would `git clone` this project as well as Derek Molloy's [exploringBB project](https://github.com/derekmolloy/exploringBB). Much of the code in this project uses code from exploringBB as a template. You should also install the latest `AM335x PRU Package` which contains `libprussdrv`.

```
git clone https://github.com/beagleboard/am335x_pru_package.git
cd am335x_pru_package
make CROSS_COMPILE=""
make install PREFIX=/usr CROSS_COMPILE=""
```
