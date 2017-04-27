### QoT and PRU Time Synchronization Accuracy

#### Pins
* __P8_46__: PRU output for time sync.
* __P8_45__: PRU input for reporting timestamp.
* __P8_9__: QoT input capture for time sync.
* __P8_10__: QoT input capture for reporting timestamp.

__P8_46__ is connected to __P8_9__ and this connection is used for time synchronization between the PRU and main processor. 
Asserting __P8_45__ causes the PRU to report its current timestamp. Similarily asserting __P8_10__, reports the current timestamp from the main processor.
__P8_45__ and __P8_10__ are connected to a PWM output from an external microcontroller which will periodically assert both pins simultaneously to produce timestamps for comparing the synchronization accuracy.

The PRU initiates time synchronization by asserting __P8_46__ and records its own timestamp at that instant (__T1__).
Since __P8_46__ is connected to __P8_9__, this will produce an input capture timestamp (__T2__) on the main processor.
__T2__ is delivered to the PRU by a user-space program and the PRU will subtract __T1__ and __T2__ to calculate the offset and adjust its own clock.

#### Results

Test run lasts approximately 33 minutes.
For each run, time synchronization between the PRU and the main procesor occurs every 8 or 32 seconds or just once at the beginning. 
Once the PRU and main processor are synchronized, we assert __P8_45__ and __P8_10__ 
every 250 milliseconds and record the timestamps from the PRU and main processor.
We use the difference between the reported timestamps as a measure for the accuracy of synchronization.

The csv files contain the timestamps from each run. The __left column is the QoT timestamp__, the __middle column is the PRU timestamp__ the __right column is the time difference__.
All values are in nanoseconds. 

* [sync-8-sec_sample-250-ms.csv](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/examples/measure_time_sync/sync-8-sec_sample-250-ms.csv) (sync every 8 sec, test every 250 ms)
* [sync-32-sec_sample-250-ms.csv](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/examples/measure_time_sync/sync-32-sec_sample-250-ms.csv) (sync every 32 sec, test every 250 ms)
* [sync-once_sample-250-ms.csv](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/examples/measure_time_sync/sync-once_sample-250-ms.csv) (sync only once at beginning then test every 250 ms)
