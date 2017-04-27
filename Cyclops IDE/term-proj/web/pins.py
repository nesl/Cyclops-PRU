P8 = {}
P9 = {}

def _set(name, modes=None, mode=None, offset=None, enable_pullupdown=False,\
		pullup=False, input=False, slew=False):
	pin = {
		"name": name,
		"modes": [None] * 8,
		"mode": mode,
		"offset": offset,
		"enable_pullupdown": enable_pullupdown,
		"pullup": pullup,
		"input": input,
		"slew": slew
	}

	if type(modes) is dict:
		for key in modes:
			pin["modes"][key] = modes[key]
	elif type(modes) is list:
		pin['modes'] = modes

	return pin

P8[1] = _set("DGND")
P8[2] = _set("DGND")
P8[3] = _set("GPIO1_6", offset=0x018, modes={0: "gpmc_ad6", 1: "mmc1_dat6", \
		7: "gpio1[6]"})
P8[4] = _set("GPIO1_7", offset=0x01c, modes={0: "gpmc_ad7", 1: "mmc1_dat7", \
		7: "gpio1[7]"})
P8[5] = _set("GPIO1_2", offset=0x008, modes={0: "gpmc_ad2", 1: "mmc1_dat2", \
		7: "gpio1[2]"})
P8[6] = _set("GPIO1_3", offset=0x00c, modes={0: "gpmc_ad3", 1: "mmc1_dat3", \
		7: "gpio1[3]"})
P8[7] = _set("TIMER4", offset=0x090, modes={0: "gpmc_advn_ale", 2: "timer4", \
		7: "gpio2[2]"})
P8[8] = _set("TIMER7", offset=0x094, modes={0: "gpmc_oen_ren", 2: "timer7", \
		7: "gpio2[3]"})
P8[9] = _set("TIMER5", offset=0x09c, modes={0: "gpmc_be0n_cle", 2: "timer5", \
		7: "gpio2[5]"})
P8[10] = _set("TIMER6", offset=0x098, modes={0: "gpmc_wen", 2: "timer6", \
		7: "gpio2[4]"})
P8[11] = _set("GPIO1_13", offset=0x034, modes=["gpmc_ad13", "lcd_data18", \
		"mmc1_dat5", "mmc2_dat1", "eQEP2B_in", None, \
		"pr1_pru0_pru_r30_15", "gpio1[13]"])
P8[12] = _set("GPIO1_12", offset=0x030, modes=["gpmc_ad12", "lcd_data19", \
		"mmc1_dat4", "mmc2_dat0", "eQEP2A_in", None, \
		"pr1_pru0_pru_r30_14", "gpio1[12]"])
P8[13] = _set("EHRPWM2B", offset=0x024, modes=["gpmc_ad9", "lcd_data22", \
		"mmc1_dat1", "mmc2_dat5", "ehrpwm2B", None, None, \
		"gpio0[23]"])
P8[14] = _set("GPIO0_26", offset=0x028, modes=["gpmc_ad10", "lcd_data21", \
		"mmc1_dat2", "mmc2_dat6", "ehrpwm2_tripzone_in", None, None, \
		"gpio0[26]"])
P8[15] = _set("GPIO1_15", offset=0x03c, modes=["gpmc_ad15", "lcd_data16", \
		"mmc1_dat7", "mmc2_dat3", "eQEP2_strobe", None, \
		"pr1_pru0_pru_r31_15", "gpio1[15]"])
P8[16] = _set("GPIO1_14", offset=0x038, modes=["gpmc_ad14", "lcd_data17", \
		"mmc1_dat6", "mmc2_dat2", "eQEP2_index", None, \
		"pr1_pru0_pru_r31_14", "gpio1[14]"])
P8[17] = _set("GPIO0_27", offset=0x02c, modes=["gpmc_ad11", "lcd_data20", \
		"mmc1_dat3", "mmc2_dat7", "ehrpwm0_synco", None, None, \
		"gpio0[27]"])
P8[18] = _set("GPIO2_1", offset=0x08c, modes=["gpmc_clk_mux0", \
		"lcd_memory_clk", "gpmc_wait1", "mmc2_clk", None, None, \
		"mcasp0_fsr", "gpio2[1]"])
P8[19] = _set("EHRPWM2A", offset=0x020, modes=["gpmc_ad8", "lcd_data23", \
		"mmc1_dat0", "mmc2_dat4", "ehrpwm2A", None, None, \
		"gpio0[22]"])
P8[20] = _set("GPIO1_31", offset=0x084, modes={0: "gpmc_csn2", \
		1: "gpmc_be1n", 2: "mmc1_cmd", 5: "pr1_pru1_pru_r30_13", \
		6: "pr1_pru1_pru_r31_13", 7: "gpio1[31]"})
P8[21] = _set("GPIO1_30", offset=0x080, modes={0: "gpmc_csn1", \
		1: "gpmc_clk", 2: "mmc1_clk", 5: "pr1_pru1_pru_r30_12", \
		6: "pr1_pru1_pru_r31_12", 7: "gpio1[30]"})
P8[22] = _set("GPIO1_5", offset=0x014, modes={0: "gpmc_ad5", \
		1: "mmc1_dat5", 7: "gpio1[5]"})
P8[23] = _set("GPIO1_4", offset=0x010, modes={0: "gpmc_ad4", \
		1: "mmc1_dat4", 7: "gpio1[4]"})
P8[24] = _set("GPIO1_1", offset=0x004, modes={0: "gpmc_ad1", \
		1: "mmc1_dat1", 7: "gpio1[1]"})
P8[25] = _set("GPIO1_0", offset=0x000, modes={0: "gpmc_ad0", \
		1: "mmc1_dat0", 7: "gpio1[0]"})
P8[26] = _set("GPIO1_29", offset=0x07c, modes={0: "gpmc_csn0", 7: "gpio1[29]"})
P8[27] = _set("GPIO2_22", offset=0x0e0, modes={0: "lcd_vsync", \
		1: "gpmc_a8", 5: "pr1_pru1_pru_r30_8", \
		6: "pr1_pru1_pru_r31_8", 7: "gpio2[22]"})
P8[28] = _set("GPIO2_24", offset=0x0e8, modes={0: "lcd_pclk", \
		1: "gpmc_a10", 5: "pr1_pru1_pru_r30_10", \
		6: "pr1_pru1_pru_r31_10", 7: "gpio2[24]"})
P8[29] = _set("GPIO2_23", offset=0x0e4, modes={0: "lcd_hsync", \
		1: "gpmc_a9", 5: "pr1_pru1_pru_r30_9", \
		6: "pr1_pru1_pru_r31_9", 7: "gpio2[23]"})
P8[30] = _set("GPIO2_25", offset=0x0ec, modes={0: "lcd_ac_bias_en", \
		1: "gpmc_a11", 7: "gpio2[25]"})
P8[31] = _set("UART5_CTSN", offset=0x0d8, modes=["lcd_data14", "gpmc_a18", \
		"eQEP1_index", "mcasp0_axr1", "uart5_rxd", None, \
		"uart5_ctsn", "gpio0[10]"])
P8[32] = _set("UART5_RTSN", offset=0x0dc, modes=["lcd_data15", "gpmc_a19", \
		"eQEP1_strobe", "mcasp0_ahclkx", "mcasp0_axr3", None, \
		"uart5_rtsn", "gpio0[11]"])
P8[33] = _set("UART4_RTSN", offset=0x0d4, modes=["lcd_data13", "gpmc_a17", \
		"eQEP1B_in", "mcasp0_fsr", "mcasp0_axr3", None, "uart4_rtsn", \
		"gpio0[9]"])
P8[34] = _set("UART3_RTSN", offset=0x0cc, modes=["lcd_data11", "gpmc_a15", \
		"ehrpwm1B", "mcasp0_ahclkr", "mcasp0_axr2", None, \
		"uart3_rtsn", "gpio2[17]"])
P8[35] = _set("UART4_CTSN", offset=0x0d0, modes=["lcd_data12", "gpmc_a16", \
		"eQEP1A_in", "mcasp0_aclkr", "mcasp0_axr2", None, \
		"uart4_ctsn", "gpio0[8]"])
P8[36] = _set("UART3_CTSN", offset=0x0c8, modes=["lcd_data10", "gpmc_a14", \
		"ehrpwm1A", "mcasp0_axr0", None, None, "uart3_ctsn", \
		"gpio2[16]"])
P8[37] = _set("UART5_TXD", offset=0x0c0, modes=["lcd_data8", "gpmc_a12", \
		"ehrpwm1_tripzone_in", "mcasp0_aclkx", "uart5_txd", \
		"None", "uart2_ctsn", "gpio2[14]"])
P8[38] = _set("UART5_RXD", offset=0x0c4, modes=["lcd_data9", "gpmc_a13", \
		"ehrpwm0_synco", "mcasp0_fsx", "uart5_rxd", None, \
		"uart2_rtsn", "gpio2[15]"])
P8[39] = _set("GPIO2_12", offset=0x0b8, modes=["lcd_data6", "gpmc_a6", None, \
		"eQEP2_index", None, "pr1_pru1_pru_r30_6", \
		"pr1_pru1_pru_r31_6", "gpio2[12]"])
P8[40] = _set("GPIO2_13", offset=0x0bc, modes=["lcd_data7", "gpmc_a7", None, \
		"eQEP2_strobe", "pr1_edio_data_out7", "pr1_pru1_pru_r30_7", \
		"pr1_pru1_pru_r31_7", "gpio2[13]"])
P8[41] = _set("GPIO2_10", offset=0x0b0, modes=["lcd_data4", "gpmc_a4", None, \
		"eQEP2A_in", None, "pr1_pru1_pru_r30_4", "pr1_pru1_pru_r30_4", \
		"gpio2[10]"])
P8[42] = _set("GPIO2_11", offset=0x0b4, modes=["lcd_data5", "gpmc_a5", None, \
		"eQEP2B_in", None, "pr1_pru1_pru_r30_5", "pr1_pru1_pru_r31_5", \
		"gpio2[11]"])
P8[43] = _set("GPIO2_8", offset=0x0a8, modes=["lcd_data2", "lcd_data2", None, \
		"ehrpwm2_tripzone_in", None, "pr1_pru1_pru_r30_2", \
		"pr1_pru1_pru_r31_2", "gpio2[8]"])
P8[44] = _set("GPIO2_9", offset=0x0ac, modes=["lcd_data3", "gpmc_a3", None, \
		"ehrpwm0_synco", None, "pr1_pru1_pru_r30_3", \
		"pr1_pru1_pru_r31_3", "gpio2[9]"])
P8[45] = _set("GPIO2_6", offset=0x0a0, modes=["lcd_data0", "gpmc_a0", None, \
		"ehrpwm2A", None, "pr1_pru1_pru_r30_0", "pr1_pru1_pru_r31_0", \
		"gpio2[6]"])
P8[46] = _set("GPIO2_7", offset=0x0a4, modes=["lcd_data1", "gpmc_a1", None, \
		"ehrpwm2B", None, "pr1_pru1_pru_r30_1", "pr1_pru1_pru_r31_1", \
		"gpio2[7]"])
