#-- Lattice Semiconductor Corporation Ltd.
#-- Synplify OEM project file

#device options
set_option -technology MACHXO2
set_option -part LCMXO2_1200HC
set_option -package TG144I
set_option -speed_grade -4

#compilation/mapping options
set_option -symbolic_fsm_compiler true
set_option -resource_sharing true

#use verilog 2001 standard option
set_option -vlog_std v2001

#map options
set_option -frequency 200
set_option -fanout_limit 100
set_option -auto_constrain_io true
set_option -disable_io_insertion false
set_option -retiming false; set_option -pipe false
set_option -force_gsr false
set_option -compiler_compatible true
set_option -dup false
set_option -frequency 1
set_option -default_enum_encoding default

#simulation options


#timing analysis options
set_option -num_critical_paths 3
set_option -num_startend_points 0

#automatic place and route (vendor) options
set_option -write_apr_constraint 0

#synplifyPro options
set_option -fixgatedclocks 3
set_option -fixgeneratedclocks 3
set_option -update_models_cp 0
set_option -resolve_multiple_driver 1

#-- add_file options
set_option -include_path {E:/CPLD/MUH4K862/MUH88E(4K862)}
add_file -verilog {D:/YangProgram/lscc/diamond/2.0/cae_library/synthesis/verilog/machxo2.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/IR_RS232_SW.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/HC74595.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/ir_rx.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/my_uart_top.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/my_uart_tx.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/speed_select.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/fifo64x8.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/ir_car_detect.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/check_IR_defv.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/uart_rx.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/pc2mcu_uart_top.v}
add_file -verilog {E:/CPLD/MUH4K862/MUH88E(4K862)/new_uart/mcu2pc_uart_top.v}

#-- top module name
set_option -top_module IR_RS232_SW

#-- set result format/file last
project -result_file {E:/CPLD/MUH4K862/MUH88E(4K862)/IR_RS232_SW/IR_RS232_SW_IR_RS232_SW.edi}

#-- error message log file
project -log_file {IR_RS232_SW_IR_RS232_SW.srf}

#-- set any command lines input by customer


#-- run Synplify with 'arrange HDL file'
project -run
