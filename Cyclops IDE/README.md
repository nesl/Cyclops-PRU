#### Table of Contents

1. [Cyclops](#project-proposal)
  * [Objectives](#project-objectives)
2. [Background](#background)
  * [Programmable Realtime Unit Subsystem](#programmable-realtime-unit-subsystem)
  * [Device Tree Configuration](#device-tree)
3. [Design](#design)
  * [Kernel Modules](#kernel-modules)
  * [PRU Loader](#pru-loader)
  * [Programming Language and Compiler](#programming-language-and-compiler)
  * [IDE](#ide)
4. [References](#references)
5. [Open Source](#open-source-attributions)

### Cyclops: PL + Compiler + IDE for making PRUs easier to use

![alt text](https://github.com/yifanz/CSM213A/raw/master/images/ide_screenshot.png "ide")

The Sitara AM335x SoC on the Beaglebone Black development board features an on-chip programmable real-time unit (PRU) subsystem consisting of two 32-bit RISC cores.
Key features include a deterministic RISC instruction set with single cycle access to I/O pins, access to all resources on the SoC and an interrupt controller for capturing system-wide events.
Having a separate PRU subsystem allows for software emulation of peripheral hardware without taking time away from the main processor.
Moreover, the general purpose programmability of the PRUs affords more fexibility compared to custom ASICs without the increased board space, materials and development cost of adding an FPGA.

Despite these advantages, PRUs are often overlooked by application developers because of impediments they face during development. For instance, the PRUs are not enabled by default which requires developers to learn how to configure them through the Linux device tree subsystem before any further development can occur. Once configured, developers then need to learn the low level bits of the PRU and implement boilerplate logic for loading program binaries and communication with the PRU. Furthermore, the only languages supported by TI are C/C++ and the PRU assembly language. In either case, all user interaction is performed on the commandline.

It would be convenient to have a [mbed](https://developer.mbed.org) style development environment along with a high level language for experimenting on the PRU. The aim of this project is to make PRU programming easier by adding automated configuration for device tree setup, creating a high level programming language for the PRU and provide an IDE which hides all the boilerplate and configuration. This will be a deep dive on language and compiler implementation for the PRU hardware as well as a bit of Linux kernel module development.

#### Objectives

1. Automate PRU and pinmux setup that will handle the Linux device tree configuration.
2. Implement a custom kernel driver for the pinmux to get around limitations of device tree overlays.
3. Design a language aimed at making PRU programming easier and implement the corresponding compiler.
4. Create a browser based IDE (similar to mbed) for prototyping on the PRU.

## Background

This project targets the Beaglebone Black Rev C running the default Debian Linux (3.8+ kernel) image. We will be dealing primarily with the on-chip Programmable Realtime Unit Subsystem (PRUSS) and the device tree subsystem on Linux.

### Programmable Realtime Unit Subsystem

The PRUSS functions as an on-chip peripheral that is capable of general purpose computation as well as fast digital I/O. Within the PRUSS, there are two 32-bit PRU cores. Each is clocked at 200 Mhz (5 ns per instruction) and has its own dedicated instruction and data RAM (8Kb each) in addition to 12 Kb of shared RAM between each core. The PRUs also hace full access to the main memory as well as other on-chip peripherals, but accessing them from the PRU requires sending data across the higher latency L3 network-on-chip interconnect. However, certain pins classified as fast digital I/O pins with single cycle access latency can be assigned directly to the PRUs through a pinmux configuration.

<p align="center">
  <img src="https://github.com/yifanz/CSM213A/raw/master/images/pru.png" alt="PRUSS" width="500">
</p>

### Device Tree

When a system boots up, the kernel needs to learn what memory address ranges correspond to peripheral hardware registers.
A naive approach would be to hardcode this information into the kernel sources, but this would require a separate build of the kernel for each hardware platform with a different configuration.
PC-style systems, solve this by providing onboard firmware (e.g. BIOS or UEFI) that will fill out in-memory system descriptor tables according to an industry standard format (ACPI).
Hotplugged hardware is handled by bus protocols like PCI which support self-discovery and runtime reconfiguration.

Unlike PCs, hardware in embedded systems is typically fixed for a specific use case with tight performance requirements and are often used in proprietary settings.
Running hotplug and self-discovery firmware may interfere with performance requirements or may simply be no worth the additional complexity.

For these cases, Linux adopted a solution called device tree, sometimes referred to as Open Firmware or flattened device tree (FDT).
In short, device tree is a standard binary data structure for describing non-discoverable hardware to the kernel.
There are no strict rules for what can be described in the device tree, but they typically contain parameters for hardware register addresses, interrupts and hardware specific metadata.
Linux parses the device tree and provides a querying API to device drivers.

Initially, the device tree can be written by users in a `dts` text format which can then be serialized into a `dtb` binary using the `dtc` userspace utility. The `dtb` can be copied into the boot partition and the bootloader can be configured to pass the `dtb` to the kernel in a parameter. This way, a kernel compiled for a particular architecture can be supported on different hardware platforms of the same architecture simply by specifying a different `dtb`.

## Design

The system can roughly be divided into the following components: a kernel module for pinmux configuration, a PRU loader for handling PRU communication, a high level programming language (Cyclops) for quick prototyping, a compiler implemented entirely in javascript for portability and a browser based IDE that ties everything together. As shown in the diagram below (Cyclops components in blue text), the components form a vertical slice of the system starting from the high level user-facing application all the way down to the kernel module for interacting with the hardware.

<p align="center">
  <img src="https://github.com/yifanz/CSM213A/raw/master/images/cyclops_diag.png" alt="PRUSS" width="600">
</p>

### Kernel Modules

There are two main on-chip peripherals Cyclops needs to manipulate: the pinmux controller and the PRUSS. The pinmux controller needs to be reconfigured during runtime in order to support dynamically remapping pins to and from the PRU. The PRUSS is obviously needed to execute programs on the PRU.

#### libpruss UIO driver

Libpruss is a UIO (userspace IO) driver shipped with the Linux distribution for controlling the PRUSS. In general, UIO is a Linux kernel subsystem for mapping hardware MMIO (memory mapped IO) address ranges directly into userspace. This way, most of the driver logic can live as a userspace library and only a minimal amount of code needs to run in the kernel.

#### pin-pirate LKM

The device tree system is limited in that you must reboot the system if you wish to change the configuration. In order to change the device tree during runtime, one would need to dynamically determine which hardware components will be affected and then reinitialized all of their corresponding drivers and transitive driver dependencies. Clearly this is non-trivial; however, versions 3.17 or later of the mainline Linux kernel include a subsystem known as "device tree overlay" which allows reconfiguring the device tree without rebooting. In fact, the developers of the BeagleBone Black were innovators in this area because they had introduced the "bone cape manager" which allowed reconfiguring the device tree without rebooting to support different hardware "capes" (addons) long before device tree overlays made it into the mainline kernel.

In any case, the complex nature of device tree reloading makes both the bone cape manager and device tree overlay susceptible to kernel panics particularily upon repeated and frequent configuration changes. Rather, noth systems appear to be designed for one time configuration changes typically right after successful bootup. This way, one can maintain a stable device tree configuration for booting and another for experimentation. The intention seems to help you avoid accidentally making your system unbootable.

In the case of Cyclops, we need to make configuration changes everytime a new program that needs a different set of pins is executed. As explained previously, device tree overlays are not suitable for this use case. This motivates the need for the pin-pirate Linux kernel module (LKM). Pin-pirate is a simple LKM that writes directly in the pinmux controller's hardware registers. It is exposed to userspace as a simple character device through the device node located at `/dev/pinpirate`. If `udev` rules are setup properly on the system, then you can access this node without root permissions. To change a pin configuration one can simply run `echo 0x34 0x06 > /dev/pinpirate` with the register offset and value, respectively. The addresses and meaning of register values can be found in TI AM335x Technical Reference Manual section 9.3.1
"CONTROL_MODULE Registers". The manual also notes in section 9.1 that the MPU (memory protection unit) will need to be in privileged mode of operation and writes will not work from user mode. In other words, you cannot simply `mmap` the control register region you want through the memory device node `/dev/mem` using a userspace program. Thus, pin-pirate had to be implemented as a kernel module. Lastly, to compile the module from source, you will need ensure that you have installed the Linux kernel headers for the version of the kernel you are using (see [example](https://github.com/yifanz/CSM213A/blob/master/term-proj/scripts/install-BBB-kernel-headers.sh)).

### PRU Loader

The PRU loader is a simple Linux userspace application for loading binaries into the PRU and facilitating communication between the PRU and Linux. For this project, only the communication is the `print` function and a PRU program termination signal. More important are the collection of shell scripts located under [scripts](https://github.com/yifanz/CSM213A/tree/master/term-proj/scripts) directory that automate some routine tasks. Below is a description of each script:

* `beaglebone-net-gateway.sh` Run this when on the BBB to set your default gateway so you can access the network. You may need to change the IP addresses and the name of the network interface to match your setup. I'm running off the usb ethernet interface with the default IP.
* `beaglebone-net-forwarding.sh` If you are connecting the BBB to your computer using the usb ethernet, then you probably need to setup your iptable configuration with this script. This script is to be executed on your host computer. Again, you may need to change the name of the network interface and IP address to match your setup. The MASQUERADE flag allows your computer to behave sort of like an NAT gateway for your BBB. All traffic from the BBB will appear to be from your computer to external nodes.
* `update-time.sh` Run this on the BBB to update your local time via ntp. The BBB does not maintain time between power cycles.
* `activate-pruss.sh` The PRUSS is disabled by default on the BBB. Run this to turn it on. Run `./activate-pruss.sh unload` to turn it back off.
* `list-capemgr-slots.sh` Convenience script to print out what bone cape manager slots are active. While we use the pin-pirate LKM to map pins, turning the PRUSS on with the `activate-pruss.sh` script will load a minimal device tree configuration that turns on the PRUSS (see [PRU-ACTIVATE-00A0.dts](https://github.com/yifanz/CSM213A/blob/master/term-proj/device-tree/PRU-ACTIVATE-00A0.dts)).
* `start-ide.sh` Starts the webserver that hosts the IDE. Browse to `http://<IP of BBB>:8081`. You can configure the IP address and port in [start.py](https://github.com/yifanz/CSM213A/blob/master/term-proj/ide/start.py#L59).

### Programming Language and Compiler

Cyclops provides a high level programming language that compiles directly into PRU assembly code. The syntax of the language is a limited subset of javascript and is intended to have a minimal learning curve and serve as a way to quickly experiment with the PRU. To keep within the timeframe of this project, the language features were kept barebone. The language currently supports arithmetic expressions, conditional expressions, assignments, while loops, if/else conditionals and a print statement.

Variables do not need to be explicitly declared in Cyclops. Upon first assignment, they become implicitly declared and are assigned a global scope. The number of variables in a program is limited to the size of the register file. While this seems limiting, it ensures that there will never to unexpected latencies from register spilling and helps keep the compiler implementation simple. Furthermore, PRUs generously offer 32 registers for general use. Cyclops reserves `r0` for use as the zero register and `r1` for for use as a stack pointer for passing arguments to the print function. In the case of arithmetic expressions, intermediate results are assigned to temporary registers that are scoped to the current statement. The range of registers available for general use and temporary use can be configured [here](https://github.com/yifanz/CSM213A/blob/master/term-proj/ide/views/grammar.tpl#L4). A future improvement would be adding liveness analysis to optimize the register allocation.

Symbols referring to external fast GPIO pins follow this syntax: `P<header>_<pin#>`. The compiler infers whether or not a pin is for input or output based on the first reference. A statement assigning a 1 or 0 to the pin implies that the pin should be configured for output. If the pin is used in a conditional expression within an `if` or `while` statement, then that pin should be set as a pulldown input pin. A compiler error is generated if a pin is being used for both input and output in the same program.

### IDE

The IDE is implemented as a browser based application hosted from the Beaglebone Black. The user has the option to either use the high level Cyclops language or code directly in PRU assembly. The 'compile' button converts the Cyclops program into PRU assembly. The 'run' button sends the PRU assembly code to the BBB to be assembled by `pasm` (PRU assembler provided by Texas Instruments) and then invoked by the [PRU loader](#pru-loader). If the user is dissatisfied with the compiler's ouput, they can simply edit the generated assembler by hand before clicking on 'run'. In order to dynamically configure pin mappings, pinmux register offsets and values are parsed from comments at the top of the assembly code and send to the [pin-pirate driver](#pin-pirate-lkm) before the PRU loader is invoked (see [compile-pru.sh](https://github.com/yifanz/CSM213A/blob/master/term-proj/scripts/compile-pru.sh#L22)). Finally, any data from standard output or error are piped back into the IDE's terminal pane.

## References

1. https://www.devicetree.org
2. ELC 2015 - Enhancing Real-Time Capabilities with the PRU - Rob Birkett, Texas Instruments. https://www.youtube.com/watch?v=plCYsbmMbmY
3. Molloy, Derek. Exploring BeagleBone: Tools and Techniques for Building with Embedded Linux. John Wiley & Sons, 2014.
4. https://github.com/derekmolloy
5. http://processors.wiki.ti.com/index.php/PRU_Assembly_Instructions
6. AM335x and AMIC110 Technical Reference Manual. Texas Instruments.
7. Corbet, Jonathan, Alessandro Rubini, and Greg Kroah-Hartman. Linux device drivers. " O'Reilly Media, Inc.", 2005.

## Open Source Attributions

This project uses open source code. The source code and
corresponding license information is listed below.

1. The IDE relies on the [Ace Editor](https://ace.c9.io) for the code editing panels. The syntax highlighting for the Cyclops language and PRU assembler is based on the javascript and x86 highlighter from ace. [[BSD License]](https://github.com/ajaxorg/ace/blob/master/LICENSE) [[source code]](https://github.com/ajaxorg/ace)
2. The Cyclops compiler uses the [PEG.js Parser](https://pegjs.org) written by David Majda. [[MIT License]](https://github.com/pegjs/pegjs/blob/master/LICENSE) [[source code]](https://github.com/pegjs/pegjs)
3. The [Bottle.py](https://bottlepy.org) webserver is used to handle serving the browser based application and HTTP handling. [[MIT License]](https://raw.githubusercontent.com/bottlepy/bottle/master/LICENSE) [[source code]](https://github.com/bottlepy/bottle)
