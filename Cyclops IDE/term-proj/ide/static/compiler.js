var code_boilerplate_begin =
".origin 0\n" +
".entrypoint TOP\n" +
"\n" +
"TOP:\n" +
"\n" +
"// use r0 as a constant zero register\n" +
"mov r0, 0\n" +
"\n" +
"// Control register base address\n" +
"mov r2, 0x22000\n" +
"// Read in control register\n" +
"lbbo r3, r2, 0, 1\n" +
"// Turn on bit 3 to enable counters\n" +
"set r3.t3\n" +
"// Write to control register\n" +
"sbbo r3, r2, 0, 1\n" +
"// reset cycle counter\n" +
"sbbo r0, r2, 0xC, 4\n" +
"// stack pointer in r1\n" +
"ldi r1, 1024\n\n";

var code_boilerplate_end =
"END:\n" +
"// Interrupt the host process\n" +
"ldi r2, 2\n" +
"sbbo r2, r0, r0, 4\n" +
"halt\n\n";

var code_print =
"PRINT:\n" +
"sub r1, r1, 4\n" +
"// num args in r2\n" +
"lbbo r2, r1, 0, 4\n" +
"// write offset\n" +
"ldi r4, 0\n" +
"// print status code = 1\n" +
"ldi r3, 1\n" +
"sbbo r3, r4, 0, 4\n" +
"add r4, r4, 4\n" +
"sbbo r2, r4, 0, 4\n" +
"add r4, r4, 4\n" +
"PRINT_LOOP:\n" +
"sub r1, r1, 4\n" +
"lbbo r3, r1, 0, 4\n" +
"sbbo r3, r4, 0, 4\n" +
"add r4, r4, 4\n" +
"sub r2, r2, 1\n" +
"qbne PRINT_LOOP, r2, 0\n" +
"PRINT_WAIT:\n" +
"lbbo r2, r0, 0, 4\n" +
"qbne PRINT_WAIT, r2, 0\n" +
"ret\n";

/* Example call to print

ldi r5, 321
sbbo r5, r1, 0, 4
add r1, r1, 4
ldi r5, 1
sbbo r5, r1, 0, 4
add r1, r1, 4
call PRINT

*/

var parser;

function compiler_init() {
	parser = peg.generate($("#grammar").text());	
}

function compile(src) {
	var asm_src = "";

	parser.parse(src);

	var pin_configs = "";
	for (var key in window.pin_settings) {
		if (!window.pin_settings.hasOwnProperty(key)) continue;

		var pin_conf = window.pin_settings[key];
		pin_configs += "//" + pin_conf.conf + "\n"
	}	

	asm_src += pin_configs
	asm_src += code_boilerplate_begin;

	asm_src += "// START\n\n";
	asm_src += window.asm_out_buf;
	asm_src += "\n// END\n\n";

	asm_src += code_boilerplate_end;	
	asm_src += code_print;

	window.asm_out_buf = "";
	return asm_src;
}
