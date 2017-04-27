Make sure you have loaded the necessary DT [overlays](https://github.com/yifanz/ucla-nesl-pru-lib/tree/master/overlay).
Run `./config-pins`.
Then connect P9\_27 to P8\_10 on the beaglebone (see image below).

![alt text](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/examples/time_sync/time_sync_setup.jpg)

Here is roughly what happens during synchronization:

1. The PRU will send a pulse on P9\_27 and record the its own time (T1).
2. This will trigger the input capture configured on P8\_10 to record the time the pulse was received (T2).
3. The user-space program reads T2 from "/dev/ptp\*" and sends it to the PRU.
4. The PRU calculates its offset simply by subtracting T2 minus T1.

Here is some example output. The PRU will send its timestamp to the host every second. Synchronization occurs every 5 seconds.

```
root@beaglebone:~/ucla-nesl-pru-lib/examples/time_sync# ./build.sh
Compiling and linking the PRU firmware
<Linking>
~/ucla-nesl-pru-lib/examples/time_sync ~/ucla-nesl-pru-lib/examples/time_sync
Generating text and data segments (text.bin and data.bin)
Translating to Binary format...
   "pru.out" .text:_c_int00* ==> .text:_c_int00*
   "pru.out" .text ==> .text
   "pru.out" .data ==> .data
~/ucla-nesl-pru-lib/examples/time_sync
Compiling and linking the host application
Build SUCCESSFUL.
To run you must change to the gen directory:
cd gen/
./host
root@beaglebone:~/ucla-nesl-pru-lib/examples/time_sync# cd gen
root@beaglebone:~/ucla-nesl-pru-lib/examples/time_sync/gen# ./host
Device opened 5
Set pin func successful for 5
Requesting timestamps success for 5
PRU: 1531 ns
PRU: 1000015906 ns
PRU: 2000031611 ns
PRU: 3000047276 ns
QoT event time: 1487137387350957509 ns
PRU: 1487137387352118384 ns
PRU: 1487137388352133609 ns
PRU: 1487137389352149304 ns
PRU: 1487137390352164959 ns
PRU: 1487137391352180654 ns
QoT event time: 1487137392352196385 ns
PRU: 1487137392352442105 ns
PRU: 1487137393352457280 ns
PRU: 1487137394352473005 ns
PRU: 1487137395352488670 ns
PRU: 1487137396352504375 ns
QoT event time: 1487137397352519969 ns
PRU: 1487137397352756659 ns
PRU: 1487137398352771729 ns
PRU: 1487137399352787369 ns
PRU: 1487137400352802989 ns
PRU: 1487137401352818609 ns
QoT event time: 1487137402352834220 ns
PRU: 1487137402353068290 ns
PRU: 1487137403353083400 ns
PRU: 1487137404353099060 ns
PRU: 1487137405353114665 ns
PRU: 1487137406353130250 ns
QoT event time: 1487137407353145845 ns
PRU: 1487137407353385765 ns
PRU: 1487137408353400685 ns
PRU: 1487137409353416155 ns
PRU: 1487137410353431625 ns
PRU: 1487137411353447095 ns
QoT event time: 1487137412353462554 ns
PRU: 1487137412353715799 ns
```
