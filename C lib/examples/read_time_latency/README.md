### Measure latency for reading/calculating PRU timestamp

Measure the latency for `iep_get_time()`, `iep_adj_time()` and reading the IEP counter.

PRU is 200 MHz so 1 cycle is 5 ns.
There is no cache or pipeline in the PRU architecture so every instruction (as long as it does not access main processor resources) executes deterministically.
In this test, it appears that all functions are completely deterministic and they take the same number of cycles to execute everytime.

* `iep_get_time()` = 124 cycles = 620 ns
* `iep_adj_time()` = 58 cycles = 290 ns
* Reading IEP counter (32-bit) = 23 cycles = 115 ns

`iep_get_time()` is used to get the current 64-bit PRU timestamp. It includes reading the IEP counter and handles roll over checking.

`iep_adj_time()` is a simpler function used to adjust the current PRU timestamp based on it's offset with respect to an external timestamp.
It does not read the IEP counter.
