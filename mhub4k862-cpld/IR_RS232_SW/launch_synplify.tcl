#-- Lattice Semiconductor Corporation Ltd.
#-- Synplify OEM project file E:/CPLD/MUH4K862/MUH88E(4K862)/IR_RS232_SW/launch_synplify.tcl
#-- Written on Tue Aug 23 09:57:43 2016

project -close
set filename "E:/CPLD/MUH4K862/MUH88E(4K862)/IR_RS232_SW/IR_RS232_SW_syn.prj"
if ([file exists "$filename"]) {
	project -load "$filename"
	project_file -remove *
	} else {
	project -new "$filename"
}

#device options
set_option -technology MACHXO2
set_option -part LCMXO2_1200HC
set_option -package TG144I
set_option -speed_grade -4

#-- add_file options
set_option -include_path "E:/CPLD/MUH4K862/MUH88E(4K862)"
add_file -verilog "D:/YangProgram/lscc/diamond/2.0/cae_library/synthesis/verilog/machxo2.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/IR_RS232_SW.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/HC74595.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/ir_rx.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/my_uart_top.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/my_uart_tx.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/speed_select.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/fifo64x8.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/ir_car_detect.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/check_IR_defv.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/uart_rx.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/uart_tx.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/pc2mcu_uart_top.v"
add_file -verilog "E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/mcu2pc_uart_top.v"
project -result_file {E:/CPLD/MUH4K862/MUH88E(4K862)/IR_RS232_SW/IR_RS232_SW.edi}
project -save "$filename"
