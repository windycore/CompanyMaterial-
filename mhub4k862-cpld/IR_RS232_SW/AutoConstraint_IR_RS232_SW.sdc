
#Begin clock constraint
define_clock -name {IR_RS232_SW|clk} {p:IR_RS232_SW|clk} -period 5.954 -clockgroup Autoconstr_clkgroup_0 -rise 0.000 -fall 2.977 -route 0.000 
#End clock constraint

#Begin clock constraint
define_clock -name {IR_RS232_SW|rclk} {p:IR_RS232_SW|rclk} -period 10000000.000 -clockgroup Autoconstr_clkgroup_1 -rise 0.000 -fall 5000000.000 -route 0.000 
#End clock constraint

#Begin clock constraint
define_clock -name {IR_RS232_SW|srclk} {p:IR_RS232_SW|srclk} -period 1.000 -clockgroup Autoconstr_clkgroup_2 -rise 0.000 -fall 0.500 -route 0.000 
#End clock constraint
