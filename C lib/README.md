# UCLA NESL PRU Library

There are two approaches to interfacing with the PRU subsystem (PRUSS).
Older kernels (pre-4.1.x) generally use the user-space IO (UIO) driver and newer ones use [`remote_proc`](https://www.kernel.org/doc/Documentation/remoteproc.txt) and [`RPMsg`](https://www.kernel.org/doc/Documentation/rpmsg.txt).
The UIO driver simply maps regions of PRUSS memory and registers directly into user-space with a minimal kernel part for handling interrupts.
On the other hand, the `remote_proc` framework provides a high level interface for controlling (power on, load firmware, power off) heterogeneous remote processor systems as well as the ability to map ELF formatted binaries into PRU memory.
`RPMsg` provides a communication channel between the PRU and the main processor using `virtio` based messaging.
User-space programs are expected to use syscalls on device nodes (e.g. `/dev/rpmsg31`) to access the PRU (direct access may still be possible using `/dev/mem`, but I have not tested this).

Despite these advantages, we chose to use the UIO driver because it is better documented and there exists more code examples from the community.
We also prefer the low latency properties of accessing PRU memory mapped directly into user-space over the more throughput optimized approach of `RPMsg`.
Finally, the UIO driver is more portable and works on both old and new Linux kernels. Even the corresponding PRU driver on the FreeBSD operating system takes a similar approach.

## Quick Start

The easiest way to get started is to boot with our [image](https://github.com/yifanz/ucla-nesl-pru-sys-images#how-to-flash-to-sd-card) which contains this repository along with all of the necessary dependencies. This helps avoid the 'works on my machine' syndrome and should save you hours of dependency management problems. If you choose not to use our image, here is roughly what you would need to install on your own system.

* A kernel which has the `uio_pruss` module. Do `lsmod | grep pru` to see if `uio_pruss` is running.
* The AM335x PRU PACKAGE ([source](https://github.com/beagleboard/am335x_pru_package)). Check under `/usr/local/include` and `/usr/local/lib` to see if your distribution already has `prussdrv.h` and `libprussdrv.so` installed.
* TI's PRU code generation tools ([binaries](http://software-dl.ti.com/codegen/non-esd/downloads/beta.ht)). Check `/usr/share/ti/cgt-pru` to see if your distribution already has this installed.

Assuming that you are using our image, you can terminal into the device using `root` as the username without any password. Here is an example of how to run the `gpio` example:

```
cd ucla-nesl-pru-lib/examples/gpio
./build.sh
./config-pins.sh
cd gen/
./host
```

As a convention, all examples have a `pru_main.c` and `host_main.c` which correspond to the parts that runs on the PRU and Linux, respectively. Take a look at `build.sh` if you want to change this.
Every project under the [`examples`](https://github.com/yifanz/ucla-nesl-pru-lib/tree/master/examples) directory can be used as a template. Simply copy them elsewhere to create your own project.
