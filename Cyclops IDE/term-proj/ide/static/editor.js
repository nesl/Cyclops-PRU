$(document).ready(function() {
	var editor = ace.edit("editor");
	editor.$blockScrolling = Infinity;
	editor.setTheme("ace/theme/solarized_light");
	editor.session.setMode("ace/mode/cyclops");

	var asm_editor = ace.edit("asm-editor");
	asm_editor.$blockScrolling = Infinity;
	asm_editor.setTheme("ace/theme/dawn");
	asm_editor.session.setMode("ace/mode/assembly_pru");

	compiler_init();

	$('#compile').click(function() {
		var terminal = $("#console");
		terminal.append("Compiling...\n");
		try {
			var asm_src = compile(editor.getValue());
			asm_editor.setValue(asm_src, -1);
			terminal.append("Compile Done\n");
		} catch (e) {
			terminal.append("Compile Error: " + e.message + "\n");
		}
		$('#tail').click();
	});

	$('#run').click(function() {
		var pru_num = $('#run').attr('pru-num');
		var src = asm_editor.getValue();
		$.post("pru/" + pru_num + "/run", { "src": src }, function(data) {
			console.log("run");
			var terminal = $("#console");
			terminal.append("Running...\n");
			$('#tail').click();
		});
	});

	$('#stop').click(function() {
		var pru_num = $('#stop').attr('pru-num');
		$.ajax({
			url: "pru/" + pru_num + "/stop", 
			type: "PUT"
		});
	});

	$('#clear').click(function() {
		$('#console').text("");
	});

	$('#tail').click(function() {
		var terminal = $("#console");
		var height = terminal[0].scrollHeight;
		terminal.scrollTop(height);
	});

	function update_terminal() {
		var pru_num = $('#run').attr('pru-num');
		$.get("pru/" + pru_num + "/out", function(data) {
			if (data) {
				var terminal = $("#console");
				terminal.append(data);
				var height = terminal[0].scrollHeight;
				terminal.scrollTop(height);
				console.log(data);
			}
			window.setTimeout(update_terminal, 100);
		}).fail(function() { window.setTimeout(update_terminal, 5000); });
	}

	window.setTimeout(update_terminal, 100);
});
