/dts-v1/;
/plugin/;

/ {
	compatible = "ti,beaglebone", "ti,beaglebone-green", "ti,beaglebone-black";
	part-number = "{{part_num}}";
	version = "{{version}}";

	%if pins:
	exclusive-use =
		"{{pins[0]['header']}}.{{pins[0]['pin_num']}}", \\
	%for pin in pins[1:]:
"{{pin['header']}}.{{pin['pin_num']}}", \\
	%end
"pru0", "pru1";

	fragment@0 {
		target = <&am33xx_pinmux>;
		__overlay__ {
			example_pins: pinmux_pru_pru_pins {
				pinctrl-single,pins = <
				%for pin in pins:
					{{hex(pin['offset'])}} {{hex(pin['bits'])}}
				%end
				>;
			};
		};
	};
	%end

	fragment@1 {
		target = <&pruss>;
		__overlay__ {
			status = "{{status}}";
			%if pins:
			pinctrl-names = "default";
			pinctrl-0 = <&example_pins>;
			%end
		};
	};
};
