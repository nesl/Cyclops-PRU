<!DOCTYPE html>
<html>
<head>
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta charset="utf-8">
	<title>{{title or 'PRU'}}</title>
	<meta name="description" content="Beaglebone black PRU">
	<meta name="author" content="Yi-Fan Zhang">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="static/editor.css">
	<link rel="shortcut icon" href="">
	<style>

	</style>
</head>
<body>
	<div class="menu-bar">
		<button id="compile" style="float: left; margin-left: 3px;"class="button" pru-num="{{pru_num}}">Compile</button>
		<button id="run" style="float: left;"class="button" pru-num="{{pru_num}}">Run</button>
		<button id="stop" style="float: left;"class="button" pru-num="{{pru_num}}">Stop</button>
		<button id="tail" style="float: right; margin-right: 3px;" class="button" pru-num="{{pru_num}}">Tail</button>
		<button id="clear" style="float: right;" class="button" pru-num="{{pru_num}}">Clear</button>
	</div>
	<div class="body-container">
		{{!base}}
	</div>
	% include('grammar.tpl')
	<script src="static/jquery.js" type="text/javascript"></script>
	<script src="static/ace/ace.js" type="text/javascript"></script>
	<script src="static/peg.js" type="text/javascript"></script>
	<script src="static/compiler.js" type="text/javascript"></script>
	<script src="static/editor.js" type="text/javascript"></script>
</body>
</html>
