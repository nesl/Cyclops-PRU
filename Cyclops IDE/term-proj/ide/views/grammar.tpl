<pre id="grammar" style="display: none;">
{
	var reg_var_map = {};
	var base_tmp_reg = 25;
	var tmp_reg = base_tmp_reg;
	var base_free_reg = 5;
	var free_reg = base_free_reg;
	var loop_cnt = 0;
	
	// 14 pins for each PRU
	var pin_map = {
		"P8_11": { /* no input */                           output: { reg: 30, bit: 15, mode: 0x06 }, offset: 0x034 }, // PRU0
		"P8_12": { /* no input */                           output: { reg: 30, bit: 14, mode: 0x06 }, offset: 0x030 }, // PRU0
		"P8_15": { input: { reg: 31, bit: 15, mode: 0x06 }, /* no output */                           offset: 0x03c }, // PRU0
		"P8_16": { input: { reg: 31, bit: 14, mode: 0x06 }, /* no output */                           offset: 0x038 }, // PRU0
		"P9_24": { input: { reg: 31, bit: 16, mode: 0x06 }, /* no output */                           offset: 0x184 }, // PRU0 
		"P9_25": { input: { reg: 31, bit:  7, mode: 0x06 }, output: { reg: 30, bit:  7, mode: 0x05 }, offset: 0x1ac }, // PRU0 
		"P9_27": { input: { reg: 31, bit:  5, mode: 0x06 }, output: { reg: 30, bit:  5, mode: 0x05 }, offset: 0x1a4 }, // PRU0 
		"P9_28": { input: { reg: 31, bit:  3, mode: 0x06 }, output: { reg: 30, bit:  3, mode: 0x05 }, offset: 0x19c }, // PRU0 
		"P9_29": { input: { reg: 31, bit:  1, mode: 0x06 }, output: { reg: 30, bit:  1, mode: 0x05 }, offset: 0x194 }, // PRU0 
		"P9_30": { input: { reg: 31, bit:  2, mode: 0x06 }, output: { reg: 30, bit:  2, mode: 0x05 }, offset: 0x198 }, // PRU0 
		"P9_31": { input: { reg: 31, bit:  0, mode: 0x06 }, output: { reg: 30, bit:  0, mode: 0x05 }, offset: 0x190 }, // PRU0 
		// Not sure how these pins are suppose to be configured
		// "P9_41": { }, // PRU 0
		// "P9_42": { }, // PRU 0

		"P8_20": { input: { reg: 31, bit: 13, mode: 0x06 }, output: { reg: 30, bit: 13, mode: 0x05 }, offset: 0x084 }, // PRU1
		"P8_21": { input: { reg: 31, bit: 12, mode: 0x06 }, output: { reg: 30, bit: 12, mode: 0x05 }, offset: 0x080 }, // PRU1
		"P8_27": { input: { reg: 31, bit:  8, mode: 0x06 }, output: { reg: 30, bit:  8, mode: 0x05 }, offset: 0x0e0 }, // PRU1
		"P8_28": { input: { reg: 31, bit: 10, mode: 0x06 }, output: { reg: 30, bit: 10, mode: 0x05 }, offset: 0x0e8 }, // PRU1
		"P8_29": { input: { reg: 31, bit:  9, mode: 0x06 }, output: { reg: 30, bit:  9, mode: 0x05 }, offset: 0x0e4 }, // PRU1
		"P8_30": { input: { reg: 31, bit: 11, mode: 0x06 }, output: { reg: 30, bit: 11, mode: 0x05 }, offset: 0x0ec }, // PRU1
		"P8_39": { input: { reg: 31, bit:  6, mode: 0x06 }, output: { reg: 30, bit:  6, mode: 0x05 }, offset: 0xb80 }, // PRU1
		"P8_40": { input: { reg: 31, bit:  7, mode: 0x06 }, output: { reg: 30, bit:  7, mode: 0x05 }, offset: 0xbc0 }, // PRU1
		"P8_41": { input: { reg: 31, bit:  4, mode: 0x06 }, output: { reg: 30, bit:  4, mode: 0x05 }, offset: 0xb00 }, // PRU1
		"P8_42": { input: { reg: 31, bit:  5, mode: 0x06 }, output: { reg: 30, bit:  5, mode: 0x05 }, offset: 0xb40 }, // PRU1
		"P8_43": { input: { reg: 31, bit:  2, mode: 0x06 }, output: { reg: 30, bit:  2, mode: 0x05 }, offset: 0xa80 }, // PRU1
		"P8_44": { input: { reg: 31, bit:  3, mode: 0x06 }, output: { reg: 30, bit:  3, mode: 0x05 }, offset: 0xac0 }, // PRU1
		"P8_45": { input: { reg: 31, bit:  0, mode: 0x06 }, output: { reg: 30, bit:  0, mode: 0x05 }, offset: 0xa00 }, // PRU1
		"P8_46": { input: { reg: 31, bit:  1, mode: 0x06 }, output: { reg: 30, bit:  1, mode: 0x05 }, offset: 0xa40 }, // PRU1
		"P9_26": { input: { reg: 31, bit: 16, mode: 0x06 }, /* no output */                           offset: 0x180 }, // PRU1
	};

	var pin_enable_pull_bit = (1 << 3);
	var pin_pullup_bit = (1 << 4);
	var pin_input_bit = (1 << 5);
	var pin_settings = {};

	window.asm_out_buf = "";
	window.pin_settings = pin_settings;

	function asm_out(str) {
		window.asm_out_buf += str + "\n";
	}

	function assign_reg(symbol) {
		var reg = reg_var_map[symbol];

		if (!reg) {
			var num = free_reg++;
			if (num >= base_tmp_reg) throw { message: "out of registers" };
			reg_var_map[symbol] = num;
		}

		return "r" + reg_var_map[symbol];
	}

	function assign_tmp_reg() {
		var num = tmp_reg++;
		if (num > 29) throw { message: "out of tmp registers" };
		return "r" + num;
	}

	function assign_tmp_reg_imm() {
		var num = tmp_reg;
		if (num > 29) throw { message: "out of tmp registers" };
		return "r" + num;
	}

	function reset_tmp_regs() {
		tmp_reg = base_tmp_reg;
	}

	function get_pin_info(pin) {
		var pin_info = pin_map[pin];
		if (!pin_info) throw { message: "invalid pin " + pin};
		return pin_info;
	}

	function assign_pin(pin, input) {
		var pin_conf = pin_settings[pin];
		if (pin_conf && pin_conf.input !== input) {
			throw { message: "" + pin + " cannot be used for input and output simultaneously" };	
		}
		if (pin_conf && pin_conf.input === input) return;

		var pin_info = pin_map[pin];
		if (!pin_info) throw { message: "" + pin + " does not exist" };

		var conf = "0x" + Number(pin_info.offset).toString(16);
		if (input) {
			conf += " 0x" + Number(pin_info.input.mode | pin_enable_pull_bit | pin_input_bit).toString(16);
		} else {
			conf += " 0x" + Number(pin_info.output.mode).toString(16);
		}

		pin_settings[pin] = { input: input, conf: conf };	
	}
}


Root
	= Statement*

Statement
	= _ Assignment _ ";" _
	/ _ Loop _
	/ _ ifpart:If elsepart:(_ "{" _ Statement* _ "}")? _ {
		if (elsepart) {
			asm_out("" + ifpart.value + "_ELSE_END:");
		}
	}
	/ _ Print _ ";" _
	/ _ Pulse _ ";" _

Assignment
	= _ lhs:(Pin / Tick / Identifier) _ "=" _ rhs:Expression {
		if (lhs.type === 'pin') {
			if (rhs.type === 'int') {
				var pin_info = get_pin_info(lhs.value);
				if (!pin_info.output) throw { message: lhs.value + " cannot be used for output" };
				var preg = "r" + pin_info.output.reg;
				var pbit = pin_info.output.bit;
				if (rhs.value === 0) {
					asm_out("clr " + preg + ", " + preg + ", " + pbit);
				} else {
					asm_out("set " + preg + ", " + preg + ", " + pbit);
				}
				assign_pin(lhs.value, false);
			} else {
				throw { message: "pin must be set with an integer literal" };
			}
		} else if (lhs.type === 'tick') {
			var treg = assign_tmp_reg();
			asm_out("mov " + treg + ", 0x22000");
			if (rhs.type === 'int') {
				var treg2 = assign_tmp_reg();
				asm_out("mov " + treg2 + ", " + rhs.value);
				asm_out("sbbo " + treg2 + ", " + treg + ", 0xC, 4");
				tmp_reg--;
			} else {
				asm_out("sbbo " + rhs.value + ", " + treg + ", 0xC, 4");
			}
			tmp_reg--;
		} else {
			if (rhs.type !== 'int') {
				asm_out("mov " + lhs.value + ", " + rhs.value);
			} else {
				asm_out("mov " + lhs.value + ", " + rhs.value);
			}
		}
		reset_tmp_regs();
		return null;
	}

Print
	= "print" _ "(" _ head:Expression tail:(_ "," _ Expression)* _ ")" {
		var treg = assign_tmp_reg_imm();
		for (var i = tail.length - 1; i >= 0; i--) {
			var arg = tail[i][3];
			if (arg.type === 'int') {
				asm_out("mov " + treg + ", " + arg.value);
				asm_out("sbbo " + treg + ", r1, 0, 4");
			} else {
				asm_out("sbbo " + arg.value + ", r1, 0, 4");
			}
			asm_out("add r1, r1, 4");
		}

		arg = head;
		if (arg.type === 'int') {
			asm_out("mov " + treg + ", " + arg.value);
			asm_out("sbbo " + treg + ", r1, 0, 4");
		} else {
			asm_out("sbbo " + arg.value + ", r1, 0, 4");
		}
		asm_out("add r1, r1, 4");

		var argc = 1 + tail.length;
		asm_out("mov " + treg + ", " + argc);
		asm_out("sbbo " + treg + ", r1, 0, 4");
		asm_out("add r1, r1, 4");
		asm_out("call PRINT");
	}

Pulse
	= "pulse" _ "(" _ pin:Pin _ "," _ duration:Integer _ unit:("sec"/"ms"/"us")? _ ")" {
		var pin_info = get_pin_info(pin.value);
		if (!pin_info.output) throw { message: pin.value + " cannot be used for output" };
		var preg = "r" + pin_info.output.reg;
		var pbit = pin_info.output.bit;

		var num_cycles = duration.value;
		if (unit === "sec") {
			num_cycles *= 200000000;
		} else if (unit === "ms") {
			num_cycles *= 200000;
		} else if (unit === "us") {
			num_cycles *= 200;
		}

		asm_out("set " + preg + ", " + preg + ", " + pbit);
		var loop_label = "COND_" + loop_cnt++;
		asm_out(loop_label + ":");
		var treg = assign_tmp_reg();
		var treg2 = assign_tmp_reg();
		asm_out("mov " + treg + ", 0x22000");
		asm_out("lbbo " + treg + ", " + treg + ", 0xC, 4");
		asm_out("mov " + treg2 + ", " + num_cycles);
		asm_out("qbge " + loop_label + "_END, " + treg2 + ", " + treg);
		asm_out("jmp " + loop_label);
		asm_out(loop_label + "_END:");
		asm_out("clr " + preg + ", " + preg + ", " + pbit);

		tmp_reg -= 2;
		assign_pin(pin.value, false);
	}

Loop
	= while_keyword:While _ "(" _ cond:CondExpression _ ")" _ "{" _ Statement* _ "}" {
		asm_out("jmp " + while_keyword.value);
		asm_out("" + while_keyword.value + "_END:");
	}

While
	= "while" {
		var label = "COND_" + loop_cnt;
		var label_end = label + "_END";

		asm_out(label + ":");

		return {
			type: 'label',
			value: label
		};
	}

If
	= ifkey:If_key _ "(" _ cond:CondExpression _ ")" _ "{" _ Statement* _ "}" _ elsekey:("else"?) {
		if (elsekey === "else") {
			asm_out("jmp " + ifkey.value + "_ELSE_END");
			asm_out("" + ifkey.value + "_END:");
		} else {
			asm_out("" + ifkey.value + "_END:");
		}

		return {
			type: 'label',
			value: ifkey.value
		};
	}

If_key
	= "if" {
		var label = "COND_" + loop_cnt;
		var label_end = label + "_END";

		asm_out(label + ":");

		return {
			type: 'label',
			value: label
		};
	}

CondExpression
	= not:("!"?) _ pin:Pin {
		var opcode = "qbbc ";
		if (not === "!") opcode = "qbbs ";
		
		var label = "COND_" + loop_cnt++;
		var label_end = label + "_END";

		var pin_info = get_pin_info(pin.value);
		if (!pin_info.input) throw { message: pin.value + " cannot be used for input" };
		var preg = "r" + pin_info.input.reg;
		var pbit = pin_info.input.bit;

		asm_out(opcode + label_end + ", " + preg + ", " + pbit);

		assign_pin(pin.value, true);

		return {
			type: 'label',
			value: label
		};
	}
	/ lhs:Expression _ op:(">=" / "<=" / "==" / ">" / "<") _ rhs:Expression {
		var opcode;
		if (op == ">") opcode = "qble ";
		else if (op == "<") opcode = "qbge ";
		else if (op == ">=") opcode = "qblt ";
		else if (op == "<=") opcode = "qbgt ";
		else if (op == "==") opcode = "qbne ";
		
		var label = "COND_" + loop_cnt++;
		var label_end = label + "_END";

		var tmpcnt = 0;
		if (lhs.type === 'int') {
			var treg = assign_tmp_reg();
			asm_out("mov " + treg + ", " + lhs.value);
			lhs = { type: 'reg', value: treg };
			tmpcnt++;
		}

		if (rhs.type === 'int') {
			treg = assign_tmp_reg();
			asm_out("mov " + treg + ", " + rhs.value);
			rhs = { type: 'reg', value: treg };
			tmpcnt++;
		}

		tmp_reg -= tmpcnt;

		asm_out(opcode + label_end + ", " + rhs.value + ", " + lhs.value);

		return {
			type: 'label',
			value: label
		};
	}

Expression
	= head:Term tail:(_ ("&" / "|") _ Term)* {
		var lhs = head;

		if (tail.length == 0) {
			return lhs;
		}

		var treg = assign_tmp_reg();


		for (var i = 0; i < tail.length; i++) {
			var opcode;

			if (tail[i][1] === "&") opcode = "and ";
			if (tail[i][1] === "|") opcode = "or ";

			var rhs = tail[i][3];

			if (lhs.type === 'int' && rhs.type === 'int') {
				var result;
				if (opcode === "and ") result = lhs.value & rhs.value;
				else result = lhs.value | rhs.value;
				asm_out("mov " + treg + ", " + result);
			} else if (lhs.type === 'int' && rhs.type !== 'int') {
				var head_tmp = assign_tmp_reg_imm();
				asm_out("mov " + head_tmp + ", " + lhs.value);
				asm_out(opcode + treg + ", " + head_tmp + ", " + rhs.value);
			} else {
				asm_out(opcode + treg + ", " + lhs.value + ", " + rhs.value);
			}

			lhs = { type: 'reg', value: treg };
		}

		return {
			type: "reg",
			value: treg
		};
	}

Term
	= head:Factor tail:(_ ("+" / "-") _ Factor)* {
		var lhs = head;

		if (tail.length == 0) {
			return lhs;
		}

		var treg = assign_tmp_reg();

		for (var i = 0; i < tail.length; i++) {
			var opcode;

			if (tail[i][1] === "+") opcode = "add ";
			if (tail[i][1] === "-") opcode = "sub ";

			var rhs = tail[i][3];

			if (lhs.type === 'int' && rhs.type === 'int') {
				var result;
				if (opcode === "add ") result = lhs.value + rhs.value;
				else result = lhs.value - rhs.value;
				asm_out("mov " + treg + ", " + result);
			} else if (lhs.type === 'int' && rhs.type !== 'int') {
				var head_tmp = assign_tmp_reg_imm();
				asm_out("mov " + head_tmp + ", " + lhs.value);
				asm_out(opcode + treg + ", " + head_tmp + ", " + rhs.value);
			} else {
				asm_out(opcode + treg + ", " + lhs.value + ", " + rhs.value);
			}

			lhs = { type: 'reg', value: treg };
		}

		return {
			type: "reg",
			value: treg
		};
	}

Factor
	= "(" _ expr:Expression _ ")" {
		return expr;
	}
	/ Integer
	/ Tick {
		var treg = assign_tmp_reg();
		asm_out("mov " + treg + ", 0x22000");
		asm_out("lbbo " + treg + ", " + treg + ", 0xC, 4"); 
		return {
			type: 'reg',
			value: treg
		}
	}
	/ Identifier

Integer "integer"
	= "0x" [0-9a-fA-F]+ {
		return {
			type: 'int',
			value: parseInt(text(), 16)
		};
	}
	/ [0-9]+ { 
		return {
			type: "int",
			value: parseInt(text(), 10)
		};
	}

Pin
	= "P" header:("8" / "9") "_" num:([1-9][0-9]*) {
		return {
			type: 'pin',
			value: text()
		}
	}

Tick
	= "tick" {
		return {
			type: 'tick'
		}
	}

Identifier
	= ([_a-zA-Z][_a-zA-Z0-9]*) {
		return {
			type: "reg",
			value: assign_reg(text())
		};
	}

_ "whitespace"
	= ([ \t\n\r] / Comments / Comment)*

Comments
	= "/*" (!"*/" .)* "*/"

Comment 
	= "//" (!([\n\r]) .)* 
</pre>
