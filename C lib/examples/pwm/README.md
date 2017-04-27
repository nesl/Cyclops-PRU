### Code Comparison

__C__ Language Example ([pru_main.c](https://github.com/yifanz/ucla-nesl-pru-lib/blob/master/examples/pwm/pru_main.c))
```
#include "nesl_pru_intc.h"
#include "nesl_pru_gpio.h"
#include "nesl_pru_wait.h"

void pwm(enum pru_pin pin,
        unsigned int period_us, unsigned int pulse_width_us)
{
    // Keep sending PWM signal until P8_45 is asserted.
    while(read_pin(P8_45) == 0) {
        assert_pin(pin);
        WAIT_US(pulse_width_us);
        deassert_pin(pin);
        WAIT_US(period_us - pulse_width_us);
    }
}

int main()
{
    // Ouput PWM signal with period of 1 second and
    // pulse width of 0.5 seconds.
    pwm(P8_46, 1000000, 500000);

    // Terminate program
    TRIG_INTC(3);
    __halt();
}
```

__Cyclops__ high-level language PWM example (https://github.com/yifanz/CSM213A).
The `tick` is implemented using the cycle counter. Setting `tick = 0` resets the cycle counter.
This language is in a prototype phase. We currently don't support function declarations in the grammar.
```
// PWM with 1 second period and pulse width of 0.5 seconds.
while(P8_45 == 0) {
    // Each tick is 5 nanoseconds.
    tick = 0;
    
    // Output High
    P8_46 = 1;
    
    // Wait for 0.5 seconds (2 * 10^8 cycles)
    while (tick < 0xBEBC200) {}
    
    tick = 0;
    
    // Output Low
    P8_46 = 0;
    
    // Wait for 0.5 seconds (2 * 10^8 cycles)
    while (tick < 0xBEBC200) {}
}
```

__PASM__ example (adapted from Derek Molloy's "[Exploring Beaglebone](http://exploringbeaglebone.com)" Chapter 13)
```
.origin 0
.entrypoint START

#define PRU0_R31_VEC_VALID 32
#define EVENTOUT0 3

// PWM with 1 second period and pulse width of 0.5 seconds.

START:
    MOV	r0, 0xBEBC200                   // count for 0.5 seconds (2 * 10^8 cycles)

MAINLOOP:

    SET	r30.t1                          // set the output pin P8_46 high
    
    MOV r1, r0                          // wait for 0.5 seconds
HOLD_HIGH:
    SUB	r1, r1, 1
    QBNE    HOLD_HIGH, r1, 0
    
    CLR	r30.t1                          // set the output pin P8_46 low
    
    MOV r1, r0                          // wait for 0.5 seconds
HOLD_LOW:
    SUB	r1, r1, 1
    QBNE    HOLD_LOW, r1, 0

    QBBS    END, r31.t0                 // quit if input pin P8_45 is asserted
    QBA	MAINLOOP
    
END:                               
    MOV	R31.b0, PRU0_R31_VEC_VALID | EVENTOUT0      // Terminate program
    HALT
```
