#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

int main(int argc, const char *argv[]) {
	int pru_num;

	if (argc != 3) {
		printf("Usage: %s <pru_code.bin> <pru num>\n", argv[0]);
		fflush(stdout);
		return 1;
	}

	pru_num = atoi(argv[2]);

	prussdrv_init();

	if (prussdrv_open(PRU_EVTOUT_0) == -1) {
		printf("prussdrv_open() failed\n");
		fflush(stdout);
		return 1;
	}

	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	prussdrv_pruintc_init(&pruss_intc_initdata);

	int which_pru = 0;

	if (pru_num == 0) {
		which_pru = 0;
	} else if (pru_num == 1) {
		which_pru = 1;
	}

	void* dataram;
	if (prussdrv_map_prumem(which_pru ? PRUSS0_PRU1_DATARAM : PRUSS0_PRU0_DATARAM, &dataram)) {
		printf("pru mem map failed\n");
		fflush(stdout);
		return 1;
	}
	memset(dataram, 0, 0x2000);

	printf("Executing program and waiting for termination\n");
	fflush(stdout);
	prussdrv_exec_program(which_pru, argv[1]);

	// Wait for interrupt from PRU
	//prussdrv_pru_wait_event(PRU_EVTOUT_0);
	
	while(1) {
		uint32_t* p = (uint32_t*) dataram;

		// status code = 1 is print
		if (*p == 1) {
			uint32_t num_args = *(++p);
			for (int i = 0; i < num_args; i++) {
				uint32_t arg = *(++p);
				if (i == num_args - 1) {
					printf(" %u [0x%lx]", arg, arg);
					fflush(stdout);
				} else {
					printf(" %u [0x%lx],", arg, arg);
					fflush(stdout);
				}
			}
			printf("\n");
			fflush(stdout);
			p = (uint32_t*) dataram;
			*p = 0;
		} else if (*p == 2) {
			// terminate
			printf("Terminate\n");
			fflush(stdout);
			break;
		}
	}

	prussdrv_pru_disable(which_pru);
	prussdrv_exit();

	return 0;
}
