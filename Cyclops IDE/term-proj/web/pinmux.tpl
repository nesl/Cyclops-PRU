% rebase('base.tpl', title='PRU Pin Configuration')
<form autocomplete="off" action="/pru/pinmux" method="post">
<table>
	<tr class="
	%if pruss_enabled:
		pru-assigned
	%end
	">
		<td><label for="enable_pruss">PRU Subsystem Enabled:</label></td>
		<td>
			<input id="enable_pruss" name="enable_pruss" type="checkbox"
			%if pruss_enabled:
				class="pru_assigned"
				checked
			%end
			>
		</td>
		<td><input type="submit" value="Apply"></td>
	</tr>
</table>
<table>
<caption>P8</caption>
<%for i in range(1, 46, 2):
	l = P8[i]
	r = P8[i+1]
	%>

	<tr>
		%include('pinmux-pin.tpl', pin=l, pin_num="P8_" + str(i), reverse=True)
		<td>{{i}}</td>
		<td>{{i+1}}</td>
		%include('pinmux-pin.tpl', pin=r, pin_num="P8_" + str(i+1), reverse=False)
	</tr>
%end
</table>
</form>
