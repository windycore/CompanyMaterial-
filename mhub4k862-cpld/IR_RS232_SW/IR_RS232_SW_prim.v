// Verilog netlist produced by program LSE :  version Diamond Version 2.0.0.154
// Netlist written on Thu Mar 09 19:53:05 2017

//
// Verilog Description of module IR_RS232_SW
//

module IR_RS232_SW (clk, rstn, rclk, srclk, ser, ir_eye, ir_front, 
            ir_all_in, ir_all_out, irx_i, irx_o, ir_402x_i, ir_402x_o, 
            rs232_402x_tx, rs232_402x_rx, rs232_db9_tx, rs232_db9_rx, 
            rs232_tcp_tx, rs232_tcp_rx, rs232_mcu_tx, rs232_mcu_rx, 
            ir2mcu, ack2mcu) /* synthesis syn_module_defined=1 */ ;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(6[8:19])
    input clk;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(9[15:18])
    input rstn;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(10[15:19])
    input rclk;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(12[15:19])
    input srclk;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(13[15:20])
    input ser;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(14[15:18])
    input ir_eye;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(16[15:21])
    input ir_front;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(18[15:23])
    input ir_all_in;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(20[15:24])
    output ir_all_out;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(21[15:25])
    input [7:0]irx_i;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(23[21:26])
    output [7:0]irx_o;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(24[21:26])
    input [7:0]ir_402x_i;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(26[21:30])
    output [7:0]ir_402x_o;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(27[21:30])
    input [7:0]rs232_402x_tx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(35[21:34])
    output [7:0]rs232_402x_rx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(36[21:34])
    input rs232_db9_tx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(39[15:27])
    output rs232_db9_rx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(40[15:27])
    input rs232_tcp_tx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(42[15:27])
    output rs232_tcp_rx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(43[15:27])
    input rs232_mcu_tx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(45[15:27])
    output rs232_mcu_rx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(46[15:27])
    output ir2mcu;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(52[15:21])
    output ack2mcu;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(53[15:22])
    
    wire [2:0]tx_baud_adj_282;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(9[15:22])
    wire VCC_net;   // g:/zjf/mhub4k862-cpld/fifo64x8.v(23[10:19])
    wire clk_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(9[15:18])
    wire rstn_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(10[15:19])
    wire rclk_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(12[15:19])
    wire srclk_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(13[15:20])
    wire ser_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(14[15:18])
    wire [0:0]ir_defX_in;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(16[15:21])
    wire ir_front_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(18[15:23])
    wire ir_all_in_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(20[15:24])
    wire ir_all_out_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(21[15:25])
    wire [7:0]irx_i_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(23[21:26])
    wire [7:0]irx_o_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(24[21:26])
    wire [7:0]ir_402x_i_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(26[21:30])
    wire [7:0]rs232_402x_tx_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(35[21:34])
    wire [7:0]rs232_402x_rx_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(36[21:34])
    wire rs232_db9_tx_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(39[15:27])
    wire rs232_db9_rx_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(40[15:27])
    wire rs232_tcp_tx_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(42[15:27])
    wire rs232_tcp_rx_c_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(43[15:27])
    wire rs232_mcu_rx_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(46[15:27])
    wire [0:0]n111;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(52[15:21])
    wire ack2mcu_c;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(53[15:22])
    wire [7:0]con;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(61[12:15])
    wire rclk_r0;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(78[5:12])
    wire rclk_r1;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(79[5:12])
    wire rclk_r2;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(80[5:12])
    wire rclk_r3;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(81[5:12])
    wire rclk_r;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(82[5:11])
    wire txen;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(113[12:16])
    wire [7:0]txdata;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(114[12:18])
    wire rs232tx;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(115[12:19])
    wire rdy;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(116[12:15])
    wire empty;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(117[12:17])
    wire full;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(118[12:16])
    wire rden;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(119[12:16])
    wire wren_w;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(122[7:29])
    wire [10:0]wd_disable_402_to_mcu;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(147[12:33])
    wire rx_int0;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(168[6:13])
    wire rx_int1;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(169[6:13])
    wire [7:0]sp_com_data;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(187[11:22])
    wire [7:0]ir_com_o;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(191[11:19])
    wire [7:0]ir_402_com_i;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(196[11:23])
    wire [3:0]chnum;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(200[11:16])
    wire [2:0]bps_sel;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(140[13:20])
    wire wren;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(211[5:9])
    wire getting_com;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(212[5:16])
    wire [15:0]clock_count;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(346[12:23])
    wire [7:0]def_voltag;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(358[12:22])
    wire [7:0]sw0_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(423[11:18])
    wire [7:0]sw1_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(424[11:18])
    wire [7:0]sw2_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(425[11:18])
    wire [7:0]sw3_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(426[11:18])
    wire [7:0]sw4_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(427[11:18])
    wire [7:0]sw5_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(428[11:18])
    wire [7:0]sw6_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(429[11:18])
    wire [7:0]sw7_f2n;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(430[11:18])
    wire [7:0]f2n_irx_level;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(457[12:25])
    wire [7:0]f2n_irx_temp;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(460[12:24])
    wire [2:0]to_db9_baud;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(498[11:22])
    wire ir_car_care;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(504[6:17])
    wire f_to_mcu_en;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(505[9:20])
    wire [9:0]car_cnt;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(533[11:18])
    wire clock_count15_reg0;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(889[5:23])
    wire clock_count15_reg1;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(890[5:23])
    wire [2:0]rxbaud_fifo_tx_db9;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(897[11:29])
    wire db9_402_en;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(899[5:15])
    wire [2:0]rs232_402_all_back;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(912[11:29])
    wire rs232_send_to_mcu_changebaud;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(947[6:34])
    wire n30348;   // g:/zjf/mhub4k862-cpld/fifo64x8.v(15[16:21])
    wire n1175;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(6[9:11])
    wire qc0;   // g:/zjf/mhub4k862-cpld/hc74595.v(33[5:8])
    wire qd0;   // g:/zjf/mhub4k862-cpld/hc74595.v(34[5:8])
    wire qb0;   // g:/zjf/mhub4k862-cpld/hc74595.v(32[5:8])
    wire qa0;   // g:/zjf/mhub4k862-cpld/hc74595.v(31[5:8])
    wire qe0;   // g:/zjf/mhub4k862-cpld/hc74595.v(35[5:8])
    wire ir_car_null;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_74;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_75;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_76;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_77;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_78;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_79;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire ir_car_null_adj_80;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(78[6:17])
    wire [0:0]ir_def_lv;   // g:/zjf/mhub4k862-cpld/ir_def_lv.v(23[19:28])
    wire [3:0]rx_step;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(29[11:18])
    wire [13:0]rx_baud_count_adj_299;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(129[12:25])
    
    wire n13, n30328, n29472, n30352, n121, n14_adj_70, n30349, 
        n16410, n30178, n4152;
    wire [7:0]n17_adj_428;
    
    wire n1829, n30327, n30357;
    wire [7:0]n482;
    
    wire n4150, n4210, n3607, n3, n30345, n14_adj_71;
    wire [7:0]n1381;
    wire [7:0]n1386;
    wire [7:0]n1388;
    
    wire n30177, n30176, n29387, n16793, n29501, n12, n3_adj_72, 
        n30048, n29858, n3_adj_73;
    wire [7:0]n1510;
    
    wire n30351, n30350, n4153, n29392, n4439, n2820, n10_adj_81, 
        n4151, n14_adj_82, n29440, n4438, n30355, n14_adj_83, n12_adj_84, 
        n12_adj_85, n29471, n10_adj_86, n10_adj_87, n30344, n4643, 
        n4154, n29683, n29619, n14_adj_88, n4663, n30166, n4683, 
        n14_adj_89, n30162, n2685, n2683, n12_adj_90, n2681, n2679, 
        n2677, n2675, n2673, n2671, n29390, n4415, n29607, n30155;
    wire [9:0]n46;
    wire [9:0]n58_adj_596;
    wire [15:0]n122;
    wire [15:0]n70_adj_600;
    wire [10:0]n87_adj_605;
    wire [10:0]n50_adj_603;
    
    wire n10_adj_116, n30147, n29110, n30146, n29096, n10_adj_117, 
        n14_adj_118, n29545, n12_adj_119, n30140, n14_adj_120, n10_adj_121, 
        n12_adj_122, n16755, n30134, n10_adj_123, n30132, n29005, 
        n14_adj_124, n12_adj_125, n30131, n10_adj_126, n16703, n10_adj_127, 
        n30130, n30054, n14_adj_128, n12_adj_129, n30053, n29391, 
        n29019, n30046, n4_adj_130, n28870, n28869, n28868, n28867, 
        n28866, n28865, n28864, n28863, n10_adj_131, n28862, n28861, 
        n30044, n28860, n28859, n28858, n28857, n11_adj_132, n30045, 
        n28856, n28855, n28854, n28853, n8_adj_133, n29415, n10_adj_134, 
        n6_adj_135, n29382, n5_adj_136, n30301, n29489, n30185, 
        n30183, n30300, n30182, n30181, n30180, n30125, n29490, 
        n30121, n30179, n12_adj_137, n11_adj_138, n29036, n29032, 
        n29030;
    
    VHI i2 (.Z(VCC_net));
    LUT4 i4_4_lut (.A(con[7]), .B(n29440), .C(con[6]), .D(n30357), .Z(n4643)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i4_4_lut.init = 16'h8000;
    FD1S3AX rclk_r2_476 (.D(rclk_r1), .CK(clk_c), .Q(rclk_r2));
    defparam rclk_r2_476.GSR = "DISABLED";
    LUT4 i1_2_lut (.A(con[0]), .B(n17_adj_428[1]), .Z(n30327)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut.init = 16'h8888;
    FD1S3AX rclk_r_478 (.D(rclk_r3), .CK(clk_c), .Q(rclk_r));
    defparam rclk_r_478.GSR = "DISABLED";
    FD1P3AX sw6_f2n_i0_i0 (.D(n482[0]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[0]));
    defparam sw6_f2n_i0_i0.GSR = "DISABLED";
    FD1S3AX rx_int0_483 (.D(txen), .CK(clk_c), .Q(rx_int0));
    defparam rx_int0_483.GSR = "ENABLED";
    FD1S3AX rx_int1_484 (.D(rx_int0), .CK(clk_c), .Q(rx_int1));
    defparam rx_int1_484.GSR = "ENABLED";
    FD1P3IX db9_402_en_515 (.D(n30301), .SP(n2820), .CD(n4415), .CK(clk_c), 
            .Q(db9_402_en));
    defparam db9_402_en_515.GSR = "DISABLED";
    FD1P3AX sw5_f2n_i0_i0 (.D(n482[0]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[0]));
    defparam sw5_f2n_i0_i0.GSR = "DISABLED";
    IB rstn_pad (.I(rstn), .O(rstn_c));
    LUT4 i27841_4_lut (.A(n30349), .B(n1510[7]), .C(n17_adj_428[4]), .D(n30344), 
         .Z(n29607)) /* synthesis lut_function=(A (B+(C+(D)))) */ ;
    defparam i27841_4_lut.init = 16'haaa8;
    LUT4 i27853_4_lut (.A(con[0]), .B(n29607), .C(n17_adj_428[0]), .D(n4643), 
         .Z(n29619)) /* synthesis lut_function=(A (B+(C))+!A (B+!((D)+!C))) */ ;
    defparam i27853_4_lut.init = 16'hecfc;
    LUT4 n30053_bdd_2_lut (.A(n30053), .B(rs232tx), .Z(n30054)) /* synthesis lut_function=(A+(B)) */ ;
    defparam n30053_bdd_2_lut.init = 16'heeee;
    FD1P3AX sw4_f2n_i0_i0 (.D(n482[0]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[0]));
    defparam sw4_f2n_i0_i0.GSR = "DISABLED";
    LUT4 chnum_0__bdd_4_lut (.A(chnum[0]), .B(chnum[1]), .C(chnum[2]), 
         .D(chnum[3]), .Z(n30048)) /* synthesis lut_function=(!(A (B+(C+!(D)))+!A (((D)+!C)+!B))) */ ;
    defparam chnum_0__bdd_4_lut.init = 16'h0240;
    LUT4 n30045_bdd_2_lut (.A(n30045), .B(rs232tx), .Z(n30046)) /* synthesis lut_function=(A+(B)) */ ;
    defparam n30045_bdd_2_lut.init = 16'heeee;
    LUT4 rs232tx_bdd_4_lut_28154 (.A(chnum[3]), .B(chnum[2]), .C(chnum[0]), 
         .D(chnum[1]), .Z(n30045)) /* synthesis lut_function=(A (B+((D)+!C))+!A ((C+(D))+!B)) */ ;
    defparam rs232tx_bdd_4_lut_28154.init = 16'hffdb;
    LUT4 i1_2_lut_3_lut_4_lut (.A(rclk_r3), .B(rclk_r), .C(n17_adj_428[3]), 
         .D(rstn_c), .Z(n4152)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'h4000;
    LUT4 i28089_4_lut_3_lut (.A(rclk_r3), .B(rclk_r), .C(ack2mcu_c), .Z(n29858)) /* synthesis lut_function=(A ((C)+!B)+!A !(B+!(C))) */ ;
    defparam i28089_4_lut_3_lut.init = 16'hb2b2;
    LUT4 i1_2_lut_3_lut_4_lut_adj_16 (.A(rclk_r3), .B(rclk_r), .C(n17_adj_428[1]), 
         .D(rstn_c), .Z(n4154)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_16.init = 16'h4000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_17 (.A(rclk_r3), .B(rclk_r), .C(n17_adj_428[2]), 
         .D(rstn_c), .Z(n4153)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_17.init = 16'h4000;
    LUT4 i1_2_lut_rep_100_3_lut (.A(rclk_r3), .B(rclk_r), .C(rstn_c), 
         .Z(n30131)) /* synthesis lut_function=(!(A+!(B (C)))) */ ;
    defparam i1_2_lut_rep_100_3_lut.init = 16'h4040;
    LUT4 i17_2_lut_rep_116 (.A(rclk_r3), .B(rclk_r), .Z(n30147)) /* synthesis lut_function=(!(A+!(B))) */ ;
    defparam i17_2_lut_rep_116.init = 16'h4444;
    LUT4 i2_3_lut_rep_101_4_lut (.A(con[2]), .B(con[3]), .C(con[1]), .D(n14_adj_71), 
         .Z(n30132)) /* synthesis lut_function=((((D)+!C)+!B)+!A) */ ;
    defparam i2_3_lut_rep_101_4_lut.init = 16'hff7f;
    LUT4 i2_3_lut_4_lut (.A(con[2]), .B(con[3]), .C(con[1]), .D(n29390), 
         .Z(n29392)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i2_3_lut_4_lut.init = 16'h0800;
    FD1P3AX step_FSM__i7 (.D(n29005), .SP(n30147), .CK(clk_c), .Q(n1510[7]));
    defparam step_FSM__i7.GSR = "ENABLED";
    FD1P3IX sp_com_data_i0_i6 (.D(con[6]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[6]));
    defparam sp_com_data_i0_i6.GSR = "DISABLED";
    FD1P3AX step_FSM__i6 (.D(n3607), .SP(n30147), .CK(clk_c), .Q(n17_adj_428[6]));
    defparam step_FSM__i6.GSR = "ENABLED";
    FD1S3AX rclk_r1_475 (.D(rclk_r0), .CK(clk_c), .Q(rclk_r1));
    defparam rclk_r1_475.GSR = "DISABLED";
    FD1P3AX step_FSM__i5 (.D(n4663), .SP(n30147), .CK(clk_c), .Q(n17_adj_428[5]));
    defparam step_FSM__i5.GSR = "ENABLED";
    LUT4 i14798_2_lut_rep_115 (.A(con[2]), .B(con[3]), .Z(n30146)) /* synthesis lut_function=(A (B)) */ ;
    defparam i14798_2_lut_rep_115.init = 16'h8888;
    FD1P3IX sp_com_data_i0_i7 (.D(con[7]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[7]));
    defparam sp_com_data_i0_i7.GSR = "DISABLED";
    FD1P3AX step_FSM__i4 (.D(n29110), .SP(n30147), .CK(clk_c), .Q(n17_adj_428[4]));
    defparam step_FSM__i4.GSR = "ENABLED";
    FD1S3AX rs232_402_all_back_i0 (.D(n30130), .CK(clk_c), .Q(rs232_402_all_back[0]));
    defparam rs232_402_all_back_i0.GSR = "DISABLED";
    FD1S3AX clock_count15_reg0_511 (.D(clock_count[15]), .CK(clk_c), .Q(clock_count15_reg0));
    defparam clock_count15_reg0_511.GSR = "DISABLED";
    FD1S3AX clock_count15_reg1_512 (.D(clock_count15_reg0), .CK(clk_c), 
            .Q(clock_count15_reg1));
    defparam clock_count15_reg1_512.GSR = "DISABLED";
    FD1P3AX step_FSM__i3 (.D(n17_adj_428[2]), .SP(n30147), .CK(clk_c), 
            .Q(n17_adj_428[3]));
    defparam step_FSM__i3.GSR = "ENABLED";
    FD1S3AX rclk_r0_474 (.D(rclk_c), .CK(clk_c), .Q(rclk_r0));
    defparam rclk_r0_474.GSR = "DISABLED";
    FD1P3AX step_FSM__i2 (.D(n29392), .SP(n30147), .CK(clk_c), .Q(n17_adj_428[2]));
    defparam step_FSM__i2.GSR = "ENABLED";
    FD1S3AX qa_32 (.D(qa0), .CK(rclk_c), .Q(con[0]));
    defparam qa_32.GSR = "DISABLED";
    FD1S3AX qb_33 (.D(qb0), .CK(rclk_c), .Q(con[1]));
    defparam qb_33.GSR = "DISABLED";
    FD1S3AX qc_34 (.D(qc0), .CK(rclk_c), .Q(con[2]));
    defparam qc_34.GSR = "DISABLED";
    FD1S3AX qd_35 (.D(qd0), .CK(rclk_c), .Q(con[3]));
    defparam qd_35.GSR = "DISABLED";
    FD1S3AX qe_36 (.D(qe0), .CK(rclk_c), .Q(con[4]));
    defparam qe_36.GSR = "DISABLED";
    FD1P3AX step_FSM__i1 (.D(n29391), .SP(n30147), .CK(clk_c), .Q(n17_adj_428[1]));
    defparam step_FSM__i1.GSR = "ENABLED";
    OFS1P3DX send_to_402_i7 (.D(n29415), .SP(VCC_net), .SCLK(clk_c), .CD(tx_baud_adj_282[2]), 
            .Q(rs232_402x_rx_c[7]));
    defparam send_to_402_i7.GSR = "DISABLED";
    check_IR_defv check_IR_defv7 (.\clock_count[15] (clock_count[15]), .ir_402x_i_c_7(ir_402x_i_c[7]), 
            .\def_voltag[7] (def_voltag[7]), .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    FD1P3AX sw3_f2n_i0_i0 (.D(n482[0]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[0]));
    defparam sw3_f2n_i0_i0.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i0 (.D(n482[0]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[0]));
    defparam sw2_f2n_i0_i0.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i0 (.D(n482[0]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[0]));
    defparam sw1_f2n_i0_i0.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i0 (.D(n482[0]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[0]));
    defparam sw0_f2n_i0_i0.GSR = "DISABLED";
    FD1P3AX bps_sel_i0_i0 (.D(n3), .SP(n4150), .CK(clk_c), .Q(bps_sel[0]));
    defparam bps_sel_i0_i0.GSR = "DISABLED";
    FD1P3AX chnum_i0_i0 (.D(n30345), .SP(n4151), .CK(clk_c), .Q(chnum[0]));
    defparam chnum_i0_i0.GSR = "DISABLED";
    LUT4 i1_2_lut_4_lut (.A(chnum[1]), .B(chnum[3]), .C(chnum[2]), .D(rs232tx), 
         .Z(n29415)) /* synthesis lut_function=(A+((C+(D))+!B)) */ ;
    defparam i1_2_lut_4_lut.init = 16'hfffb;
    FD1P3AX sp_com_data_i0_i0 (.D(n30327), .SP(n4154), .CK(clk_c), .Q(sp_com_data[0]));
    defparam sp_com_data_i0_i0.GSR = "DISABLED";
    FD1P3IX rxbaud_fifo_tx_db9_i0_i0 (.D(bps_sel[0]), .SP(n2820), .CD(n4415), 
            .CK(clk_c), .Q(rxbaud_fifo_tx_db9[0]));
    defparam rxbaud_fifo_tx_db9_i0_i0.GSR = "DISABLED";
    FD1P3AX sw5_f2n_i0_i7 (.D(n482[7]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[7]));
    defparam sw5_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw5_f2n_i0_i6 (.D(n482[6]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[6]));
    defparam sw5_f2n_i0_i6.GSR = "DISABLED";
    LUT4 i1_2_lut_adj_18 (.A(con[0]), .B(n17_adj_428[4]), .Z(n30345)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_18.init = 16'h8888;
    FD1S3IX wd_disable_402_to_mcu_1472__i0 (.D(n50_adj_603[0]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[0]));
    defparam wd_disable_402_to_mcu_1472__i0.GSR = "DISABLED";
    LUT4 i1_2_lut_3_lut_4_lut_adj_19 (.A(ir_com_o[2]), .B(ir_com_o[1]), 
         .C(ir_com_o[0]), .D(n30185), .Z(n2673)) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_19.init = 16'h0010;
    LUT4 i28085_2_lut_2_lut_3_lut_4_lut (.A(ir_com_o[2]), .B(ir_com_o[1]), 
         .C(ir_com_o[0]), .D(n30185), .Z(n2671)) /* synthesis lut_function=(!(A+(B+(C+(D))))) */ ;
    defparam i28085_2_lut_2_lut_3_lut_4_lut.init = 16'h0001;
    FD1P3AX to_db9_baud_i0_i0 (.D(sp_com_data[0]), .SP(n1829), .CK(clk_c), 
            .Q(to_db9_baud[0]));
    defparam to_db9_baud_i0_i0.GSR = "ENABLED";
    FD1S3AX car_cnt_1475__i0 (.D(n58_adj_596[0]), .CK(clk_c), .Q(car_cnt[0]));
    defparam car_cnt_1475__i0.GSR = "ENABLED";
    FD1P3AX sw7_f2n_i0_i0 (.D(n482[0]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[0]));
    defparam sw7_f2n_i0_i0.GSR = "DISABLED";
    IB clk_pad (.I(clk), .O(clk_c));
    OB ack2mcu_pad (.I(ack2mcu_c), .O(ack2mcu));
    OB irx_o_pad_4 (.I(irx_o_c[4]), .O(irx_o[4]));
    OB ir2mcu_pad (.I(n111[0]), .O(ir2mcu));
    OB rs232_mcu_rx_pad (.I(rs232_mcu_rx_c), .O(rs232_mcu_rx));
    OB irx_o_pad_5 (.I(irx_o_c[5]), .O(irx_o[5]));
    OB rs232_tcp_rx_pad (.I(rs232_tcp_rx_c_c), .O(rs232_tcp_rx));
    OB rs232_db9_rx_pad (.I(rs232_db9_rx_c), .O(rs232_db9_rx));
    LUT4 i1_2_lut_adj_20 (.A(n17_adj_428[5]), .B(con[0]), .Z(n3)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_20.init = 16'h8888;
    OB rs232_402x_rx_pad_0 (.I(rs232_402x_rx_c[0]), .O(rs232_402x_rx[0]));
    FD1P3AX sw5_f2n_i0_i5 (.D(n482[5]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[5]));
    defparam sw5_f2n_i0_i5.GSR = "DISABLED";
    OB rs232_402x_rx_pad_1 (.I(rs232_402x_rx_c[1]), .O(rs232_402x_rx[1]));
    OB rs232_402x_rx_pad_2 (.I(rs232_402x_rx_c[2]), .O(rs232_402x_rx[2]));
    OB rs232_402x_rx_pad_3 (.I(rs232_402x_rx_c[3]), .O(rs232_402x_rx[3]));
    FD1P3AX sw5_f2n_i0_i4 (.D(n482[4]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[4]));
    defparam sw5_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw5_f2n_i0_i3 (.D(n482[3]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[3]));
    defparam sw5_f2n_i0_i3.GSR = "DISABLED";
    OB rs232_402x_rx_pad_4 (.I(rs232_402x_rx_c[4]), .O(rs232_402x_rx[4]));
    OB rs232_402x_rx_pad_5 (.I(rs232_402x_rx_c[5]), .O(rs232_402x_rx[5]));
    FD1P3AX sw5_f2n_i0_i2 (.D(n482[2]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[2]));
    defparam sw5_f2n_i0_i2.GSR = "DISABLED";
    OB rs232_402x_rx_pad_6 (.I(rs232_402x_rx_c[6]), .O(rs232_402x_rx[6]));
    OB rs232_402x_rx_pad_7 (.I(rs232_402x_rx_c[7]), .O(rs232_402x_rx[7]));
    OB ir_402x_o_pad_0 (.I(irx_i_c[0]), .O(ir_402x_o[0]));
    LUT4 i1_2_lut_4_lut_adj_21 (.A(rs232tx), .B(chnum[1]), .C(chnum[0]), 
         .D(chnum[2]), .Z(n29489)) /* synthesis lut_function=(A+(B+((D)+!C))) */ ;
    defparam i1_2_lut_4_lut_adj_21.init = 16'hffef;
    LUT4 rs232tx_bdd_3_lut_rep_109 (.A(rs232tx), .B(chnum[1]), .C(chnum[0]), 
         .Z(n30140)) /* synthesis lut_function=(A+(B+!(C))) */ ;
    defparam rs232tx_bdd_3_lut_rep_109.init = 16'hefef;
    FD1P3AX sw5_f2n_i0_i1 (.D(n482[1]), .SP(n2681), .CK(clk_c), .Q(sw5_f2n[1]));
    defparam sw5_f2n_i0_i1.GSR = "DISABLED";
    OB ir_402x_o_pad_1 (.I(irx_i_c[1]), .O(ir_402x_o[1]));
    OB ir_402x_o_pad_2 (.I(irx_i_c[2]), .O(ir_402x_o[2]));
    OB ir_402x_o_pad_3 (.I(irx_i_c[3]), .O(ir_402x_o[3]));
    my_uart_top uart_u2 (.n134(n30348), .rstn_c(rstn_c), .clk_c(clk_c), 
            .GND_net(tx_baud_adj_282[2]), .bps_sel({bps_sel}), .rdy(rdy), 
            .rden(rden), .txdata({txdata}), .rs232tx(rs232tx), .n4439(n4439)) /* synthesis syn_module_defined=1 */ ;
    OB ir_402x_o_pad_4 (.I(irx_i_c[4]), .O(ir_402x_o[4]));
    OB ir_402x_o_pad_5 (.I(irx_i_c[5]), .O(ir_402x_o[5]));
    OB ir_402x_o_pad_6 (.I(irx_i_c[6]), .O(ir_402x_o[6]));
    IB rclk_pad (.I(rclk), .O(rclk_c));
    OB ir_402x_o_pad_7 (.I(irx_i_c[7]), .O(ir_402x_o[7]));
    FD1P3AY step_FSM__i0 (.D(n29619), .SP(n30147), .CK(clk_c), .Q(n17_adj_428[0]));
    defparam step_FSM__i0.GSR = "ENABLED";
    OB irx_o_pad_0 (.I(irx_o_c[0]), .O(irx_o[0]));
    LUT4 i3_4_lut (.A(con[2]), .B(con[3]), .C(con[1]), .D(n29390), .Z(n29391)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i3_4_lut.init = 16'h4000;
    LUT4 i2_3_lut (.A(n17_adj_428[0]), .B(n14_adj_71), .C(con[0]), .Z(n29390)) /* synthesis lut_function=(!((B+(C))+!A)) */ ;
    defparam i2_3_lut.init = 16'h0202;
    FD1P3AX sw6_f2n_i0_i7 (.D(n482[7]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[7]));
    defparam sw6_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw6_f2n_i0_i6 (.D(n482[6]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[6]));
    defparam sw6_f2n_i0_i6.GSR = "DISABLED";
    OB irx_o_pad_1 (.I(irx_o_c[1]), .O(irx_o[1]));
    FD1P3AX sw6_f2n_i0_i5 (.D(n482[5]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[5]));
    defparam sw6_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw6_f2n_i0_i4 (.D(n482[4]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[4]));
    defparam sw6_f2n_i0_i4.GSR = "DISABLED";
    OB irx_o_pad_2 (.I(irx_o_c[2]), .O(irx_o[2]));
    FD1P3AX sw6_f2n_i0_i3 (.D(n482[3]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[3]));
    defparam sw6_f2n_i0_i3.GSR = "DISABLED";
    OB irx_o_pad_3 (.I(irx_o_c[3]), .O(irx_o[3]));
    LUT4 i1_2_lut_3_lut_4_lut_adj_22 (.A(n30185), .B(ir_com_o[0]), .C(ir_com_o[2]), 
         .D(ir_com_o[1]), .Z(n2675)) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_22.init = 16'h0100;
    LUT4 i1_2_lut_3_lut_4_lut_adj_23 (.A(n30185), .B(ir_com_o[0]), .C(ir_com_o[2]), 
         .D(ir_com_o[1]), .Z(n2683)) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_23.init = 16'h1000;
    LUT4 i4_4_lut_adj_24 (.A(rs232_402x_tx_c[2]), .B(rs232_402x_tx_c[5]), 
         .C(rs232_402x_tx_c[0]), .D(rs232_402x_tx_c[1]), .Z(n10_adj_126)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i4_4_lut_adj_24.init = 16'h8000;
    LUT4 i1_2_lut_3_lut (.A(n30162), .B(sp_com_data[4]), .C(n30179), .Z(n29471)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i1_2_lut_3_lut.init = 16'h0808;
    LUT4 i1_2_lut_adj_25 (.A(n17_adj_428[4]), .B(n17_adj_428[5]), .Z(n5_adj_136)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i1_2_lut_adj_25.init = 16'heeee;
    LUT4 i1_2_lut_3_lut_adj_26 (.A(n30162), .B(sp_com_data[4]), .C(n30183), 
         .Z(n29472)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i1_2_lut_3_lut_adj_26.init = 16'h8080;
    LUT4 i2_2_lut (.A(n17_adj_428[0]), .B(n17_adj_428[6]), .Z(n6_adj_135)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i2_2_lut.init = 16'heeee;
    LUT4 i2_4_lut (.A(n30132), .B(con[0]), .C(n5_adj_136), .D(n6_adj_135), 
         .Z(n29110)) /* synthesis lut_function=(!(A+(B+!(C+(D))))) */ ;
    defparam i2_4_lut.init = 16'h1110;
    FD1P3AX sw6_f2n_i0_i2 (.D(n482[2]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[2]));
    defparam sw6_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX ir_car_care_507 (.D(n29387), .SP(n29472), .CK(clk_c), .Q(ir_car_care));
    defparam ir_car_care_507.GSR = "ENABLED";
    LUT4 chnum_1__bdd_3_lut (.A(chnum[1]), .B(chnum[0]), .C(chnum[3]), 
         .Z(n30044)) /* synthesis lut_function=(!(A (B+(C))+!A !(B (C)))) */ ;
    defparam chnum_1__bdd_3_lut.init = 16'h4242;
    FD1P3AX f_to_mcu_en_508 (.D(n29382), .SP(n29471), .CK(clk_c), .Q(f_to_mcu_en));
    defparam f_to_mcu_en_508.GSR = "ENABLED";
    OB irx_o_pad_6 (.I(irx_o_c[6]), .O(irx_o[6]));
    LUT4 i1_2_lut_adj_27 (.A(n17_adj_428[5]), .B(n17_adj_428[6]), .Z(n30344)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i1_2_lut_adj_27.init = 16'heeee;
    FD1P3AX getting_com_487 (.D(n30351), .SP(n30131), .CK(clk_c), .Q(getting_com));
    defparam getting_com_487.GSR = "DISABLED";
    FD1P3AX wren_496 (.D(n30147), .SP(n4210), .CK(clk_c), .Q(wren));
    defparam wren_496.GSR = "DISABLED";
    LUT4 i3_4_lut_adj_28 (.A(con[4]), .B(con[6]), .C(con[7]), .D(con[5]), 
         .Z(n14_adj_71)) /* synthesis lut_function=(A+!(B (C (D)))) */ ;
    defparam i3_4_lut_adj_28.init = 16'hbfff;
    LUT4 i1_2_lut_adj_29 (.A(n17_adj_428[1]), .B(n17_adj_428[3]), .Z(n4_adj_130)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i1_2_lut_adj_29.init = 16'heeee;
    LUT4 i2_4_lut_adj_30 (.A(con[0]), .B(n4_adj_130), .C(n1510[7]), .D(n30132), 
         .Z(n29005)) /* synthesis lut_function=(A (B+(C (D)))+!A (B+(C))) */ ;
    defparam i2_4_lut_adj_30.init = 16'hfcdc;
    FD1P3AX sw6_f2n_i0_i1 (.D(n482[1]), .SP(n2683), .CK(clk_c), .Q(sw6_f2n[1]));
    defparam sw6_f2n_i0_i1.GSR = "DISABLED";
    OB irx_o_pad_7 (.I(irx_o_c[7]), .O(irx_o[7]));
    OB ir_all_out_pad (.I(ir_all_out_c), .O(ir_all_out));
    FD1P3IX ir_402_com_i__i1 (.D(n4683), .SP(n4153), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_402_com_i[0]));
    defparam ir_402_com_i__i1.GSR = "DISABLED";
    LUT4 i27780_2_lut (.A(f_to_mcu_en), .B(wd_disable_402_to_mcu[10]), .Z(n29545)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27780_2_lut.init = 16'h8888;
    LUT4 i472_4_lut (.A(rs232_db9_rx_c), .B(rs232_db9_tx_c), .C(db9_402_en), 
         .D(n29545), .Z(n1175)) /* synthesis lut_function=(A (B)+!A !((C (D))+!B)) */ ;
    defparam i472_4_lut.init = 16'h8ccc;
    FD1S3AX clock_count_1473__i0 (.D(n70_adj_600[0]), .CK(clk_c), .Q(n122[0]));
    defparam clock_count_1473__i0.GSR = "ENABLED";
    FD1P3IX chnum_i0_i1 (.D(con[1]), .SP(n4151), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(chnum[1]));
    defparam chnum_i0_i1.GSR = "DISABLED";
    FD1P3IX chnum_i0_i2 (.D(con[2]), .SP(n4151), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(chnum[2]));
    defparam chnum_i0_i2.GSR = "DISABLED";
    FD1P3IX chnum_i0_i3 (.D(con[3]), .SP(n4151), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(chnum[3]));
    defparam chnum_i0_i3.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i7 (.D(n482[7]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[7]));
    defparam sw7_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i6 (.D(n482[6]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[6]));
    defparam sw7_f2n_i0_i6.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i5 (.D(n482[5]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[5]));
    defparam sw7_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i4 (.D(n482[4]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[4]));
    defparam sw7_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i3 (.D(n482[3]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[3]));
    defparam sw7_f2n_i0_i3.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i2 (.D(n482[2]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[2]));
    defparam sw7_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX sw7_f2n_i0_i1 (.D(n482[1]), .SP(n2685), .CK(clk_c), .Q(sw7_f2n[1]));
    defparam sw7_f2n_i0_i1.GSR = "DISABLED";
    FD1P3AX to_db9_baud_i0_i2 (.D(sp_com_data[2]), .SP(n1829), .CK(clk_c), 
            .Q(to_db9_baud[2]));
    defparam to_db9_baud_i0_i2.GSR = "ENABLED";
    FD1P3AX to_db9_baud_i0_i1 (.D(sp_com_data[1]), .SP(n1829), .CK(clk_c), 
            .Q(to_db9_baud[1]));
    defparam to_db9_baud_i0_i1.GSR = "ENABLED";
    FD1P3IX rxbaud_fifo_tx_db9_i0_i2 (.D(bps_sel[2]), .SP(n2820), .CD(n4415), 
            .CK(clk_c), .Q(rxbaud_fifo_tx_db9[2]));
    defparam rxbaud_fifo_tx_db9_i0_i2.GSR = "DISABLED";
    FD1P3IX rxbaud_fifo_tx_db9_i0_i1 (.D(bps_sel[1]), .SP(n2820), .CD(n4415), 
            .CK(clk_c), .Q(rxbaud_fifo_tx_db9[1]));
    defparam rxbaud_fifo_tx_db9_i0_i1.GSR = "DISABLED";
    FD1P3AX bps_sel_i0_i2 (.D(n3_adj_73), .SP(n4150), .CK(clk_c), .Q(bps_sel[2]));
    defparam bps_sel_i0_i2.GSR = "DISABLED";
    FD1P3AX bps_sel_i0_i1 (.D(n3_adj_72), .SP(n4150), .CK(clk_c), .Q(bps_sel[1]));
    defparam bps_sel_i0_i1.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i7 (.D(n482[7]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[7]));
    defparam sw0_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i6 (.D(n482[6]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[6]));
    defparam sw0_f2n_i0_i6.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i5 (.D(n482[5]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[5]));
    defparam sw0_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i4 (.D(n482[4]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[4]));
    defparam sw0_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i3 (.D(n482[3]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[3]));
    defparam sw0_f2n_i0_i3.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i2 (.D(n482[2]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[2]));
    defparam sw0_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX sw0_f2n_i0_i1 (.D(n482[1]), .SP(n2671), .CK(clk_c), .Q(sw0_f2n[1]));
    defparam sw0_f2n_i0_i1.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i7 (.D(n482[7]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[7]));
    defparam sw1_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i6 (.D(n482[6]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[6]));
    defparam sw1_f2n_i0_i6.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i5 (.D(n482[5]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[5]));
    defparam sw1_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i4 (.D(n482[4]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[4]));
    defparam sw1_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i3 (.D(n482[3]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[3]));
    defparam sw1_f2n_i0_i3.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i2 (.D(n482[2]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[2]));
    defparam sw1_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX sw1_f2n_i0_i1 (.D(n482[1]), .SP(n2673), .CK(clk_c), .Q(sw1_f2n[1]));
    defparam sw1_f2n_i0_i1.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i7 (.D(n482[7]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[7]));
    defparam sw2_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i6 (.D(n482[6]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[6]));
    defparam sw2_f2n_i0_i6.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i5 (.D(n482[5]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[5]));
    defparam sw2_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i4 (.D(n482[4]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[4]));
    defparam sw2_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i3 (.D(n482[3]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[3]));
    defparam sw2_f2n_i0_i3.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i2 (.D(n482[2]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[2]));
    defparam sw2_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX sw2_f2n_i0_i1 (.D(n482[1]), .SP(n2675), .CK(clk_c), .Q(sw2_f2n[1]));
    defparam sw2_f2n_i0_i1.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i7 (.D(n482[7]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[7]));
    defparam sw3_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i6 (.D(n482[6]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[6]));
    defparam sw3_f2n_i0_i6.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i5 (.D(n482[5]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[5]));
    defparam sw3_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i4 (.D(n482[4]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[4]));
    defparam sw3_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i3 (.D(n482[3]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[3]));
    defparam sw3_f2n_i0_i3.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i2 (.D(n482[2]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[2]));
    defparam sw3_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX sw3_f2n_i0_i1 (.D(n482[1]), .SP(n2677), .CK(clk_c), .Q(sw3_f2n[1]));
    defparam sw3_f2n_i0_i1.GSR = "DISABLED";
    FD1P3IX ir_com_o__i2 (.D(n29032), .SP(n4152), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_com_o[1]));
    defparam ir_com_o__i2.GSR = "DISABLED";
    FD1P3IX ir_com_o__i1 (.D(n4683), .SP(n4152), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_com_o[0]));
    defparam ir_com_o__i1.GSR = "DISABLED";
    FD1S3AX rs232_402_all_back_i2 (.D(rs232_402_all_back[1]), .CK(clk_c), 
            .Q(rs232_402_all_back[2]));
    defparam rs232_402_all_back_i2.GSR = "DISABLED";
    FD1S3AX rs232_402_all_back_i1 (.D(rs232_402_all_back[0]), .CK(clk_c), 
            .Q(rs232_402_all_back[1]));
    defparam rs232_402_all_back_i1.GSR = "DISABLED";
    OFS1P3DX send_to_402_i6 (.D(n30054), .SP(VCC_net), .SCLK(clk_c), .CD(tx_baud_adj_282[2]), 
            .Q(rs232_402x_rx_c[6]));
    defparam send_to_402_i6.GSR = "DISABLED";
    OFS1P3DX send_to_402_i5 (.D(n10_adj_131), .SP(VCC_net), .SCLK(clk_c), 
            .CD(tx_baud_adj_282[2]), .Q(rs232_402x_rx_c[5]));
    defparam send_to_402_i5.GSR = "DISABLED";
    OFS1P3DX send_to_402_i4 (.D(n29490), .SP(VCC_net), .SCLK(clk_c), .CD(tx_baud_adj_282[2]), 
            .Q(rs232_402x_rx_c[4]));
    defparam send_to_402_i4.GSR = "DISABLED";
    OFS1P3DX send_to_402_i3 (.D(n30046), .SP(VCC_net), .SCLK(clk_c), .CD(tx_baud_adj_282[2]), 
            .Q(rs232_402x_rx_c[3]));
    defparam send_to_402_i3.GSR = "DISABLED";
    OFS1P3DX send_to_402_i2 (.D(n8_adj_133), .SP(VCC_net), .SCLK(clk_c), 
            .CD(tx_baud_adj_282[2]), .Q(rs232_402x_rx_c[2]));
    defparam send_to_402_i2.GSR = "DISABLED";
    OFS1P3DX send_to_402_i1 (.D(n11_adj_132), .SP(VCC_net), .SCLK(clk_c), 
            .CD(tx_baud_adj_282[2]), .Q(rs232_402x_rx_c[1]));
    defparam send_to_402_i1.GSR = "DISABLED";
    OFS1P3DX send_to_402_i0 (.D(n29489), .SP(VCC_net), .SCLK(clk_c), .CD(tx_baud_adj_282[2]), 
            .Q(rs232_402x_rx_c[0]));
    defparam send_to_402_i0.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i7 (.D(n482[7]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[7]));
    defparam sw4_f2n_i0_i7.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i6 (.D(n482[6]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[6]));
    defparam sw4_f2n_i0_i6.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i5 (.D(n482[5]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[5]));
    defparam sw4_f2n_i0_i5.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i4 (.D(n482[4]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[4]));
    defparam sw4_f2n_i0_i4.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i3 (.D(n482[3]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[3]));
    defparam sw4_f2n_i0_i3.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i2 (.D(n482[2]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[2]));
    defparam sw4_f2n_i0_i2.GSR = "DISABLED";
    FD1P3AX sw4_f2n_i0_i1 (.D(n482[1]), .SP(n2679), .CK(clk_c), .Q(sw4_f2n[1]));
    defparam sw4_f2n_i0_i1.GSR = "DISABLED";
    FD1P3IX ir_com_o__i3 (.D(n29036), .SP(n4152), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_com_o[2]));
    defparam ir_com_o__i3.GSR = "DISABLED";
    IB srclk_pad (.I(srclk), .O(srclk_c));
    IB ser_pad (.I(ser), .O(ser_c));
    IB ir_eye_pad (.I(ir_eye), .O(ir_defX_in[0]));
    IB ir_front_pad (.I(ir_front), .O(ir_front_c));
    IB ir_all_in_pad (.I(ir_all_in), .O(ir_all_in_c));
    IB n3419_pad (.I(irx_i[7]), .O(irx_i_c[7]));
    IB n3420_pad (.I(irx_i[6]), .O(irx_i_c[6]));
    IB n3421_pad (.I(irx_i[5]), .O(irx_i_c[5]));
    IB n3422_pad (.I(irx_i[4]), .O(irx_i_c[4]));
    IB n3423_pad (.I(irx_i[3]), .O(irx_i_c[3]));
    IB n3424_pad (.I(irx_i[2]), .O(irx_i_c[2]));
    IB n3425_pad (.I(irx_i[1]), .O(irx_i_c[1]));
    IB ir_402x_o_c_pad (.I(irx_i[0]), .O(irx_i_c[0]));
    IB ir_402x_i_pad_7 (.I(ir_402x_i[7]), .O(ir_402x_i_c[7]));
    IB ir_402x_i_pad_6 (.I(ir_402x_i[6]), .O(ir_402x_i_c[6]));
    IB ir_402x_i_pad_5 (.I(ir_402x_i[5]), .O(ir_402x_i_c[5]));
    IB ir_402x_i_pad_4 (.I(ir_402x_i[4]), .O(ir_402x_i_c[4]));
    IB ir_402x_i_pad_3 (.I(ir_402x_i[3]), .O(ir_402x_i_c[3]));
    IB ir_402x_i_pad_2 (.I(ir_402x_i[2]), .O(ir_402x_i_c[2]));
    IB ir_402x_i_pad_1 (.I(ir_402x_i[1]), .O(ir_402x_i_c[1]));
    IB ir_402x_i_pad_0 (.I(ir_402x_i[0]), .O(ir_402x_i_c[0]));
    IB rs232_402x_tx_pad_7 (.I(rs232_402x_tx[7]), .O(rs232_402x_tx_c[7]));
    IB rs232_402x_tx_pad_6 (.I(rs232_402x_tx[6]), .O(rs232_402x_tx_c[6]));
    IB rs232_402x_tx_pad_5 (.I(rs232_402x_tx[5]), .O(rs232_402x_tx_c[5]));
    IB rs232_402x_tx_pad_4 (.I(rs232_402x_tx[4]), .O(rs232_402x_tx_c[4]));
    IB rs232_402x_tx_pad_3 (.I(rs232_402x_tx[3]), .O(rs232_402x_tx_c[3]));
    IB rs232_402x_tx_pad_2 (.I(rs232_402x_tx[2]), .O(rs232_402x_tx_c[2]));
    IB rs232_402x_tx_pad_1 (.I(rs232_402x_tx[1]), .O(rs232_402x_tx_c[1]));
    IB rs232_402x_tx_pad_0 (.I(rs232_402x_tx[0]), .O(rs232_402x_tx_c[0]));
    IB rs232_db9_tx_pad (.I(rs232_db9_tx), .O(rs232_db9_tx_c));
    IB rs232_tcp_tx_pad (.I(rs232_tcp_tx), .O(rs232_tcp_tx_c));
    IB rs232_tcp_rx_c_pad (.I(rs232_mcu_tx), .O(rs232_tcp_rx_c_c));
    FD1P3IX ir_com_o__i4 (.D(n29030), .SP(n4152), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_com_o[3]));
    defparam ir_com_o__i4.GSR = "DISABLED";
    LUT4 i1_2_lut_3_lut_4_lut_adj_31 (.A(n30185), .B(ir_com_o[0]), .C(ir_com_o[2]), 
         .D(ir_com_o[1]), .Z(n2677)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_31.init = 16'h0400;
    LUT4 i1_2_lut_3_lut_4_lut_adj_32 (.A(n30185), .B(ir_com_o[0]), .C(ir_com_o[2]), 
         .D(ir_com_o[1]), .Z(n2685)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_32.init = 16'h4000;
    FD1P3IX ir_402_com_i__i2 (.D(n29032), .SP(n4153), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_402_com_i[1]));
    defparam ir_402_com_i__i2.GSR = "DISABLED";
    FD1S3IX car_cnt_1475__i1 (.D(n46[1]), .CK(clk_c), .CD(n4438), .Q(car_cnt[1]));
    defparam car_cnt_1475__i1.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i2 (.D(n46[2]), .CK(clk_c), .CD(n4438), .Q(car_cnt[2]));
    defparam car_cnt_1475__i2.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i3 (.D(n46[3]), .CK(clk_c), .CD(n4438), .Q(car_cnt[3]));
    defparam car_cnt_1475__i3.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i4 (.D(n46[4]), .CK(clk_c), .CD(n4438), .Q(car_cnt[4]));
    defparam car_cnt_1475__i4.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i5 (.D(n46[5]), .CK(clk_c), .CD(n4438), .Q(car_cnt[5]));
    defparam car_cnt_1475__i5.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i6 (.D(n46[6]), .CK(clk_c), .CD(n4438), .Q(car_cnt[6]));
    defparam car_cnt_1475__i6.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i7 (.D(n46[7]), .CK(clk_c), .CD(n4438), .Q(car_cnt[7]));
    defparam car_cnt_1475__i7.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i8 (.D(n46[8]), .CK(clk_c), .CD(n4438), .Q(car_cnt[8]));
    defparam car_cnt_1475__i8.GSR = "ENABLED";
    FD1S3IX car_cnt_1475__i9 (.D(n46[9]), .CK(clk_c), .CD(n4438), .Q(car_cnt[9]));
    defparam car_cnt_1475__i9.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i1 (.D(n70_adj_600[1]), .CK(clk_c), .Q(n122[1]));
    defparam clock_count_1473__i1.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i2 (.D(n70_adj_600[2]), .CK(clk_c), .Q(n122[2]));
    defparam clock_count_1473__i2.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i3 (.D(n70_adj_600[3]), .CK(clk_c), .Q(n122[3]));
    defparam clock_count_1473__i3.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i4 (.D(n70_adj_600[4]), .CK(clk_c), .Q(n122[4]));
    defparam clock_count_1473__i4.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i5 (.D(n70_adj_600[5]), .CK(clk_c), .Q(n122[5]));
    defparam clock_count_1473__i5.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i6 (.D(n70_adj_600[6]), .CK(clk_c), .Q(n122[6]));
    defparam clock_count_1473__i6.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i7 (.D(n70_adj_600[7]), .CK(clk_c), .Q(n122[7]));
    defparam clock_count_1473__i7.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i8 (.D(n70_adj_600[8]), .CK(clk_c), .Q(n122[8]));
    defparam clock_count_1473__i8.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i9 (.D(n70_adj_600[9]), .CK(clk_c), .Q(n122[9]));
    defparam clock_count_1473__i9.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i10 (.D(n70_adj_600[10]), .CK(clk_c), .Q(n122[10]));
    defparam clock_count_1473__i10.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i11 (.D(n70_adj_600[11]), .CK(clk_c), .Q(n122[11]));
    defparam clock_count_1473__i11.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i12 (.D(n70_adj_600[12]), .CK(clk_c), .Q(n122[12]));
    defparam clock_count_1473__i12.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i13 (.D(n70_adj_600[13]), .CK(clk_c), .Q(n122[13]));
    defparam clock_count_1473__i13.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i14 (.D(n70_adj_600[14]), .CK(clk_c), .Q(n122[14]));
    defparam clock_count_1473__i14.GSR = "ENABLED";
    FD1S3AX clock_count_1473__i15 (.D(n70_adj_600[15]), .CK(clk_c), .Q(clock_count[15]));
    defparam clock_count_1473__i15.GSR = "ENABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i1 (.D(n50_adj_603[1]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[1]));
    defparam wd_disable_402_to_mcu_1472__i1.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i2 (.D(n50_adj_603[2]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[2]));
    defparam wd_disable_402_to_mcu_1472__i2.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i3 (.D(n50_adj_603[3]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[3]));
    defparam wd_disable_402_to_mcu_1472__i3.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i4 (.D(n50_adj_603[4]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[4]));
    defparam wd_disable_402_to_mcu_1472__i4.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i5 (.D(n50_adj_603[5]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[5]));
    defparam wd_disable_402_to_mcu_1472__i5.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i6 (.D(n50_adj_603[6]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[6]));
    defparam wd_disable_402_to_mcu_1472__i6.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i7 (.D(n50_adj_603[7]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[7]));
    defparam wd_disable_402_to_mcu_1472__i7.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i8 (.D(n50_adj_603[8]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[8]));
    defparam wd_disable_402_to_mcu_1472__i8.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i9 (.D(n50_adj_603[9]), .CK(clk_c), 
            .CD(n4439), .Q(n87_adj_605[9]));
    defparam wd_disable_402_to_mcu_1472__i9.GSR = "DISABLED";
    FD1S3IX wd_disable_402_to_mcu_1472__i10 (.D(n50_adj_603[10]), .CK(clk_c), 
            .CD(n4439), .Q(wd_disable_402_to_mcu[10]));
    defparam wd_disable_402_to_mcu_1472__i10.GSR = "DISABLED";
    FD1P3IX ir_402_com_i__i3 (.D(n29036), .SP(n4153), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_402_com_i[2]));
    defparam ir_402_com_i__i3.GSR = "DISABLED";
    FD1P3IX ir_402_com_i__i4 (.D(n29030), .SP(n4153), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(ir_402_com_i[3]));
    defparam ir_402_com_i__i4.GSR = "DISABLED";
    FD1P3IX sp_com_data_i0_i1 (.D(con[1]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[1]));
    defparam sp_com_data_i0_i1.GSR = "DISABLED";
    FD1P3IX sp_com_data_i0_i2 (.D(con[2]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[2]));
    defparam sp_com_data_i0_i2.GSR = "DISABLED";
    FD1P3IX txen_482 (.D(n30301), .SP(n121), .CD(rdy), .CK(clk_c), .Q(txen));
    defparam txen_482.GSR = "ENABLED";
    FD1P3JX rclk_r3_477 (.D(n30300), .SP(n30328), .PD(n13), .CK(clk_c), 
            .Q(rclk_r3));
    defparam rclk_r3_477.GSR = "DISABLED";
    FD1P3IX sp_com_data_i0_i3 (.D(con[3]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[3]));
    defparam sp_com_data_i0_i3.GSR = "DISABLED";
    FD1P3IX sp_com_data_i0_i4 (.D(con[4]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[4]));
    defparam sp_com_data_i0_i4.GSR = "DISABLED";
    fifo64x8_U10 fifo_u1 (.con({con}), .clk_c(clk_c), .wren_w(wren_w), 
            .rden(rden), .n134(n30348), .txdata({txdata}), .empty(empty), 
            .full(full), .GND_net(tx_baud_adj_282[2]), .VCC_net(VCC_net)) /* synthesis syn_module_defined=1 */ ;
    GSR GSR_INST (.GSR(rstn_c));
    INV i3094 (.A(con[0]), .Z(n4683));
    LUT4 i4_4_lut_adj_33 (.A(sw5_f2n[7]), .B(sw1_f2n[7]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_90)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_33.init = 16'hf531;
    LUT4 i1_2_lut_3_lut_4_lut_adj_34 (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[2]), .D(ir_402_com_i[0]), .Z(n482[3])) /* synthesis lut_function=(!((B+(C+!(D)))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_34.init = 16'h0200;
    FD1P3IX sp_com_data_i0_i5 (.D(con[5]), .SP(n4154), .CD(tx_baud_adj_282[2]), 
            .CK(clk_c), .Q(sp_com_data[5]));
    defparam sp_com_data_i0_i5.GSR = "DISABLED";
    LUT4 i1_2_lut_3_lut_4_lut_adj_35 (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[2]), .D(ir_402_com_i[0]), .Z(n482[7])) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_35.init = 16'h2000;
    LUT4 i2_4_lut_adj_36 (.A(sw6_f2n[7]), .B(sw7_f2n[7]), .C(n30177), 
         .D(n30176), .Z(n10_adj_116)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_36.init = 16'hf531;
    LUT4 i6_4_lut (.A(sw2_f2n[7]), .B(n12_adj_90), .C(n1386[7]), .D(n30181), 
         .Z(n14_adj_89)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut.init = 16'hc040;
    LUT4 i7_4_lut (.A(n1381[7]), .B(n14_adj_89), .C(n10_adj_116), .D(n1388[7]), 
         .Z(f2n_irx_temp[7])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut.init = 16'h8000;
    LUT4 i4_4_lut_adj_37 (.A(sw5_f2n[6]), .B(sw1_f2n[6]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_119)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_37.init = 16'hf531;
    LUT4 i1_2_lut_3_lut_4_lut_adj_38 (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[2]), .D(ir_402_com_i[0]), .Z(n482[6])) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_38.init = 16'h0020;
    LUT4 i1_2_lut_3_lut_4_lut_adj_39 (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[2]), .D(ir_402_com_i[0]), .Z(n482[2])) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_39.init = 16'h0002;
    LUT4 i2_4_lut_adj_40 (.A(sw6_f2n[6]), .B(sw7_f2n[6]), .C(n30177), 
         .D(n30176), .Z(n10_adj_121)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_40.init = 16'hf531;
    LUT4 i6_4_lut_adj_41 (.A(sw2_f2n[6]), .B(n12_adj_119), .C(n1386[6]), 
         .D(n30181), .Z(n14_adj_118)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_41.init = 16'hc040;
    LUT4 i7_4_lut_adj_42 (.A(n1381[6]), .B(n14_adj_118), .C(n10_adj_121), 
         .D(n1388[6]), .Z(f2n_irx_temp[6])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_42.init = 16'h8000;
    LUT4 i431_4_lut (.A(f2n_irx_temp[5]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_78), .Z(irx_o_c[5])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i431_4_lut.init = 16'hbaaa;
    LUT4 i473_2_lut (.A(rs232_send_to_mcu_changebaud), .B(rs232_tcp_tx_c), 
         .Z(rs232_mcu_rx_c)) /* synthesis lut_function=(A (B)) */ ;
    defparam i473_2_lut.init = 16'h8888;
    LUT4 i15178_4_lut (.A(car_cnt[5]), .B(car_cnt[6]), .C(n16703), .D(car_cnt[4]), 
         .Z(n16755)) /* synthesis lut_function=(A (B)+!A (B (C+(D)))) */ ;
    defparam i15178_4_lut.init = 16'hccc8;
    LUT4 i15208_4_lut (.A(n16755), .B(car_cnt[9]), .C(car_cnt[8]), .D(car_cnt[7]), 
         .Z(n16793)) /* synthesis lut_function=(A (B+(C))+!A (B+(C (D)))) */ ;
    defparam i15208_4_lut.init = 16'hfcec;
    LUT4 i428_4_lut (.A(f2n_irx_temp[4]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_77), .Z(irx_o_c[4])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i428_4_lut.init = 16'hbaaa;
    LUT4 i4_4_lut_adj_43 (.A(sw5_f2n[5]), .B(sw1_f2n[5]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_43.init = 16'hf531;
    LUT4 i1_2_lut_4_lut_adj_44 (.A(n14_adj_71), .B(n30146), .C(con[1]), 
         .D(con[0]), .Z(n30349)) /* synthesis lut_function=(!(A+!(B (C (D))))) */ ;
    defparam i1_2_lut_4_lut_adj_44.init = 16'h4000;
    CCU2D wd_disable_402_to_mcu_1472_add_4_11 (.A0(n87_adj_605[9]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(wd_disable_402_to_mcu[10]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28870), .S0(n50_adj_603[9]), .S1(n50_adj_603[10]));
    defparam wd_disable_402_to_mcu_1472_add_4_11.INIT0 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_11.INIT1 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_11.INJECT1_0 = "NO";
    defparam wd_disable_402_to_mcu_1472_add_4_11.INJECT1_1 = "NO";
    CCU2D wd_disable_402_to_mcu_1472_add_4_9 (.A0(n87_adj_605[7]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n87_adj_605[8]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28869), .COUT(n28870), .S0(n50_adj_603[7]), .S1(n50_adj_603[8]));
    defparam wd_disable_402_to_mcu_1472_add_4_9.INIT0 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_9.INIT1 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_9.INJECT1_0 = "NO";
    defparam wd_disable_402_to_mcu_1472_add_4_9.INJECT1_1 = "NO";
    CCU2D wd_disable_402_to_mcu_1472_add_4_7 (.A0(n87_adj_605[5]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n87_adj_605[6]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28868), .COUT(n28869), .S0(n50_adj_603[5]), .S1(n50_adj_603[6]));
    defparam wd_disable_402_to_mcu_1472_add_4_7.INIT0 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_7.INIT1 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_7.INJECT1_0 = "NO";
    defparam wd_disable_402_to_mcu_1472_add_4_7.INJECT1_1 = "NO";
    CCU2D wd_disable_402_to_mcu_1472_add_4_5 (.A0(n87_adj_605[3]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n87_adj_605[4]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28867), .COUT(n28868), .S0(n50_adj_603[3]), .S1(n50_adj_603[4]));
    defparam wd_disable_402_to_mcu_1472_add_4_5.INIT0 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_5.INIT1 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_5.INJECT1_0 = "NO";
    defparam wd_disable_402_to_mcu_1472_add_4_5.INJECT1_1 = "NO";
    CCU2D wd_disable_402_to_mcu_1472_add_4_3 (.A0(n87_adj_605[1]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n87_adj_605[2]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28866), .COUT(n28867), .S0(n50_adj_603[1]), .S1(n50_adj_603[2]));
    defparam wd_disable_402_to_mcu_1472_add_4_3.INIT0 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_3.INIT1 = 16'hfaaa;
    defparam wd_disable_402_to_mcu_1472_add_4_3.INJECT1_0 = "NO";
    defparam wd_disable_402_to_mcu_1472_add_4_3.INJECT1_1 = "NO";
    CCU2D wd_disable_402_to_mcu_1472_add_4_1 (.A0(tx_baud_adj_282[2]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(clock_count15_reg0), 
          .B1(n16410), .C1(n87_adj_605[0]), .D1(tx_baud_adj_282[2]), .COUT(n28866), 
          .S1(n50_adj_603[0]));
    defparam wd_disable_402_to_mcu_1472_add_4_1.INIT0 = 16'hF000;
    defparam wd_disable_402_to_mcu_1472_add_4_1.INIT1 = 16'hd2d2;
    defparam wd_disable_402_to_mcu_1472_add_4_1.INJECT1_0 = "NO";
    defparam wd_disable_402_to_mcu_1472_add_4_1.INJECT1_1 = "NO";
    CCU2D clock_count_1473_add_4_17 (.A0(clock_count[15]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(tx_baud_adj_282[2]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28865), .S0(n70_adj_600[15]));
    defparam clock_count_1473_add_4_17.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_17.INIT1 = 16'h0000;
    defparam clock_count_1473_add_4_17.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_17.INJECT1_1 = "NO";
    CCU2D clock_count_1473_add_4_15 (.A0(n122[13]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[14]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28864), .COUT(n28865), .S0(n70_adj_600[13]), .S1(n70_adj_600[14]));
    defparam clock_count_1473_add_4_15.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_15.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_15.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_15.INJECT1_1 = "NO";
    LUT4 i28074_3_lut (.A(n30044), .B(rs232tx), .C(chnum[2]), .Z(n11_adj_132)) /* synthesis lut_function=((B+(C))+!A) */ ;
    defparam i28074_3_lut.init = 16'hfdfd;
    LUT4 i27921_2_lut (.A(chnum[1]), .B(chnum[3]), .Z(n29683)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i27921_2_lut.init = 16'h9999;
    LUT4 i1_2_lut_4_lut_adj_45 (.A(n14_adj_71), .B(n30146), .C(con[1]), 
         .D(n17_adj_428[4]), .Z(n4663)) /* synthesis lut_function=(A (D)+!A !(B (C+!(D))+!B !(D))) */ ;
    defparam i1_2_lut_4_lut_adj_45.init = 16'hbf00;
    CCU2D clock_count_1473_add_4_13 (.A0(n122[11]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[12]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28863), .COUT(n28864), .S0(n70_adj_600[11]), .S1(n70_adj_600[12]));
    defparam clock_count_1473_add_4_13.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_13.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_13.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_13.INJECT1_1 = "NO";
    CCU2D clock_count_1473_add_4_11 (.A0(n122[9]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[10]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28862), .COUT(n28863), .S0(n70_adj_600[9]), .S1(n70_adj_600[10]));
    defparam clock_count_1473_add_4_11.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_11.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_11.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_11.INJECT1_1 = "NO";
    CCU2D clock_count_1473_add_4_9 (.A0(n122[7]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[8]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28861), .COUT(n28862), .S0(n70_adj_600[7]), .S1(n70_adj_600[8]));
    defparam clock_count_1473_add_4_9.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_9.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_9.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_9.INJECT1_1 = "NO";
    CCU2D clock_count_1473_add_4_7 (.A0(n122[5]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[6]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28860), .COUT(n28861), .S0(n70_adj_600[5]), .S1(n70_adj_600[6]));
    defparam clock_count_1473_add_4_7.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_7.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_7.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_7.INJECT1_1 = "NO";
    LUT4 i2_4_lut_adj_46 (.A(sw6_f2n[5]), .B(sw7_f2n[5]), .C(n30177), 
         .D(n30176), .Z(n10_adj_81)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_46.init = 16'hf531;
    LUT4 i28077_4_lut (.A(chnum[2]), .B(rs232tx), .C(n29683), .D(chnum[0]), 
         .Z(n8_adj_133)) /* synthesis lut_function=(A+(B+(C+!(D)))) */ ;
    defparam i28077_4_lut.init = 16'hfeff;
    CCU2D clock_count_1473_add_4_5 (.A0(n122[3]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[4]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28859), .COUT(n28860), .S0(n70_adj_600[3]), .S1(n70_adj_600[4]));
    defparam clock_count_1473_add_4_5.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_5.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_5.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_5.INJECT1_1 = "NO";
    CCU2D clock_count_1473_add_4_3 (.A0(n122[1]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[2]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28858), .COUT(n28859), .S0(n70_adj_600[1]), .S1(n70_adj_600[2]));
    defparam clock_count_1473_add_4_3.INIT0 = 16'hfaaa;
    defparam clock_count_1473_add_4_3.INIT1 = 16'hfaaa;
    defparam clock_count_1473_add_4_3.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_3.INJECT1_1 = "NO";
    LUT4 i6_4_lut_adj_47 (.A(sw2_f2n[5]), .B(n12), .C(n1386[5]), .D(n30181), 
         .Z(n14_adj_70)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_47.init = 16'hc040;
    LUT4 i15138_2_lut_rep_154 (.A(ir_com_o[3]), .B(getting_com), .Z(n30185)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15138_2_lut_rep_154.init = 16'heeee;
    CCU2D clock_count_1473_add_4_1 (.A0(tx_baud_adj_282[2]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(n122[0]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .COUT(n28858), .S1(n70_adj_600[0]));
    defparam clock_count_1473_add_4_1.INIT0 = 16'hF000;
    defparam clock_count_1473_add_4_1.INIT1 = 16'h0555;
    defparam clock_count_1473_add_4_1.INJECT1_0 = "NO";
    defparam clock_count_1473_add_4_1.INJECT1_1 = "NO";
    CCU2D car_cnt_1475_add_4_11 (.A0(car_cnt[9]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(tx_baud_adj_282[2]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28857), .S0(n46[9]));
    defparam car_cnt_1475_add_4_11.INIT0 = 16'hfaaa;
    defparam car_cnt_1475_add_4_11.INIT1 = 16'h0000;
    defparam car_cnt_1475_add_4_11.INJECT1_0 = "NO";
    defparam car_cnt_1475_add_4_11.INJECT1_1 = "NO";
    LUT4 i1_2_lut_4_lut_adj_48 (.A(n14_adj_71), .B(n30146), .C(con[1]), 
         .D(n30344), .Z(n3607)) /* synthesis lut_function=(A (D)+!A !(B (C+!(D))+!B !(D))) */ ;
    defparam i1_2_lut_4_lut_adj_48.init = 16'hbf00;
    CCU2D car_cnt_1475_add_4_9 (.A0(car_cnt[7]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(car_cnt[8]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28856), .COUT(n28857), .S0(n46[7]), .S1(n46[8]));
    defparam car_cnt_1475_add_4_9.INIT0 = 16'hfaaa;
    defparam car_cnt_1475_add_4_9.INIT1 = 16'hfaaa;
    defparam car_cnt_1475_add_4_9.INJECT1_0 = "NO";
    defparam car_cnt_1475_add_4_9.INJECT1_1 = "NO";
    LUT4 i15079_2_lut_3_lut_4_lut (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[0]), .D(ir_402_com_i[2]), .Z(n482[1])) /* synthesis lut_function=(!(A+(B+((D)+!C)))) */ ;
    defparam i15079_2_lut_3_lut_4_lut.init = 16'h0010;
    LUT4 i28061_2_lut_3_lut_4_lut (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[0]), .D(ir_402_com_i[2]), .Z(n482[0])) /* synthesis lut_function=(!(A+(B+(C+(D))))) */ ;
    defparam i28061_2_lut_3_lut_4_lut.init = 16'h0001;
    CCU2D car_cnt_1475_add_4_7 (.A0(car_cnt[5]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(car_cnt[6]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28855), .COUT(n28856), .S0(n46[5]), .S1(n46[6]));
    defparam car_cnt_1475_add_4_7.INIT0 = 16'hfaaa;
    defparam car_cnt_1475_add_4_7.INIT1 = 16'hfaaa;
    defparam car_cnt_1475_add_4_7.INJECT1_0 = "NO";
    defparam car_cnt_1475_add_4_7.INJECT1_1 = "NO";
    CCU2D car_cnt_1475_add_4_5 (.A0(car_cnt[3]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(car_cnt[4]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28854), .COUT(n28855), .S0(n46[3]), .S1(n46[4]));
    defparam car_cnt_1475_add_4_5.INIT0 = 16'hfaaa;
    defparam car_cnt_1475_add_4_5.INIT1 = 16'hfaaa;
    defparam car_cnt_1475_add_4_5.INJECT1_0 = "NO";
    defparam car_cnt_1475_add_4_5.INJECT1_1 = "NO";
    CCU2D car_cnt_1475_add_4_3 (.A0(car_cnt[1]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(car_cnt[2]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .CIN(n28853), .COUT(n28854), .S0(n46[1]), .S1(n46[2]));
    defparam car_cnt_1475_add_4_3.INIT0 = 16'hfaaa;
    defparam car_cnt_1475_add_4_3.INIT1 = 16'hfaaa;
    defparam car_cnt_1475_add_4_3.INJECT1_0 = "NO";
    defparam car_cnt_1475_add_4_3.INJECT1_1 = "NO";
    LUT4 i7_4_lut_adj_49 (.A(n1381[5]), .B(n14_adj_70), .C(n10_adj_81), 
         .D(n1388[5]), .Z(f2n_irx_temp[5])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_49.init = 16'h8000;
    CCU2D car_cnt_1475_add_4_1 (.A0(tx_baud_adj_282[2]), .B0(tx_baud_adj_282[2]), 
          .C0(tx_baud_adj_282[2]), .D0(tx_baud_adj_282[2]), .A1(car_cnt[0]), 
          .B1(tx_baud_adj_282[2]), .C1(tx_baud_adj_282[2]), .D1(tx_baud_adj_282[2]), 
          .COUT(n28853), .S1(n46[0]));
    defparam car_cnt_1475_add_4_1.INIT0 = 16'hF000;
    defparam car_cnt_1475_add_4_1.INIT1 = 16'h0555;
    defparam car_cnt_1475_add_4_1.INJECT1_0 = "NO";
    defparam car_cnt_1475_add_4_1.INJECT1_1 = "NO";
    LUT4 i28058_2_lut_3_lut_4_lut (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[0]), .D(ir_402_com_i[2]), .Z(n482[4])) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;
    defparam i28058_2_lut_3_lut_4_lut.init = 16'h0100;
    LUT4 i1_3_lut (.A(chnum[3]), .B(n30140), .C(chnum[2]), .Z(n29490)) /* synthesis lut_function=(A (B+(C))+!A (B+!(C))) */ ;
    defparam i1_3_lut.init = 16'heded;
    LUT4 i15074_2_lut_3_lut_4_lut (.A(ir_402_com_i[1]), .B(ir_402_com_i[3]), 
         .C(ir_402_com_i[0]), .D(ir_402_com_i[2]), .Z(n482[5])) /* synthesis lut_function=(!(A+(B+!(C (D))))) */ ;
    defparam i15074_2_lut_3_lut_4_lut.init = 16'h1000;
    LUT4 i1_2_lut_4_lut_adj_50 (.A(sp_com_data[1]), .B(sp_com_data[3]), 
         .C(sp_com_data[2]), .D(sp_com_data[0]), .Z(n29387)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i1_2_lut_4_lut_adj_50.init = 16'h8000;
    LUT4 i4_4_lut_adj_51 (.A(sw5_f2n[4]), .B(sw1_f2n[4]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_129)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_51.init = 16'hf531;
    LUT4 i2_3_lut_rep_152 (.A(sp_com_data[1]), .B(sp_com_data[3]), .C(sp_com_data[2]), 
         .Z(n30183)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i2_3_lut_rep_152.init = 16'h8080;
    LUT4 i2_2_lut_adj_52 (.A(irx_o_c[2]), .B(irx_o_c[4]), .Z(n10_adj_117)) /* synthesis lut_function=(A (B)) */ ;
    defparam i2_2_lut_adj_52.init = 16'h8888;
    LUT4 i2_4_lut_adj_53 (.A(sw6_f2n[4]), .B(sw7_f2n[4]), .C(n30177), 
         .D(n30176), .Z(n10_adj_134)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_53.init = 16'hf531;
    LUT4 i2_2_lut_3_lut_4_lut (.A(def_voltag[1]), .B(ir_402x_i_c[1]), .C(ir_402x_i_c[6]), 
         .D(def_voltag[6]), .Z(n11_adj_138)) /* synthesis lut_function=(!(A (B+(C (D)+!C !(D)))+!A ((C (D)+!C !(D))+!B))) */ ;
    defparam i2_2_lut_3_lut_4_lut.init = 16'h0660;
    LUT4 i6_4_lut_adj_54 (.A(irx_o_c[3]), .B(irx_o_c[1]), .C(irx_o_c[5]), 
         .D(irx_o_c[7]), .Z(n14_adj_88)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i6_4_lut_adj_54.init = 16'h8000;
    LUT4 i6_4_lut_adj_55 (.A(sw2_f2n[4]), .B(n12_adj_129), .C(n1386[4]), 
         .D(n30181), .Z(n14_adj_128)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_55.init = 16'hc040;
    LUT4 xor_199_i2_2_lut_rep_151 (.A(def_voltag[1]), .B(ir_402x_i_c[1]), 
         .Z(n30182)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i2_2_lut_rep_151.init = 16'h6666;
    LUT4 or_520_i4_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[3]), 
         .Z(n1386[3])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i4_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i7_4_lut_adj_56 (.A(n1381[4]), .B(n14_adj_128), .C(n10_adj_134), 
         .D(n1388[4]), .Z(f2n_irx_temp[4])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_56.init = 16'h8000;
    LUT4 or_520_i3_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[2]), 
         .Z(n1386[2])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i3_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i1_2_lut_3_lut_4_lut_adj_57 (.A(rstn_c), .B(n30147), .C(n17_adj_428[5]), 
         .D(n30132), .Z(n4150)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_57.init = 16'h8000;
    LUT4 i7_4_lut_adj_58 (.A(irx_o_c[0]), .B(n14_adj_88), .C(n10_adj_117), 
         .D(irx_o_c[6]), .Z(ir_all_out_c)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_58.init = 16'h8000;
    LUT4 i28072_2_lut (.A(rs232tx), .B(n30048), .Z(n10_adj_131)) /* synthesis lut_function=(A+!(B)) */ ;
    defparam i28072_2_lut.init = 16'hbbbb;
    LUT4 or_520_i2_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[1]), 
         .Z(n1386[1])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i2_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_520_i1_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[0]), 
         .Z(n1386[0])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i1_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_520_i5_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[4]), 
         .Z(n1386[4])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i5_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_520_i6_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[5]), 
         .Z(n1386[5])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i6_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i4_4_lut_adj_59 (.A(sw5_f2n[3]), .B(sw1_f2n[3]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_84)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_59.init = 16'hf531;
    LUT4 or_520_i7_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[6]), 
         .Z(n1386[6])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i7_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_520_i8_2_lut_3_lut (.A(def_voltag[3]), .B(ir_402x_i_c[3]), .C(sw3_f2n[7]), 
         .Z(n1386[7])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_520_i8_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i1_2_lut_3_lut_4_lut_adj_60 (.A(def_voltag[3]), .B(ir_402x_i_c[3]), 
         .C(ir_402x_i_c[2]), .D(def_voltag[2]), .Z(n30352)) /* synthesis lut_function=(!(A (B+(C (D)+!C !(D)))+!A ((C (D)+!C !(D))+!B))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_60.init = 16'h0660;
    LUT4 xor_199_i3_2_lut_rep_150 (.A(def_voltag[2]), .B(ir_402x_i_c[2]), 
         .Z(n30181)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i3_2_lut_rep_150.init = 16'h6666;
    LUT4 or_522_i2_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[1]), 
         .Z(n1388[1])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i2_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i1_2_lut_3_lut_4_lut_adj_61 (.A(rstn_c), .B(n30147), .C(n17_adj_428[4]), 
         .D(n30132), .Z(n4151)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_61.init = 16'h8000;
    LUT4 or_522_i4_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[3]), 
         .Z(n1388[3])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i4_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_522_i6_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[5]), 
         .Z(n1388[5])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i6_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i437_4_lut (.A(f2n_irx_temp[7]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_80), .Z(irx_o_c[7])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i437_4_lut.init = 16'hbaaa;
    LUT4 or_522_i7_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[6]), 
         .Z(n1388[6])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i7_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_522_i5_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[4]), 
         .Z(n1388[4])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i5_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i2_4_lut_adj_62 (.A(sw6_f2n[3]), .B(sw7_f2n[3]), .C(n30177), 
         .D(n30176), .Z(n10_adj_87)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_62.init = 16'hf531;
    LUT4 or_522_i8_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[7]), 
         .Z(n1388[7])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i8_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i6_4_lut_adj_63 (.A(sw2_f2n[3]), .B(n12_adj_84), .C(n1386[3]), 
         .D(n30181), .Z(n14_adj_82)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_63.init = 16'hc040;
    LUT4 i7_4_lut_adj_64 (.A(n1381[3]), .B(n14_adj_82), .C(n10_adj_87), 
         .D(n1388[3]), .Z(f2n_irx_temp[3])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_64.init = 16'h8000;
    LUT4 or_522_i1_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[0]), 
         .Z(n1388[0])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i1_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_522_i3_2_lut_3_lut (.A(def_voltag[4]), .B(ir_402x_i_c[4]), .C(sw4_f2n[2]), 
         .Z(n1388[2])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_522_i3_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i1_2_lut_rep_94_4_lut (.A(rs232_402x_tx_c[3]), .B(n10_adj_126), 
         .C(rs232_402x_tx_c[4]), .D(rs232_tcp_rx_c_c), .Z(n30125)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i1_2_lut_rep_94_4_lut.init = 16'h8000;
    LUT4 i5_3_lut_rep_99 (.A(rs232_402x_tx_c[3]), .B(n10_adj_126), .C(rs232_402x_tx_c[4]), 
         .Z(n30130)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i5_3_lut_rep_99.init = 16'h8080;
    LUT4 xor_199_i5_2_lut_rep_149 (.A(def_voltag[4]), .B(ir_402x_i_c[4]), 
         .Z(n30180)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i5_2_lut_rep_149.init = 16'h6666;
    LUT4 i1_2_lut_4_lut_adj_65 (.A(sp_com_data[3]), .B(sp_com_data[1]), 
         .C(sp_com_data[2]), .D(sp_com_data[0]), .Z(n29382)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i1_2_lut_4_lut_adj_65.init = 16'hfffe;
    LUT4 i4_4_lut_adj_66 (.A(sw5_f2n[2]), .B(sw1_f2n[2]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_125)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_66.init = 16'hf531;
    LUT4 i2_4_lut_adj_67 (.A(sw6_f2n[2]), .B(sw7_f2n[2]), .C(n30177), 
         .D(n30176), .Z(n10_adj_127)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_67.init = 16'hf531;
    LUT4 i1_2_lut_adj_68 (.A(con[1]), .B(con[0]), .Z(n29032)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1_2_lut_adj_68.init = 16'h9999;
    LUT4 i27972_3_lut (.A(rs232_402_all_back[1]), .B(rs232_402_all_back[2]), 
         .C(rs232_402_all_back[0]), .Z(n2820)) /* synthesis lut_function=(!(A+(B+(C)))) */ ;
    defparam i27972_3_lut.init = 16'h0101;
    LUT4 i6_4_lut_adj_69 (.A(sw2_f2n[2]), .B(n12_adj_125), .C(n1386[2]), 
         .D(n30181), .Z(n14_adj_124)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_69.init = 16'hc040;
    LUT4 i7_4_lut_adj_70 (.A(n1381[2]), .B(n14_adj_124), .C(n10_adj_127), 
         .D(n1388[2]), .Z(f2n_irx_temp[2])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_70.init = 16'h8000;
    LUT4 i2_3_lut_rep_148 (.A(sp_com_data[3]), .B(sp_com_data[1]), .C(sp_com_data[2]), 
         .Z(n30179)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i2_3_lut_rep_148.init = 16'hfefe;
    LUT4 i2_4_lut_adj_71 (.A(car_cnt[6]), .B(n29501), .C(car_cnt[5]), 
         .D(car_cnt[4]), .Z(n29019)) /* synthesis lut_function=(A+(B (C+(D))+!B (C))) */ ;
    defparam i2_4_lut_adj_71.init = 16'hfefa;
    LUT4 select_1300_Select_1_i3_2_lut (.A(con[1]), .B(n17_adj_428[5]), 
         .Z(n3_adj_72)) /* synthesis lut_function=(A (B)) */ ;
    defparam select_1300_Select_1_i3_2_lut.init = 16'h8888;
    LUT4 i1_4_lut (.A(rstn_c), .B(n30147), .C(n17_adj_428[6]), .D(n30132), 
         .Z(n4210)) /* synthesis lut_function=(A ((C (D))+!B)) */ ;
    defparam i1_4_lut.init = 16'ha222;
    LUT4 i15201_4_lut (.A(n29019), .B(car_cnt[9]), .C(car_cnt[8]), .D(car_cnt[7]), 
         .Z(n4438)) /* synthesis lut_function=(A (B (C+(D)))+!A (B (C))) */ ;
    defparam i15201_4_lut.init = 16'hc8c0;
    LUT4 i14910_2_lut (.A(n46[0]), .B(n4438), .Z(n58_adj_596[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14910_2_lut.init = 16'heeee;
    LUT4 select_1300_Select_2_i3_2_lut (.A(con[2]), .B(n17_adj_428[5]), 
         .Z(n3_adj_73)) /* synthesis lut_function=(A (B)) */ ;
    defparam select_1300_Select_2_i3_2_lut.init = 16'h8888;
    LUT4 or_515_i5_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[4]), 
         .Z(n1381[4])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i5_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_515_i7_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[6]), 
         .Z(n1381[6])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i7_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_515_i1_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[0]), 
         .Z(n1381[0])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i1_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i4_4_lut_adj_72 (.A(sw5_f2n[1]), .B(sw1_f2n[1]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_122)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_72.init = 16'hf531;
    LUT4 or_515_i2_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[1]), 
         .Z(n1381[1])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i2_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i2_4_lut_adj_73 (.A(sw6_f2n[1]), .B(sw7_f2n[1]), .C(n30177), 
         .D(n30176), .Z(n10_adj_123)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_73.init = 16'hf531;
    LUT4 or_515_i4_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[3]), 
         .Z(n1381[3])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i4_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i6_4_lut_adj_74 (.A(sw2_f2n[1]), .B(n12_adj_122), .C(n1386[1]), 
         .D(n30181), .Z(n14_adj_120)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_74.init = 16'hc040;
    LUT4 or_515_i8_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[7]), 
         .Z(n1381[7])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i8_2_lut_3_lut.init = 16'h6f6f;
    LUT4 or_515_i3_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[2]), 
         .Z(n1381[2])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i3_2_lut_3_lut.init = 16'h6f6f;
    LUT4 i7_4_lut_adj_75 (.A(n1381[1]), .B(n14_adj_120), .C(n10_adj_123), 
         .D(n1388[1]), .Z(f2n_irx_temp[1])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_75.init = 16'h8000;
    LUT4 or_515_i6_2_lut_3_lut (.A(def_voltag[0]), .B(ir_402x_i_c[0]), .C(sw0_f2n[5]), 
         .Z(n1381[5])) /* synthesis lut_function=(!(A (B (C))+!A !(B+!(C)))) */ ;
    defparam or_515_i6_2_lut_3_lut.init = 16'h6f6f;
    LUT4 xor_199_i1_2_lut_rep_147 (.A(def_voltag[0]), .B(ir_402x_i_c[0]), 
         .Z(n30178)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i1_2_lut_rep_147.init = 16'h6666;
    LUT4 xor_199_i7_2_lut_rep_146 (.A(def_voltag[6]), .B(ir_402x_i_c[6]), 
         .Z(n30177)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i7_2_lut_rep_146.init = 16'h6666;
    LUT4 i3_3_lut_4_lut (.A(def_voltag[7]), .B(ir_402x_i_c[7]), .C(ir_def_lv[0]), 
         .D(ir_defX_in[0]), .Z(n12_adj_137)) /* synthesis lut_function=(!(A (B+(C (D)+!C !(D)))+!A ((C (D)+!C !(D))+!B))) */ ;
    defparam i3_3_lut_4_lut.init = 16'h0660;
    LUT4 xor_199_i8_2_lut_rep_145 (.A(def_voltag[7]), .B(ir_402x_i_c[7]), 
         .Z(n30176)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i8_2_lut_rep_145.init = 16'h6666;
    INV i2833 (.A(rs232_tcp_rx_c_c), .Z(n4415));
    LUT4 i2_3_lut_4_lut_adj_76 (.A(rs232_tcp_rx_c_c), .B(n30130), .C(n30166), 
         .D(n30155), .Z(n29096)) /* synthesis lut_function=(!(A (B+((D)+!C))+!A ((D)+!C))) */ ;
    defparam i2_3_lut_4_lut_adj_76.init = 16'h0070;
    LUT4 i28088_3_lut_rep_90_4_lut (.A(rs232_tcp_rx_c_c), .B(n30130), .C(rx_step[3]), 
         .D(n30134), .Z(n30121)) /* synthesis lut_function=(!(A (B+(C+(D)))+!A (C+(D)))) */ ;
    defparam i28088_3_lut_rep_90_4_lut.init = 16'h0007;
    LUT4 i14842_2_lut (.A(wd_disable_402_to_mcu[10]), .B(clock_count15_reg1), 
         .Z(n16410)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14842_2_lut.init = 16'heeee;
    LUT4 i4_2_lut (.A(rclk_r2), .B(rclk_r1), .Z(n13)) /* synthesis lut_function=(A (B)) */ ;
    defparam i4_2_lut.init = 16'h8888;
    LUT4 i27947_2_lut (.A(rclk_r2), .B(rclk_r1), .Z(n30328)) /* synthesis lut_function=(!(A+(B))) */ ;
    defparam i27947_2_lut.init = 16'h1111;
    LUT4 i27952_2_lut (.A(rdy), .B(empty), .Z(n121)) /* synthesis lut_function=(!(A+(B))) */ ;
    defparam i27952_2_lut.init = 16'h1111;
    LUT4 m1_lut (.Z(n30301)) /* synthesis lut_function=1, syn_instantiated=1 */ ;
    defparam m1_lut.init = 16'hffff;
    LUT4 i1_4_lut_adj_77 (.A(n1510[7]), .B(n30349), .C(n17_adj_428[6]), 
         .D(n5_adj_136), .Z(n30355)) /* synthesis lut_function=(A (B)+!A (B+!(C+(D)))) */ ;
    defparam i1_4_lut_adj_77.init = 16'hcccd;
    LUT4 i1_4_lut_adj_78 (.A(n17_adj_428[0]), .B(getting_com), .C(n17_adj_428[2]), 
         .D(n30355), .Z(n30350)) /* synthesis lut_function=(!(A+(B (C+!(D))))) */ ;
    defparam i1_4_lut_adj_78.init = 16'h1511;
    FD1S3AY ack_480 (.D(n29858), .CK(clk_c), .Q(ack2mcu_c));
    defparam ack_480.GSR = "DISABLED";
    LUT4 i28053_4_lut (.A(getting_com), .B(n30350), .C(con[0]), .D(n4643), 
         .Z(n30351)) /* synthesis lut_function=(!(A (B)+!A (B+(C+!(D))))) */ ;
    defparam i28053_4_lut.init = 16'h2322;
    LUT4 m0_lut (.Z(n30300)) /* synthesis lut_function=0, syn_instantiated=1 */ ;
    defparam m0_lut.init = 16'h0000;
    LUT4 i47_2_lut (.A(rx_int0), .B(rx_int1), .Z(rden)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i47_2_lut.init = 16'h2222;
    LUT4 i434_4_lut (.A(f2n_irx_temp[6]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_79), .Z(irx_o_c[6])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i434_4_lut.init = 16'hbaaa;
    LUT4 xor_199_i6_2_lut (.A(def_voltag[5]), .B(ir_402x_i_c[5]), .Z(f2n_irx_level[5])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam xor_199_i6_2_lut.init = 16'h6666;
    LUT4 i4_4_lut_adj_79 (.A(sw5_f2n[0]), .B(sw1_f2n[0]), .C(f2n_irx_level[5]), 
         .D(n30182), .Z(n12_adj_85)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i4_4_lut_adj_79.init = 16'hf531;
    LUT4 i2_4_lut_adj_80 (.A(sw6_f2n[0]), .B(sw7_f2n[0]), .C(n30177), 
         .D(n30176), .Z(n10_adj_86)) /* synthesis lut_function=(A (B (C (D))+!B (C))+!A ((D)+!B)) */ ;
    defparam i2_4_lut_adj_80.init = 16'hf531;
    LUT4 i6_4_lut_adj_81 (.A(sw2_f2n[0]), .B(n12_adj_85), .C(n1386[0]), 
         .D(n30181), .Z(n14_adj_83)) /* synthesis lut_function=(A (B (C (D)))+!A (B (C))) */ ;
    defparam i6_4_lut_adj_81.init = 16'hc040;
    LUT4 i25_2_lut (.A(wren), .B(full), .Z(wren_w)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i25_2_lut.init = 16'h2222;
    LUT4 i7_4_lut_adj_82 (.A(n1381[0]), .B(n14_adj_83), .C(n10_adj_86), 
         .D(n1388[0]), .Z(f2n_irx_temp[0])) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut_adj_82.init = 16'h8000;
    ir_car_detect_U8 ir_car_detect1 (.clk_c(clk_c), .\f2n_irx_temp[1] (f2n_irx_temp[1]), 
            .ir_car_null(ir_car_null_adj_74), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    ir_car_detect_U7 ir_car_detect2 (.clk_c(clk_c), .\f2n_irx_temp[2] (f2n_irx_temp[2]), 
            .ir_car_null(ir_car_null_adj_75), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    check_IR_defv_U13 check_IR_defv4 (.\clock_count[15] (clock_count[15]), 
            .\def_voltag[4] (def_voltag[4]), .ir_402x_i_c_4(ir_402x_i_c[4]), 
            .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    \ir_def_lv(BIT_NUM=3'b01,IO_NUM=7'b01)  ir_def_lv_inst1 (.ir_defX_in({ir_defX_in}), 
            .clk_c(clk_c), .ir_def_lv({ir_def_lv})) /* synthesis syn_module_defined=1 */ ;
    VLO i1 (.Z(tx_baud_adj_282[2]));
    check_IR_defv_U14 check_IR_defv3 (.\clock_count[15] (clock_count[15]), 
            .\def_voltag[3] (def_voltag[3]), .ir_402x_i_c_3(ir_402x_i_c[3]), 
            .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    LUT4 i1_2_lut_4_lut_adj_83 (.A(sp_com_data[6]), .B(sp_com_data[7]), 
         .C(sp_com_data[5]), .D(sp_com_data[4]), .Z(n1829)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;
    defparam i1_2_lut_4_lut_adj_83.init = 16'h0008;
    LUT4 i2_3_lut_rep_131 (.A(sp_com_data[6]), .B(sp_com_data[7]), .C(sp_com_data[5]), 
         .Z(n30162)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i2_3_lut_rep_131.init = 16'h0808;
    ir_car_detect_U9 ir_car_detect0 (.clk_c(clk_c), .\f2n_irx_temp[0] (f2n_irx_temp[0]), 
            .ir_car_null(ir_car_null), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    ir_car_detect_U5 ir_car_detect4 (.GND_net(tx_baud_adj_282[2]), .clk_c(clk_c), 
            .\f2n_irx_temp[4] (f2n_irx_temp[4]), .ir_car_null(ir_car_null_adj_77)) /* synthesis syn_module_defined=1 */ ;
    ir_car_detect ir_car_detect7 (.clk_c(clk_c), .\f2n_irx_temp[7] (f2n_irx_temp[7]), 
            .ir_car_null(ir_car_null_adj_80), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    LUT4 i15134_3_lut_4_lut (.A(car_cnt[1]), .B(car_cnt[2]), .C(car_cnt[3]), 
         .D(car_cnt[0]), .Z(n16703)) /* synthesis lut_function=(A (C)+!A (B (C)+!B (C (D)))) */ ;
    defparam i15134_3_lut_4_lut.init = 16'hf0e0;
    LUT4 i1_2_lut_3_lut_adj_84 (.A(car_cnt[1]), .B(car_cnt[2]), .C(car_cnt[3]), 
         .Z(n29501)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i1_2_lut_3_lut_adj_84.init = 16'hfefe;
    LUT4 i1_2_lut_3_lut_4_lut_adj_85 (.A(ir_com_o[2]), .B(ir_com_o[1]), 
         .C(ir_com_o[0]), .D(n30185), .Z(n2679)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_85.init = 16'h0002;
    LUT4 i1_2_lut_3_lut_4_lut_adj_86 (.A(ir_com_o[2]), .B(ir_com_o[1]), 
         .C(ir_com_o[0]), .D(n30185), .Z(n2681)) /* synthesis lut_function=(!((B+((D)+!C))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_86.init = 16'h0020;
    check_IR_defv_U12 check_IR_defv5 (.\clock_count[15] (clock_count[15]), 
            .ir_402x_i_c_5(ir_402x_i_c[5]), .\def_voltag[5] (def_voltag[5]), 
            .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    ir_car_detect_U4 ir_car_detect5 (.clk_c(clk_c), .\f2n_irx_temp[5] (f2n_irx_temp[5]), 
            .ir_car_null(ir_car_null_adj_78), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    pc2mcu_uart_top pc2mcu_uart_top1 (.clk_c(clk_c), .n30301(n30301), .n30300(n30300), 
            .rs232_send_to_mcu_changebaud(rs232_send_to_mcu_changebaud), .GND_net(tx_baud_adj_282[2]), 
            .n1175(n1175), .to_db9_baud({to_db9_baud}), .rx_baud_count({rx_baud_count_adj_299}), 
            .n134(n30348), .VCC_net(VCC_net)) /* synthesis syn_module_defined=1 */ ;
    LUT4 i425_4_lut (.A(f2n_irx_temp[3]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_76), .Z(irx_o_c[3])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i425_4_lut.init = 16'hbaaa;
    LUT4 i422_4_lut (.A(f2n_irx_temp[2]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_75), .Z(irx_o_c[2])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i422_4_lut.init = 16'hbaaa;
    LUT4 i419_4_lut (.A(f2n_irx_temp[1]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null_adj_74), .Z(irx_o_c[1])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i419_4_lut.init = 16'hbaaa;
    check_IR_defv_U11 check_IR_defv6 (.ir_402x_i_c_6(ir_402x_i_c[6]), .\clock_count[15] (clock_count[15]), 
            .\def_voltag[6] (def_voltag[6]), .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    ir_car_detect_U6 ir_car_detect3 (.clk_c(clk_c), .\f2n_irx_temp[3] (f2n_irx_temp[3]), 
            .ir_car_null(ir_car_null_adj_76), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    check_IR_defv_U15 check_IR_defv2 (.ir_402x_i_c_2(ir_402x_i_c[2]), .\def_voltag[2] (def_voltag[2]), 
            .\clock_count[15] (clock_count[15]), .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    LUT4 i1_2_lut_3_lut_adj_87 (.A(con[1]), .B(con[0]), .C(con[2]), .Z(n29036)) /* synthesis lut_function=(A (C)+!A (B (C)+!B !(C))) */ ;
    defparam i1_2_lut_3_lut_adj_87.init = 16'he1e1;
    LUT4 i1_3_lut_4_lut (.A(con[1]), .B(con[0]), .C(con[2]), .D(con[3]), 
         .Z(n29030)) /* synthesis lut_function=(A (D)+!A (B (D)+!B (C (D)+!C !(D)))) */ ;
    defparam i1_3_lut_4_lut.init = 16'hfe01;
    LUT4 i416_4_lut (.A(f2n_irx_temp[0]), .B(n16793), .C(ir_car_care), 
         .D(ir_car_null), .Z(irx_o_c[0])) /* synthesis lut_function=(A+!(B+!(C (D)))) */ ;
    defparam i416_4_lut.init = 16'hbaaa;
    check_IR_defv_U17 check_IR_defv0 (.ir_402x_i_c_0(ir_402x_i_c[0]), .\clock_count[15] (clock_count[15]), 
            .\def_voltag[0] (def_voltag[0]), .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    hc74595 hc595_u (.qb0(qb0), .srclk_c(srclk_c), .qa0(qa0), .qc0(qc0), 
            .qd0(qd0), .qe0(qe0), .\con[5] (con[5]), .rclk_c(rclk_c), 
            .\con[6] (con[6]), .\con[7] (con[7]), .VCC_net(VCC_net), .GND_net(tx_baud_adj_282[2]), 
            .ser_c(ser_c)) /* synthesis syn_module_defined=1 */ ;
    LUT4 i1_2_lut_adj_88 (.A(con[5]), .B(con[3]), .Z(n30357)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_adj_88.init = 16'h8888;
    LUT4 i1_3_lut_adj_89 (.A(con[4]), .B(con[2]), .C(con[1]), .Z(n29440)) /* synthesis lut_function=(!(A+!(B+(C)))) */ ;
    defparam i1_3_lut_adj_89.init = 16'h5454;
    check_IR_defv_U16 check_IR_defv1 (.\clock_count[15] (clock_count[15]), 
            .\def_voltag[1] (def_voltag[1]), .ir_402x_i_c_1(ir_402x_i_c[1]), 
            .rstn_c(rstn_c)) /* synthesis syn_module_defined=1 */ ;
    ir_car_detect_U3 ir_car_detect6 (.clk_c(clk_c), .\f2n_irx_temp[6] (f2n_irx_temp[6]), 
            .ir_car_null(ir_car_null_adj_79), .GND_net(tx_baud_adj_282[2])) /* synthesis syn_module_defined=1 */ ;
    ir_rx ir_rx_isntx (.clk_c(clk_c), .n111({n111}), .GND_net(tx_baud_adj_282[2]), 
          .n11(n11_adj_138), .ir_front_c(ir_front_c), .n12(n12_adj_137), 
          .\f2n_irx_level[5] (f2n_irx_level[5]), .n30180(n30180), .n30178(n30178), 
          .n10(n30352)) /* synthesis syn_module_defined=1 */ ;
    LUT4 rs232tx_bdd_4_lut (.A(chnum[0]), .B(chnum[2]), .C(chnum[3]), 
         .D(chnum[1]), .Z(n30053)) /* synthesis lut_function=((B (C+!(D))+!B ((D)+!C))+!A) */ ;
    defparam rs232tx_bdd_4_lut.init = 16'hf7df;
    PUR PUR_INST (.PUR(VCC_net));
    defparam PUR_INST.RST_PULSE = 1;
    mcu2pc_uart_top mcu2pc_uart_top1 (.clk_c(clk_c), .n30301(n30301), .to_db9_baud({to_db9_baud}), 
            .rx_baud_count({rx_baud_count_adj_299}), .rs232_db9_rx_c(rs232_db9_rx_c), 
            .n30300(n30300), .GND_net(tx_baud_adj_282[2]), .rxbaud_fifo_tx_db9({rxbaud_fifo_tx_db9}), 
            .\rx_step[3] (rx_step[3]), .n30134(n30134), .n30121(n30121), 
            .n30125(n30125), .n30155(n30155), .n29096(n29096), .n30166(n30166), 
            .n134(n30348), .VCC_net(VCC_net)) /* synthesis syn_module_defined=1 */ ;
    
endmodule

//
// Verilog Description of module check_IR_defv
//

module check_IR_defv (\clock_count[15] , ir_402x_i_c_7, \def_voltag[7] , 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input \clock_count[15] ;
    input ir_402x_i_c_7;
    output \def_voltag[7] ;
    input rstn_c;
    
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    wire [4:0]n26;
    
    wire n1918;
    wire [4:0]n26_adj_69;
    
    wire n30145, n4413, n9, n4222, n29577, n30154, n29551, n3997, 
        n1;
    
    LUT4 i26313_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26313_2_lut.init = 16'h6666;
    FD1S3IX L_count_1499__i0 (.D(n26_adj_69[0]), .CK(\clock_count[15] ), 
            .CD(n1918), .Q(L_count[0]));
    defparam L_count_1499__i0.GSR = "ENABLED";
    LUT4 i26320_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26320_2_lut_3_lut.init = 16'h7878;
    LUT4 i26327_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26327_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26316_2_lut_rep_114 (.A(H_count[1]), .B(H_count[0]), .Z(n30145)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26316_2_lut_rep_114.init = 16'h8888;
    FD1S3IX H_count_1498__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n4413), 
            .Q(H_count[4]));
    defparam H_count_1498__i4.GSR = "ENABLED";
    FD1S3IX H_count_1498__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n4413), 
            .Q(H_count[3]));
    defparam H_count_1498__i3.GSR = "ENABLED";
    FD1S3IX H_count_1498__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n4413), 
            .Q(H_count[2]));
    defparam H_count_1498__i2.GSR = "ENABLED";
    FD1S3IX H_count_1498__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n4413), 
            .Q(H_count[1]));
    defparam H_count_1498__i1.GSR = "ENABLED";
    LUT4 i27955_2_lut (.A(ir_402x_i_c_7), .B(n9), .Z(n4413)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i27955_2_lut.init = 16'h7777;
    FD1P3AX defv_23 (.D(n9), .SP(n4222), .CK(\clock_count[15] ), .Q(\def_voltag[7] ));
    defparam defv_23.GSR = "DISABLED";
    FD1S3IX L_count_1499__i4 (.D(n26_adj_69[4]), .CK(\clock_count[15] ), 
            .CD(n1918), .Q(L_count[4]));
    defparam L_count_1499__i4.GSR = "ENABLED";
    FD1S3IX L_count_1499__i3 (.D(n26_adj_69[3]), .CK(\clock_count[15] ), 
            .CD(n1918), .Q(L_count[3]));
    defparam L_count_1499__i3.GSR = "ENABLED";
    FD1S3IX L_count_1499__i2 (.D(n26_adj_69[2]), .CK(\clock_count[15] ), 
            .CD(n1918), .Q(L_count[2]));
    defparam L_count_1499__i2.GSR = "ENABLED";
    FD1S3IX L_count_1499__i1 (.D(n26_adj_69[1]), .CK(\clock_count[15] ), 
            .CD(n1918), .Q(L_count[1]));
    defparam L_count_1499__i1.GSR = "ENABLED";
    LUT4 i27812_2_lut (.A(H_count[4]), .B(H_count[3]), .Z(n29577)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27812_2_lut.init = 16'h8888;
    LUT4 i26370_3_lut_4_lut (.A(L_count[2]), .B(n30154), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26_adj_69[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26370_3_lut_4_lut.init = 16'h7f80;
    LUT4 i3_4_lut (.A(H_count[1]), .B(H_count[2]), .C(H_count[0]), .D(n29577), 
         .Z(n9)) /* synthesis lut_function=(A+(B+!(C (D)))) */ ;
    defparam i3_4_lut.init = 16'hefff;
    LUT4 i27786_2_lut (.A(L_count[4]), .B(L_count[3]), .Z(n29551)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27786_2_lut.init = 16'h8888;
    LUT4 i3_4_lut_adj_15 (.A(L_count[2]), .B(L_count[0]), .C(L_count[1]), 
         .D(n29551), .Z(n3997)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut_adj_15.init = 16'hfbff;
    FD1S3IX H_count_1498__i0 (.D(n1), .CK(\clock_count[15] ), .CD(n4413), 
            .Q(H_count[0]));
    defparam H_count_1498__i0.GSR = "ENABLED";
    LUT4 i899_3_lut (.A(ir_402x_i_c_7), .B(n3997), .C(n9), .Z(n1918)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i899_3_lut.init = 16'hbaba;
    LUT4 i26349_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26_adj_69[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26349_2_lut.init = 16'h6666;
    LUT4 i26334_3_lut_4_lut (.A(H_count[2]), .B(n30145), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26334_3_lut_4_lut.init = 16'h7f80;
    LUT4 i1_3_lut (.A(n3997), .B(rstn_c), .C(n9), .Z(n4222)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    INV i1832 (.A(H_count[0]), .Z(n1));
    INV i26347 (.A(L_count[0]), .Z(n26_adj_69[0]));
    LUT4 i26363_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26_adj_69[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26363_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26356_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26_adj_69[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26356_2_lut_3_lut.init = 16'h7878;
    LUT4 i26352_2_lut_rep_123 (.A(L_count[1]), .B(L_count[0]), .Z(n30154)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26352_2_lut_rep_123.init = 16'h8888;
    
endmodule

//
// Verilog Description of module my_uart_top
//

module my_uart_top (n134, rstn_c, clk_c, GND_net, bps_sel, rdy, 
            rden, txdata, rs232tx, n4439) /* synthesis syn_module_defined=1 */ ;
    output n134;
    input rstn_c;
    input clk_c;
    input GND_net;
    input [2:0]bps_sel;
    output rdy;
    input rden;
    input [7:0]txdata;
    output rs232tx;
    output n4439;
    
    wire clk_bps;   // g:/zjf/mhub4k862-cpld/my_uart_top.v(41[12:19])
    
    speed_select speed_tx (.n134(n134), .rstn_c(rstn_c), .clk_bps(clk_bps), 
            .clk_c(clk_c), .GND_net(GND_net), .bps_sel({bps_sel}), .rdy(rdy)) /* synthesis syn_module_defined=1 */ ;
    my_uart_tx my_uart_tx (.clk_c(clk_c), .rden(rden), .txdata({txdata}), 
            .rdy(rdy), .clk_bps(clk_bps), .rs232tx(rs232tx), .n4439(n4439)) /* synthesis syn_module_defined=1 */ ;
    
endmodule

//
// Verilog Description of module speed_select
//

module speed_select (n134, rstn_c, clk_bps, clk_c, GND_net, bps_sel, 
            rdy) /* synthesis syn_module_defined=1 */ ;
    output n134;
    input rstn_c;
    output clk_bps;
    input clk_c;
    input GND_net;
    input [2:0]bps_sel;
    input rdy;
    
    wire [13:0]cnt;   // g:/zjf/mhub4k862-cpld/speed_select.v(56[11:14])
    wire [13:0]bpcnt;   // g:/zjf/mhub4k862-cpld/speed_select.v(60[12:17])
    
    wire n72, n23;
    wire [13:0]n62;
    
    wire n28845, n28844, n28843, n28842, n28841, n28840, n28839, 
        n28810, n28809, n28808, n28807, n28806, n3437, n28805, 
        n30168, n5, n12, n28804;
    
    INV i26 (.A(rstn_c), .Z(n134));
    FD1S3AX clk_bps_r_18 (.D(n72), .CK(clk_c), .Q(clk_bps));
    defparam clk_bps_r_18.GSR = "ENABLED";
    FD1S3IX cnt_1508__i0 (.D(n62[0]), .CK(clk_c), .CD(n23), .Q(cnt[0]));
    defparam cnt_1508__i0.GSR = "ENABLED";
    FD1S3IX cnt_1508__i1 (.D(n62[1]), .CK(clk_c), .CD(n23), .Q(cnt[1]));
    defparam cnt_1508__i1.GSR = "ENABLED";
    FD1S3IX cnt_1508__i2 (.D(n62[2]), .CK(clk_c), .CD(n23), .Q(cnt[2]));
    defparam cnt_1508__i2.GSR = "ENABLED";
    FD1S3IX cnt_1508__i3 (.D(n62[3]), .CK(clk_c), .CD(n23), .Q(cnt[3]));
    defparam cnt_1508__i3.GSR = "ENABLED";
    FD1S3IX cnt_1508__i4 (.D(n62[4]), .CK(clk_c), .CD(n23), .Q(cnt[4]));
    defparam cnt_1508__i4.GSR = "ENABLED";
    FD1S3IX cnt_1508__i5 (.D(n62[5]), .CK(clk_c), .CD(n23), .Q(cnt[5]));
    defparam cnt_1508__i5.GSR = "ENABLED";
    FD1S3IX cnt_1508__i6 (.D(n62[6]), .CK(clk_c), .CD(n23), .Q(cnt[6]));
    defparam cnt_1508__i6.GSR = "ENABLED";
    FD1S3IX cnt_1508__i7 (.D(n62[7]), .CK(clk_c), .CD(n23), .Q(cnt[7]));
    defparam cnt_1508__i7.GSR = "ENABLED";
    FD1S3IX cnt_1508__i8 (.D(n62[8]), .CK(clk_c), .CD(n23), .Q(cnt[8]));
    defparam cnt_1508__i8.GSR = "ENABLED";
    FD1S3IX cnt_1508__i9 (.D(n62[9]), .CK(clk_c), .CD(n23), .Q(cnt[9]));
    defparam cnt_1508__i9.GSR = "ENABLED";
    FD1S3IX cnt_1508__i10 (.D(n62[10]), .CK(clk_c), .CD(n23), .Q(cnt[10]));
    defparam cnt_1508__i10.GSR = "ENABLED";
    FD1S3IX cnt_1508__i11 (.D(n62[11]), .CK(clk_c), .CD(n23), .Q(cnt[11]));
    defparam cnt_1508__i11.GSR = "ENABLED";
    FD1S3IX cnt_1508__i12 (.D(n62[12]), .CK(clk_c), .CD(n23), .Q(cnt[12]));
    defparam cnt_1508__i12.GSR = "ENABLED";
    FD1S3IX cnt_1508__i13 (.D(n62[13]), .CK(clk_c), .CD(n23), .Q(cnt[13]));
    defparam cnt_1508__i13.GSR = "ENABLED";
    CCU2D cnt_1508_add_4_15 (.A0(cnt[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28845), .S0(n62[13]));
    defparam cnt_1508_add_4_15.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_15.INIT1 = 16'h0000;
    defparam cnt_1508_add_4_15.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_15.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_13 (.A0(cnt[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(cnt[12]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28844), .COUT(n28845), .S0(n62[11]), .S1(n62[12]));
    defparam cnt_1508_add_4_13.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_13.INIT1 = 16'hfaaa;
    defparam cnt_1508_add_4_13.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_13.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_11 (.A0(cnt[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28843), .COUT(n28844), .S0(n62[9]), .S1(n62[10]));
    defparam cnt_1508_add_4_11.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_11.INIT1 = 16'hfaaa;
    defparam cnt_1508_add_4_11.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_11.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_9 (.A0(cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28842), 
          .COUT(n28843), .S0(n62[7]), .S1(n62[8]));
    defparam cnt_1508_add_4_9.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_9.INIT1 = 16'hfaaa;
    defparam cnt_1508_add_4_9.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_9.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_7 (.A0(cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28841), 
          .COUT(n28842), .S0(n62[5]), .S1(n62[6]));
    defparam cnt_1508_add_4_7.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_7.INIT1 = 16'hfaaa;
    defparam cnt_1508_add_4_7.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_7.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_5 (.A0(cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28840), 
          .COUT(n28841), .S0(n62[3]), .S1(n62[4]));
    defparam cnt_1508_add_4_5.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_5.INIT1 = 16'hfaaa;
    defparam cnt_1508_add_4_5.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_5.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_3 (.A0(cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28839), 
          .COUT(n28840), .S0(n62[1]), .S1(n62[2]));
    defparam cnt_1508_add_4_3.INIT0 = 16'hfaaa;
    defparam cnt_1508_add_4_3.INIT1 = 16'hfaaa;
    defparam cnt_1508_add_4_3.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_3.INJECT1_1 = "NO";
    CCU2D cnt_1508_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28839), .S1(n62[0]));
    defparam cnt_1508_add_4_1.INIT0 = 16'hF000;
    defparam cnt_1508_add_4_1.INIT1 = 16'h0555;
    defparam cnt_1508_add_4_1.INJECT1_0 = "NO";
    defparam cnt_1508_add_4_1.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_15 (.A0(cnt[13]), .B0(bps_sel[1]), .C0(bps_sel[0]), 
          .D0(bps_sel[2]), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28810), .S1(n72));
    defparam sub_1294_add_2_15.INIT0 = 16'h5565;
    defparam sub_1294_add_2_15.INIT1 = 16'h0000;
    defparam sub_1294_add_2_15.INJECT1_0 = "NO";
    defparam sub_1294_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_13 (.A0(cnt[11]), .B0(bps_sel[1]), .C0(bps_sel[0]), 
          .D0(bps_sel[2]), .A1(cnt[12]), .B1(bps_sel[0]), .C1(bps_sel[1]), 
          .D1(bps_sel[2]), .CIN(n28809), .COUT(n28810));
    defparam sub_1294_add_2_13.INIT0 = 16'h55a6;
    defparam sub_1294_add_2_13.INIT1 = 16'h5565;
    defparam sub_1294_add_2_13.INJECT1_0 = "NO";
    defparam sub_1294_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_11 (.A0(cnt[9]), .B0(bps_sel[0]), .C0(bps_sel[1]), 
          .D0(bps_sel[2]), .A1(cnt[10]), .B1(bps_sel[2]), .C1(bps_sel[1]), 
          .D1(bps_sel[0]), .CIN(n28808), .COUT(n28809));
    defparam sub_1294_add_2_11.INIT0 = 16'h5996;
    defparam sub_1294_add_2_11.INIT1 = 16'h5569;
    defparam sub_1294_add_2_11.INJECT1_0 = "NO";
    defparam sub_1294_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_9 (.A0(cnt[7]), .B0(bpcnt[7]), .C0(GND_net), 
          .D0(GND_net), .A1(bps_sel[0]), .B1(bps_sel[2]), .C1(cnt[8]), 
          .D1(GND_net), .CIN(n28807), .COUT(n28808));
    defparam sub_1294_add_2_9.INIT0 = 16'h5999;
    defparam sub_1294_add_2_9.INIT1 = 16'h4b4b;
    defparam sub_1294_add_2_9.INJECT1_0 = "NO";
    defparam sub_1294_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_7 (.A0(cnt[5]), .B0(bps_sel[1]), .C0(bps_sel[2]), 
          .D0(bps_sel[0]), .A1(bps_sel[1]), .B1(n3437), .C1(cnt[6]), 
          .D1(GND_net), .CIN(n28806), .COUT(n28807));
    defparam sub_1294_add_2_7.INIT0 = 16'h5a96;
    defparam sub_1294_add_2_7.INIT1 = 16'h2d2d;
    defparam sub_1294_add_2_7.INJECT1_0 = "NO";
    defparam sub_1294_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_5 (.A0(bps_sel[0]), .B0(n30168), .C0(cnt[3]), 
          .D0(GND_net), .A1(n5), .B1(n12), .C1(cnt[4]), .D1(GND_net), 
          .CIN(n28805), .COUT(n28806));
    defparam sub_1294_add_2_5.INIT0 = 16'hb4b4;
    defparam sub_1294_add_2_5.INIT1 = 16'he1e1;
    defparam sub_1294_add_2_5.INJECT1_0 = "NO";
    defparam sub_1294_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_3 (.A0(cnt[1]), .B0(bps_sel[0]), .C0(bps_sel[2]), 
          .D0(bps_sel[1]), .A1(cnt[2]), .B1(bps_sel[2]), .C1(bps_sel[1]), 
          .D1(bps_sel[0]), .CIN(n28804), .COUT(n28805));
    defparam sub_1294_add_2_3.INIT0 = 16'h5a9a;
    defparam sub_1294_add_2_3.INIT1 = 16'h5669;
    defparam sub_1294_add_2_3.INJECT1_0 = "NO";
    defparam sub_1294_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_1294_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(bps_sel[0]), .B1(bps_sel[2]), .C1(cnt[0]), 
          .D1(GND_net), .COUT(n28804));
    defparam sub_1294_add_2_1.INIT0 = 16'h0000;
    defparam sub_1294_add_2_1.INIT1 = 16'h7878;
    defparam sub_1294_add_2_1.INJECT1_0 = "NO";
    defparam sub_1294_add_2_1.INJECT1_1 = "NO";
    LUT4 i1_2_lut (.A(bps_sel[1]), .B(bps_sel[2]), .Z(n5)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut.init = 16'h8888;
    LUT4 i1930_2_lut (.A(bps_sel[2]), .B(bps_sel[0]), .Z(n3437)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i1930_2_lut.init = 16'h6666;
    LUT4 i11_3_lut (.A(bps_sel[0]), .B(bps_sel[2]), .C(bps_sel[1]), .Z(bpcnt[7])) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i11_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut_3_lut (.A(bps_sel[1]), .B(bps_sel[2]), .C(bps_sel[0]), 
         .Z(n12)) /* synthesis lut_function=(!(A (C)+!A ((C)+!B))) */ ;
    defparam i1_2_lut_3_lut.init = 16'h0e0e;
    LUT4 i14886_2_lut_rep_137 (.A(bps_sel[1]), .B(bps_sel[2]), .Z(n30168)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14886_2_lut_rep_137.init = 16'heeee;
    LUT4 i9_2_lut (.A(n72), .B(rdy), .Z(n23)) /* synthesis lut_function=(A+!(B)) */ ;
    defparam i9_2_lut.init = 16'hbbbb;
    
endmodule

//
// Verilog Description of module my_uart_tx
//

module my_uart_tx (clk_c, rden, txdata, rdy, clk_bps, rs232tx, n4439) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input rden;
    input [7:0]txdata;
    output rdy;
    input clk_bps;
    output rs232tx;
    output n4439;
    
    wire tx_int;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(141[13:19])
    wire [7:0]tx_data;   // g:/zjf/mhub4k862-cpld/my_uart_tx.v(64[10:17])
    wire [3:0]num;   // g:/zjf/mhub4k862-cpld/my_uart_tx.v(68[10:13])
    
    wire n30138, n4418;
    wire [3:0]n22;
    
    wire n29656, n29657, n29658, n4382, n7, n3510, n36, n28992, 
        n2, n1, n5, n4;
    
    FD1S3AX tx_int_485 (.D(rden), .CK(clk_c), .Q(tx_int));
    defparam tx_int_485.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i0 (.D(txdata[0]), .SP(tx_int), .CK(clk_c), .Q(tx_data[0]));
    defparam tx_data_i0_i0.GSR = "ENABLED";
    FD1P3IX num_1503__i1 (.D(n22[1]), .SP(n30138), .CD(n4418), .CK(clk_c), 
            .Q(num[1]));
    defparam num_1503__i1.GSR = "ENABLED";
    FD1P3IX num_1503__i2 (.D(n22[2]), .SP(n30138), .CD(n4418), .CK(clk_c), 
            .Q(num[2]));
    defparam num_1503__i2.GSR = "ENABLED";
    FD1P3IX num_1503__i3 (.D(n22[3]), .SP(n30138), .CD(n4418), .CK(clk_c), 
            .Q(num[3]));
    defparam num_1503__i3.GSR = "ENABLED";
    L6MUX21 i27885 (.D0(n29656), .D1(n29657), .SD(num[2]), .Z(n29658));
    FD1S3AX tx_en_24 (.D(n4382), .CK(clk_c), .Q(rdy));
    defparam tx_en_24.GSR = "ENABLED";
    FD1P3AX tx_data_i0_i7 (.D(txdata[7]), .SP(tx_int), .CK(clk_c), .Q(tx_data[7]));
    defparam tx_data_i0_i7.GSR = "ENABLED";
    LUT4 i27969_2_lut_3_lut (.A(n7), .B(rdy), .C(clk_bps), .Z(n4418)) /* synthesis lut_function=(!(A+((C)+!B))) */ ;
    defparam i27969_2_lut_3_lut.init = 16'h0404;
    LUT4 i28036_3_lut_rep_107 (.A(n7), .B(rdy), .C(clk_bps), .Z(n30138)) /* synthesis lut_function=(A (B (C))+!A (B)) */ ;
    defparam i28036_3_lut_rep_107.init = 16'hc4c4;
    FD1P3AX tx_data_i0_i6 (.D(txdata[6]), .SP(tx_int), .CK(clk_c), .Q(tx_data[6]));
    defparam tx_data_i0_i6.GSR = "ENABLED";
    FD1P3AX tx_data_i0_i5 (.D(txdata[5]), .SP(tx_int), .CK(clk_c), .Q(tx_data[5]));
    defparam tx_data_i0_i5.GSR = "ENABLED";
    FD1P3AX tx_data_i0_i4 (.D(txdata[4]), .SP(tx_int), .CK(clk_c), .Q(tx_data[4]));
    defparam tx_data_i0_i4.GSR = "ENABLED";
    FD1P3AX tx_data_i0_i3 (.D(txdata[3]), .SP(tx_int), .CK(clk_c), .Q(tx_data[3]));
    defparam tx_data_i0_i3.GSR = "ENABLED";
    FD1P3AX tx_data_i0_i2 (.D(txdata[2]), .SP(tx_int), .CK(clk_c), .Q(tx_data[2]));
    defparam tx_data_i0_i2.GSR = "ENABLED";
    FD1P3AX tx_data_i0_i1 (.D(txdata[1]), .SP(tx_int), .CK(clk_c), .Q(tx_data[1]));
    defparam tx_data_i0_i1.GSR = "ENABLED";
    FD1P3IX num_1503__i0 (.D(n22[0]), .SP(n30138), .CD(n4418), .CK(clk_c), 
            .Q(num[0]));
    defparam num_1503__i0.GSR = "ENABLED";
    LUT4 i14806_3_lut (.A(rdy), .B(tx_int), .C(n7), .Z(n4382)) /* synthesis lut_function=(A (B+(C))+!A (B)) */ ;
    defparam i14806_3_lut.init = 16'hecec;
    FD1P3AY rs232_tx_r_28 (.D(n36), .SP(n3510), .CK(clk_c), .Q(rs232tx));
    defparam rs232_tx_r_28.GSR = "ENABLED";
    INV i2857 (.A(rs232tx), .Z(n4439));
    LUT4 i26521_3_lut_4_lut (.A(num[1]), .B(num[0]), .C(num[2]), .D(num[3]), 
         .Z(n22[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26521_3_lut_4_lut.init = 16'h7f80;
    LUT4 i26514_2_lut_3_lut (.A(num[1]), .B(num[0]), .C(num[2]), .Z(n22[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26514_2_lut_3_lut.init = 16'h7878;
    LUT4 i3_4_lut (.A(tx_data[7]), .B(num[0]), .C(num[2]), .D(num[1]), 
         .Z(n28992)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3_4_lut.init = 16'hfffe;
    LUT4 Mux_14_i15_3_lut (.A(n29658), .B(n28992), .C(num[3]), .Z(n36)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_14_i15_3_lut.init = 16'hcaca;
    LUT4 i1982_2_lut (.A(clk_bps), .B(rdy), .Z(n3510)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1982_2_lut.init = 16'h8888;
    LUT4 Mux_14_i2_3_lut (.A(tx_data[1]), .B(tx_data[2]), .C(num[0]), 
         .Z(n2)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_14_i2_3_lut.init = 16'hcaca;
    LUT4 i14839_2_lut (.A(tx_data[0]), .B(num[0]), .Z(n1)) /* synthesis lut_function=(A (B)) */ ;
    defparam i14839_2_lut.init = 16'h8888;
    LUT4 Mux_14_i5_3_lut (.A(tx_data[5]), .B(tx_data[6]), .C(num[0]), 
         .Z(n5)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_14_i5_3_lut.init = 16'hcaca;
    LUT4 Mux_14_i4_3_lut (.A(tx_data[3]), .B(tx_data[4]), .C(num[0]), 
         .Z(n4)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_14_i4_3_lut.init = 16'hcaca;
    LUT4 i2_4_lut (.A(num[3]), .B(num[2]), .C(num[1]), .D(num[0]), .Z(n7)) /* synthesis lut_function=((B+!(C (D)))+!A) */ ;
    defparam i2_4_lut.init = 16'hdfff;
    LUT4 i26507_2_lut (.A(num[1]), .B(num[0]), .Z(n22[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26507_2_lut.init = 16'h6666;
    INV i26505 (.A(num[0]), .Z(n22[0]));
    PFUMX i27884 (.BLUT(n4), .ALUT(n5), .C0(num[1]), .Z(n29657));
    PFUMX i27883 (.BLUT(n1), .ALUT(n2), .C0(num[1]), .Z(n29656));
    
endmodule

//
// Verilog Description of module fifo64x8_U10
//

module fifo64x8_U10 (con, clk_c, wren_w, rden, n134, txdata, empty, 
            full, GND_net, VCC_net) /* synthesis syn_module_defined=1 */ ;
    input [7:0]con;
    input clk_c;
    input wren_w;
    input rden;
    input n134;
    output [7:0]txdata;
    output empty;
    output full;
    input GND_net;
    input VCC_net;
    
    
    FIFO8KB fifo64x8_0_0 (.DI0(con[0]), .DI1(con[1]), .DI2(con[2]), .DI3(con[3]), 
            .DI4(con[4]), .DI5(con[5]), .DI6(con[6]), .DI7(con[7]), 
            .DI8(GND_net), .DI9(GND_net), .DI10(GND_net), .DI11(GND_net), 
            .DI12(GND_net), .DI13(GND_net), .DI14(GND_net), .DI15(GND_net), 
            .DI16(GND_net), .DI17(GND_net), .FULLI(full), .EMPTYI(empty), 
            .CSW1(VCC_net), .CSW0(VCC_net), .CSR1(VCC_net), .CSR0(VCC_net), 
            .WE(wren_w), .RE(rden), .ORE(rden), .CLKW(clk_c), .CLKR(clk_c), 
            .RST(n134), .RPRST(n134), .DO9(txdata[0]), .DO10(txdata[1]), 
            .DO11(txdata[2]), .DO12(txdata[3]), .DO13(txdata[4]), .DO14(txdata[5]), 
            .DO15(txdata[6]), .DO16(txdata[7]), .EF(empty), .FF(full)) /* synthesis syn_instantiated=1 */ ;   // g:/zjf/mhub4k862-cpld/ir_rs232_sw.v(123[11] 136[3])
    defparam fifo64x8_0_0.DATA_WIDTH_W = 18;
    defparam fifo64x8_0_0.DATA_WIDTH_R = 18;
    defparam fifo64x8_0_0.REGMODE = "NOREG";
    defparam fifo64x8_0_0.RESETMODE = "ASYNC";
    defparam fifo64x8_0_0.ASYNC_RESET_RELEASE = "SYNC";
    defparam fifo64x8_0_0.CSDECODE_W = "0b11";
    defparam fifo64x8_0_0.CSDECODE_R = "0b11";
    defparam fifo64x8_0_0.AEPOINTER = "0b00000001010000";
    defparam fifo64x8_0_0.AEPOINTER1 = "0b00000001100000";
    defparam fifo64x8_0_0.AFPOINTER = "0b00001111000000";
    defparam fifo64x8_0_0.AFPOINTER1 = "0b00001110110000";
    defparam fifo64x8_0_0.FULLPOINTER = "0b10000000000000";
    defparam fifo64x8_0_0.FULLPOINTER1 = "0b01111111110000";
    defparam fifo64x8_0_0.GSR = "DISABLED";
    
endmodule

//
// Verilog Description of module ir_car_detect_U8
//

module ir_car_detect_U8 (clk_c, \f2n_irx_temp[1] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[1] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n24;
    
    wire n28733;
    wire [11:0]n11;
    
    wire n28732, n28731, n28730, n28729, n28728;
    
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[1] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28733), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28732), .COUT(n28733), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28731), .COUT(n28732), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28730), .COUT(n28731), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28729), .COUT(n28730), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    LUT4 i15032_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15032_2_lut.init = 16'heeee;
    LUT4 i15031_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15031_2_lut.init = 16'heeee;
    LUT4 i15030_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15030_2_lut.init = 16'heeee;
    LUT4 i15029_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15029_2_lut.init = 16'heeee;
    LUT4 i15028_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15028_2_lut.init = 16'heeee;
    LUT4 i15027_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15027_2_lut.init = 16'heeee;
    LUT4 i15026_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15026_2_lut.init = 16'heeee;
    LUT4 i15025_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15025_2_lut.init = 16'heeee;
    LUT4 i15024_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15024_2_lut.init = 16'heeee;
    LUT4 i15023_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15023_2_lut.init = 16'heeee;
    LUT4 i15022_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15022_2_lut.init = 16'heeee;
    LUT4 i14814_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14814_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28728), .COUT(n28729), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28728), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    
endmodule

//
// Verilog Description of module ir_car_detect_U7
//

module ir_car_detect_U7 (clk_c, \f2n_irx_temp[2] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[2] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n24;
    
    wire n28739;
    wire [11:0]n11;
    
    wire n28738, n28737, n28736, n28735, n28734;
    
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[2] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28739), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28738), .COUT(n28739), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28737), .COUT(n28738), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28736), .COUT(n28737), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28735), .COUT(n28736), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28734), .COUT(n28735), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28734), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i14815_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14815_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    LUT4 i15021_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15021_2_lut.init = 16'heeee;
    LUT4 i15020_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15020_2_lut.init = 16'heeee;
    LUT4 i15019_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15019_2_lut.init = 16'heeee;
    LUT4 i15018_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15018_2_lut.init = 16'heeee;
    LUT4 i15017_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15017_2_lut.init = 16'heeee;
    LUT4 i15016_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15016_2_lut.init = 16'heeee;
    LUT4 i15015_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15015_2_lut.init = 16'heeee;
    LUT4 i15014_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15014_2_lut.init = 16'heeee;
    LUT4 i15013_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15013_2_lut.init = 16'heeee;
    LUT4 i15012_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15012_2_lut.init = 16'heeee;
    LUT4 i15011_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15011_2_lut.init = 16'heeee;
    
endmodule

//
// Verilog Description of module check_IR_defv_U13
//

module check_IR_defv_U13 (\clock_count[15] , \def_voltag[4] , ir_402x_i_c_4, 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input \clock_count[15] ;
    output \def_voltag[4] ;
    input ir_402x_i_c_4;
    input rstn_c;
    
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    
    wire n1891;
    wire [4:0]n26;
    wire [4:0]n26_adj_63;
    
    wire n30143, n4213, n9, n4423, n1, n30164, n3991, n29581, 
        n6;
    
    FD1S3IX L_count_1490__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n1891), 
            .Q(L_count[4]));
    defparam L_count_1490__i4.GSR = "ENABLED";
    FD1S3IX L_count_1490__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n1891), 
            .Q(L_count[3]));
    defparam L_count_1490__i3.GSR = "ENABLED";
    FD1S3IX L_count_1490__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n1891), 
            .Q(L_count[2]));
    defparam L_count_1490__i2.GSR = "ENABLED";
    FD1S3IX L_count_1490__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n1891), 
            .Q(L_count[1]));
    defparam L_count_1490__i1.GSR = "ENABLED";
    LUT4 i26611_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26_adj_63[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26611_2_lut_3_lut.init = 16'h7878;
    LUT4 i26618_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26_adj_63[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26618_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26607_2_lut_rep_112 (.A(H_count[1]), .B(H_count[0]), .Z(n30143)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26607_2_lut_rep_112.init = 16'h8888;
    LUT4 i26604_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26_adj_63[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26604_2_lut.init = 16'h6666;
    FD1S3IX L_count_1490__i0 (.D(n26[0]), .CK(\clock_count[15] ), .CD(n1891), 
            .Q(L_count[0]));
    defparam L_count_1490__i0.GSR = "ENABLED";
    FD1P3AX defv_23 (.D(n9), .SP(n4213), .CK(\clock_count[15] ), .Q(\def_voltag[4] ));
    defparam defv_23.GSR = "DISABLED";
    FD1S3IX H_count_1489__i0 (.D(n1), .CK(\clock_count[15] ), .CD(n4423), 
            .Q(H_count[0]));
    defparam H_count_1489__i0.GSR = "ENABLED";
    LUT4 i26719_3_lut_4_lut (.A(L_count[2]), .B(n30164), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26719_3_lut_4_lut.init = 16'h7f80;
    FD1S3IX H_count_1489__i4 (.D(n26_adj_63[4]), .CK(\clock_count[15] ), 
            .CD(n4423), .Q(H_count[4]));
    defparam H_count_1489__i4.GSR = "ENABLED";
    FD1S3IX H_count_1489__i3 (.D(n26_adj_63[3]), .CK(\clock_count[15] ), 
            .CD(n4423), .Q(H_count[3]));
    defparam H_count_1489__i3.GSR = "ENABLED";
    FD1S3IX H_count_1489__i2 (.D(n26_adj_63[2]), .CK(\clock_count[15] ), 
            .CD(n4423), .Q(H_count[2]));
    defparam H_count_1489__i2.GSR = "ENABLED";
    FD1S3IX H_count_1489__i1 (.D(n26_adj_63[1]), .CK(\clock_count[15] ), 
            .CD(n4423), .Q(H_count[1]));
    defparam H_count_1489__i1.GSR = "ENABLED";
    LUT4 i27984_2_lut (.A(ir_402x_i_c_4), .B(n9), .Z(n4423)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i27984_2_lut.init = 16'h7777;
    LUT4 i26698_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26698_2_lut.init = 16'h6666;
    LUT4 i1_3_lut (.A(n3991), .B(rstn_c), .C(n9), .Z(n4213)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    LUT4 i26625_3_lut_4_lut (.A(H_count[2]), .B(n30143), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26_adj_63[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26625_3_lut_4_lut.init = 16'h7f80;
    LUT4 i27816_2_lut (.A(H_count[4]), .B(H_count[3]), .Z(n29581)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27816_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(H_count[1]), .B(H_count[2]), .C(H_count[0]), .D(n29581), 
         .Z(n9)) /* synthesis lut_function=(A+(B+!(C (D)))) */ ;
    defparam i3_4_lut.init = 16'hefff;
    LUT4 i1_2_lut (.A(L_count[4]), .B(L_count[2]), .Z(n6)) /* synthesis lut_function=((B)+!A) */ ;
    defparam i1_2_lut.init = 16'hdddd;
    LUT4 i4_4_lut (.A(L_count[1]), .B(L_count[3]), .C(L_count[0]), .D(n6), 
         .Z(n3991)) /* synthesis lut_function=(A+(((D)+!C)+!B)) */ ;
    defparam i4_4_lut.init = 16'hffbf;
    LUT4 i872_3_lut (.A(ir_402x_i_c_4), .B(n3991), .C(n9), .Z(n1891)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i872_3_lut.init = 16'hbaba;
    INV i1826 (.A(H_count[0]), .Z(n1));
    LUT4 i26712_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26712_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26705_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26705_2_lut_3_lut.init = 16'h7878;
    LUT4 i26701_2_lut_rep_133 (.A(L_count[1]), .B(L_count[0]), .Z(n30164)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26701_2_lut_rep_133.init = 16'h8888;
    INV i26696 (.A(L_count[0]), .Z(n26[0]));
    
endmodule

//
// Verilog Description of module \ir_def_lv(BIT_NUM=3'b01,IO_NUM=7'b01) 
//

module \ir_def_lv(BIT_NUM=3'b01,IO_NUM=7'b01)  (ir_defX_in, clk_c, ir_def_lv) /* synthesis syn_module_defined=1 */ ;
    input [0:0]ir_defX_in;
    input clk_c;
    output [0:0]ir_def_lv;
    
    wire [5:0]L_count;   // g:/zjf/mhub4k862-cpld/ir_def_lv.v(25[14:21])
    wire [5:0]H_count;   // g:/zjf/mhub4k862-cpld/ir_def_lv.v(24[14:21])
    wire [5:0]n52;
    
    wire n30137;
    wire [5:0]n30;
    wire [5:0]n52_adj_57;
    
    wire n30136;
    wire [5:0]n30_adj_58;
    
    wire n1945, n4414, n2841, n1938, n30165, n30161;
    
    LUT4 i26492_3_lut_4_lut (.A(n52[3]), .B(n30137), .C(n52[4]), .D(L_count[5]), 
         .Z(n30[5])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26492_3_lut_4_lut.init = 16'h7f80;
    LUT4 i26413_3_lut_4_lut (.A(n52_adj_57[3]), .B(n30136), .C(n52_adj_57[4]), 
         .D(H_count[5]), .Z(n30_adj_58[5])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26413_3_lut_4_lut.init = 16'h7f80;
    LUT4 i926_3_lut (.A(ir_defX_in[0]), .B(L_count[5]), .C(H_count[5]), 
         .Z(n1945)) /* synthesis lut_function=(A+!((C)+!B)) */ ;
    defparam i926_3_lut.init = 16'haeae;
    FD1S3IX L_count_1502__i0 (.D(n30[0]), .CK(clk_c), .CD(n1945), .Q(n52[0]));
    defparam L_count_1502__i0.GSR = "ENABLED";
    LUT4 i27961_2_lut (.A(ir_defX_in[0]), .B(H_count[5]), .Z(n4414)) /* synthesis lut_function=((B)+!A) */ ;
    defparam i27961_2_lut.init = 16'hdddd;
    FD1P3AX ir_def_lv_0__27 (.D(n1938), .SP(n2841), .CK(clk_c), .Q(ir_def_lv[0]));
    defparam ir_def_lv_0__27.GSR = "ENABLED";
    LUT4 i26485_2_lut_3_lut_4_lut (.A(n52[2]), .B(n30165), .C(n52[4]), 
         .D(n52[3]), .Z(n30[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26485_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26406_2_lut_3_lut_4_lut (.A(n52_adj_57[2]), .B(n30161), .C(n52_adj_57[4]), 
         .D(n52_adj_57[3]), .Z(n30_adj_58[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26406_2_lut_3_lut_4_lut.init = 16'h78f0;
    FD1S3IX H_count_1501__i0 (.D(n30_adj_58[0]), .CK(clk_c), .CD(n4414), 
            .Q(n52_adj_57[0]));
    defparam H_count_1501__i0.GSR = "ENABLED";
    FD1S3IX L_count_1502__i5 (.D(n30[5]), .CK(clk_c), .CD(n1945), .Q(L_count[5]));
    defparam L_count_1502__i5.GSR = "ENABLED";
    FD1S3IX L_count_1502__i4 (.D(n30[4]), .CK(clk_c), .CD(n1945), .Q(n52[4]));
    defparam L_count_1502__i4.GSR = "ENABLED";
    FD1S3IX L_count_1502__i3 (.D(n30[3]), .CK(clk_c), .CD(n1945), .Q(n52[3]));
    defparam L_count_1502__i3.GSR = "ENABLED";
    FD1S3IX L_count_1502__i2 (.D(n30[2]), .CK(clk_c), .CD(n1945), .Q(n52[2]));
    defparam L_count_1502__i2.GSR = "ENABLED";
    FD1S3IX L_count_1502__i1 (.D(n30[1]), .CK(clk_c), .CD(n1945), .Q(n52[1]));
    defparam L_count_1502__i1.GSR = "ENABLED";
    FD1S3IX H_count_1501__i5 (.D(n30_adj_58[5]), .CK(clk_c), .CD(n4414), 
            .Q(H_count[5]));
    defparam H_count_1501__i5.GSR = "ENABLED";
    FD1S3IX H_count_1501__i4 (.D(n30_adj_58[4]), .CK(clk_c), .CD(n4414), 
            .Q(n52_adj_57[4]));
    defparam H_count_1501__i4.GSR = "ENABLED";
    FD1S3IX H_count_1501__i3 (.D(n30_adj_58[3]), .CK(clk_c), .CD(n4414), 
            .Q(n52_adj_57[3]));
    defparam H_count_1501__i3.GSR = "ENABLED";
    FD1S3IX H_count_1501__i2 (.D(n30_adj_58[2]), .CK(clk_c), .CD(n4414), 
            .Q(n52_adj_57[2]));
    defparam H_count_1501__i2.GSR = "ENABLED";
    FD1S3IX H_count_1501__i1 (.D(n30_adj_58[1]), .CK(clk_c), .CD(n4414), 
            .Q(n52_adj_57[1]));
    defparam H_count_1501__i1.GSR = "ENABLED";
    LUT4 i1581_2_lut (.A(H_count[5]), .B(L_count[5]), .Z(n2841)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i1581_2_lut.init = 16'heeee;
    LUT4 i26385_2_lut (.A(n52_adj_57[1]), .B(n52_adj_57[0]), .Z(n30_adj_58[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26385_2_lut.init = 16'h6666;
    LUT4 i26464_2_lut (.A(n52[1]), .B(n52[0]), .Z(n30[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26464_2_lut.init = 16'h6666;
    INV i919 (.A(H_count[5]), .Z(n1938));
    LUT4 i26478_2_lut_3_lut_4_lut (.A(n52[1]), .B(n52[0]), .C(n52[3]), 
         .D(n52[2]), .Z(n30[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26478_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26471_2_lut_3_lut (.A(n52[1]), .B(n52[0]), .C(n52[2]), .Z(n30[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26471_2_lut_3_lut.init = 16'h7878;
    INV i26383 (.A(n52_adj_57[0]), .Z(n30_adj_58[0]));
    LUT4 i26474_2_lut_rep_106_3_lut (.A(n52[1]), .B(n52[0]), .C(n52[2]), 
         .Z(n30137)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i26474_2_lut_rep_106_3_lut.init = 16'h8080;
    LUT4 i26467_2_lut_rep_134 (.A(n52[1]), .B(n52[0]), .Z(n30165)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26467_2_lut_rep_134.init = 16'h8888;
    LUT4 i26399_2_lut_3_lut_4_lut (.A(n52_adj_57[1]), .B(n52_adj_57[0]), 
         .C(n52_adj_57[3]), .D(n52_adj_57[2]), .Z(n30_adj_58[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26399_2_lut_3_lut_4_lut.init = 16'h78f0;
    INV i26462 (.A(n52[0]), .Z(n30[0]));
    LUT4 i26392_2_lut_3_lut (.A(n52_adj_57[1]), .B(n52_adj_57[0]), .C(n52_adj_57[2]), 
         .Z(n30_adj_58[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26392_2_lut_3_lut.init = 16'h7878;
    LUT4 i26395_2_lut_rep_105_3_lut (.A(n52_adj_57[1]), .B(n52_adj_57[0]), 
         .C(n52_adj_57[2]), .Z(n30136)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i26395_2_lut_rep_105_3_lut.init = 16'h8080;
    LUT4 i26388_2_lut_rep_130 (.A(n52_adj_57[1]), .B(n52_adj_57[0]), .Z(n30161)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26388_2_lut_rep_130.init = 16'h8888;
    
endmodule

//
// Verilog Description of module check_IR_defv_U14
//

module check_IR_defv_U14 (\clock_count[15] , \def_voltag[3] , ir_402x_i_c_3, 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input \clock_count[15] ;
    output \def_voltag[3] ;
    input ir_402x_i_c_3;
    input rstn_c;
    
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    
    wire n1882;
    wire [4:0]n26;
    
    wire n4436;
    wire [4:0]n26_adj_45;
    
    wire n4207, n9, n30172, n30153, n29555, n29529, n3989;
    
    FD1S3IX L_count_1487__i0 (.D(n26[0]), .CK(\clock_count[15] ), .CD(n1882), 
            .Q(L_count[0]));
    defparam L_count_1487__i0.GSR = "ENABLED";
    FD1S3IX H_count_1486__i0 (.D(n26_adj_45[0]), .CK(\clock_count[15] ), 
            .CD(n4436), .Q(H_count[0]));
    defparam H_count_1486__i0.GSR = "ENABLED";
    FD1P3AX defv_23 (.D(n9), .SP(n4207), .CK(\clock_count[15] ), .Q(\def_voltag[3] ));
    defparam defv_23.GSR = "DISABLED";
    LUT4 i27057_3_lut_4_lut (.A(H_count[2]), .B(n30172), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26_adj_45[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i27057_3_lut_4_lut.init = 16'h7f80;
    FD1S3IX L_count_1487__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n1882), 
            .Q(L_count[4]));
    defparam L_count_1487__i4.GSR = "ENABLED";
    FD1S3IX L_count_1487__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n1882), 
            .Q(L_count[3]));
    defparam L_count_1487__i3.GSR = "ENABLED";
    FD1S3IX L_count_1487__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n1882), 
            .Q(L_count[2]));
    defparam L_count_1487__i2.GSR = "ENABLED";
    FD1S3IX L_count_1487__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n1882), 
            .Q(L_count[1]));
    defparam L_count_1487__i1.GSR = "ENABLED";
    LUT4 i26449_3_lut_4_lut (.A(L_count[2]), .B(n30153), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26449_3_lut_4_lut.init = 16'h7f80;
    FD1S3IX H_count_1486__i1 (.D(n26_adj_45[1]), .CK(\clock_count[15] ), 
            .CD(n4436), .Q(H_count[1]));
    defparam H_count_1486__i1.GSR = "ENABLED";
    FD1S3IX H_count_1486__i2 (.D(n26_adj_45[2]), .CK(\clock_count[15] ), 
            .CD(n4436), .Q(H_count[2]));
    defparam H_count_1486__i2.GSR = "ENABLED";
    FD1S3IX H_count_1486__i3 (.D(n26_adj_45[3]), .CK(\clock_count[15] ), 
            .CD(n4436), .Q(H_count[3]));
    defparam H_count_1486__i3.GSR = "ENABLED";
    FD1S3IX H_count_1486__i4 (.D(n26_adj_45[4]), .CK(\clock_count[15] ), 
            .CD(n4436), .Q(H_count[4]));
    defparam H_count_1486__i4.GSR = "ENABLED";
    LUT4 i28045_2_lut (.A(ir_402x_i_c_3), .B(n9), .Z(n4436)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i28045_2_lut.init = 16'h7777;
    LUT4 i26428_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26428_2_lut.init = 16'h6666;
    LUT4 i27790_2_lut (.A(H_count[4]), .B(H_count[3]), .Z(n29555)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27790_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(H_count[2]), .B(H_count[0]), .C(H_count[1]), .D(n29555), 
         .Z(n9)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut.init = 16'hfbff;
    LUT4 i27764_2_lut (.A(L_count[4]), .B(L_count[3]), .Z(n29529)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27764_2_lut.init = 16'h8888;
    LUT4 i3_4_lut_adj_14 (.A(L_count[2]), .B(L_count[0]), .C(L_count[1]), 
         .D(n29529), .Z(n3989)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut_adj_14.init = 16'hfbff;
    LUT4 i863_3_lut (.A(ir_402x_i_c_3), .B(n3989), .C(n9), .Z(n1882)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i863_3_lut.init = 16'hbaba;
    LUT4 i1_3_lut (.A(n3989), .B(rstn_c), .C(n9), .Z(n4207)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    LUT4 i27036_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26_adj_45[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i27036_2_lut.init = 16'h6666;
    LUT4 i27050_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26_adj_45[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i27050_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i27043_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26_adj_45[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i27043_2_lut_3_lut.init = 16'h7878;
    LUT4 i27039_2_lut_rep_141 (.A(H_count[1]), .B(H_count[0]), .Z(n30172)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27039_2_lut_rep_141.init = 16'h8888;
    INV i27034 (.A(H_count[0]), .Z(n26_adj_45[0]));
    INV i26426 (.A(L_count[0]), .Z(n26[0]));
    LUT4 i26435_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26435_2_lut_3_lut.init = 16'h7878;
    LUT4 i26442_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26442_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26431_2_lut_rep_122 (.A(L_count[1]), .B(L_count[0]), .Z(n30153)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26431_2_lut_rep_122.init = 16'h8888;
    
endmodule

//
// Verilog Description of module ir_car_detect_U9
//

module ir_car_detect_U9 (clk_c, \f2n_irx_temp[0] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[0] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n24;
    
    wire n28722;
    wire [11:0]n11;
    
    wire n28723, n28727, n28726, n28725, n28724;
    
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[0] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28722), .COUT(n28723), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28722), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i15043_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15043_2_lut.init = 16'heeee;
    LUT4 i15042_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15042_2_lut.init = 16'heeee;
    LUT4 i15041_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15041_2_lut.init = 16'heeee;
    LUT4 i15040_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15040_2_lut.init = 16'heeee;
    LUT4 i15039_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15039_2_lut.init = 16'heeee;
    LUT4 i15038_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15038_2_lut.init = 16'heeee;
    LUT4 i15037_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15037_2_lut.init = 16'heeee;
    LUT4 i15036_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15036_2_lut.init = 16'heeee;
    LUT4 i15035_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15035_2_lut.init = 16'heeee;
    LUT4 i15034_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15034_2_lut.init = 16'heeee;
    LUT4 i15033_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15033_2_lut.init = 16'heeee;
    LUT4 i14807_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14807_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28727), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28726), .COUT(n28727), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28725), .COUT(n28726), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28724), .COUT(n28725), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28723), .COUT(n28724), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    
endmodule

//
// Verilog Description of module ir_car_detect_U5
//

module ir_car_detect_U5 (GND_net, clk_c, \f2n_irx_temp[4] , ir_car_null) /* synthesis syn_module_defined=1 */ ;
    input GND_net;
    input clk_c;
    input \f2n_irx_temp[4] ;
    output ir_car_null;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    
    wire n28749;
    wire [11:0]n11;
    
    wire n28750;
    wire [11:0]n24;
    
    wire n28748, n28747, n28746, n28751;
    
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28749), .COUT(n28750), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[4] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28748), .COUT(n28749), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28747), .COUT(n28748), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28746), .COUT(n28747), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28746), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i14819_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14819_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    LUT4 i14999_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14999_2_lut.init = 16'heeee;
    LUT4 i14998_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14998_2_lut.init = 16'heeee;
    LUT4 i14997_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14997_2_lut.init = 16'heeee;
    LUT4 i14996_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14996_2_lut.init = 16'heeee;
    LUT4 i14995_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14995_2_lut.init = 16'heeee;
    LUT4 i14994_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14994_2_lut.init = 16'heeee;
    LUT4 i14993_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14993_2_lut.init = 16'heeee;
    LUT4 i14992_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14992_2_lut.init = 16'heeee;
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28751), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    LUT4 i14991_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14991_2_lut.init = 16'heeee;
    LUT4 i14990_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14990_2_lut.init = 16'heeee;
    LUT4 i14989_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14989_2_lut.init = 16'heeee;
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28750), .COUT(n28751), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    
endmodule

//
// Verilog Description of module ir_car_detect
//

module ir_car_detect (clk_c, \f2n_irx_temp[7] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[7] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n11;
    wire [11:0]n24;
    
    wire n28776, n28775, n28774, n28773, n28772, n28771;
    
    LUT4 i14958_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14958_2_lut.init = 16'heeee;
    LUT4 i14957_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14957_2_lut.init = 16'heeee;
    LUT4 i14956_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14956_2_lut.init = 16'heeee;
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[7] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    LUT4 i14825_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14825_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28776), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28775), .COUT(n28776), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28774), .COUT(n28775), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28773), .COUT(n28774), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28772), .COUT(n28773), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28771), .COUT(n28772), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28771), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i14966_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14966_2_lut.init = 16'heeee;
    LUT4 i14965_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14965_2_lut.init = 16'heeee;
    LUT4 i14964_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14964_2_lut.init = 16'heeee;
    LUT4 i14963_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14963_2_lut.init = 16'heeee;
    LUT4 i14962_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14962_2_lut.init = 16'heeee;
    LUT4 i14961_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14961_2_lut.init = 16'heeee;
    LUT4 i14960_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14960_2_lut.init = 16'heeee;
    LUT4 i14959_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14959_2_lut.init = 16'heeee;
    
endmodule

//
// Verilog Description of module check_IR_defv_U12
//

module check_IR_defv_U12 (\clock_count[15] , ir_402x_i_c_5, \def_voltag[5] , 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input \clock_count[15] ;
    input ir_402x_i_c_5;
    output \def_voltag[5] ;
    input rstn_c;
    
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    wire [4:0]n26;
    
    wire n30139, n4434, n1, n29543, n9, n30174;
    wire [4:0]n26_adj_39;
    
    wire n4216, n1900, n3993, n29561;
    
    LUT4 i26831_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26831_2_lut.init = 16'h6666;
    LUT4 i26845_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26845_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26838_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26838_2_lut_3_lut.init = 16'h7878;
    LUT4 i26834_2_lut_rep_108 (.A(H_count[1]), .B(H_count[0]), .Z(n30139)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26834_2_lut_rep_108.init = 16'h8888;
    FD1S3IX H_count_1492__i0 (.D(n1), .CK(\clock_count[15] ), .CD(n4434), 
            .Q(H_count[0]));
    defparam H_count_1492__i0.GSR = "ENABLED";
    LUT4 i27778_2_lut (.A(H_count[4]), .B(H_count[3]), .Z(n29543)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27778_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(H_count[1]), .B(H_count[2]), .C(H_count[0]), .D(n29543), 
         .Z(n9)) /* synthesis lut_function=(A+(B+!(C (D)))) */ ;
    defparam i3_4_lut.init = 16'hefff;
    LUT4 i28033_2_lut (.A(ir_402x_i_c_5), .B(n9), .Z(n4434)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i28033_2_lut.init = 16'h7777;
    LUT4 i26852_3_lut_4_lut (.A(H_count[2]), .B(n30139), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26852_3_lut_4_lut.init = 16'h7f80;
    LUT4 i27021_3_lut_4_lut (.A(L_count[2]), .B(n30174), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26_adj_39[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i27021_3_lut_4_lut.init = 16'h7f80;
    FD1P3AX defv_23 (.D(n9), .SP(n4216), .CK(\clock_count[15] ), .Q(\def_voltag[5] ));
    defparam defv_23.GSR = "DISABLED";
    FD1S3IX L_count_1493__i0 (.D(n26_adj_39[0]), .CK(\clock_count[15] ), 
            .CD(n1900), .Q(L_count[0]));
    defparam L_count_1493__i0.GSR = "ENABLED";
    FD1S3IX H_count_1492__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n4434), 
            .Q(H_count[1]));
    defparam H_count_1492__i1.GSR = "ENABLED";
    FD1S3IX H_count_1492__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n4434), 
            .Q(H_count[2]));
    defparam H_count_1492__i2.GSR = "ENABLED";
    FD1S3IX H_count_1492__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n4434), 
            .Q(H_count[3]));
    defparam H_count_1492__i3.GSR = "ENABLED";
    FD1S3IX H_count_1492__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n4434), 
            .Q(H_count[4]));
    defparam H_count_1492__i4.GSR = "ENABLED";
    FD1S3IX L_count_1493__i1 (.D(n26_adj_39[1]), .CK(\clock_count[15] ), 
            .CD(n1900), .Q(L_count[1]));
    defparam L_count_1493__i1.GSR = "ENABLED";
    FD1S3IX L_count_1493__i2 (.D(n26_adj_39[2]), .CK(\clock_count[15] ), 
            .CD(n1900), .Q(L_count[2]));
    defparam L_count_1493__i2.GSR = "ENABLED";
    FD1S3IX L_count_1493__i3 (.D(n26_adj_39[3]), .CK(\clock_count[15] ), 
            .CD(n1900), .Q(L_count[3]));
    defparam L_count_1493__i3.GSR = "ENABLED";
    FD1S3IX L_count_1493__i4 (.D(n26_adj_39[4]), .CK(\clock_count[15] ), 
            .CD(n1900), .Q(L_count[4]));
    defparam L_count_1493__i4.GSR = "ENABLED";
    LUT4 i881_3_lut (.A(ir_402x_i_c_5), .B(n3993), .C(n9), .Z(n1900)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i881_3_lut.init = 16'hbaba;
    LUT4 i27796_2_lut (.A(L_count[4]), .B(L_count[3]), .Z(n29561)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27796_2_lut.init = 16'h8888;
    LUT4 i3_4_lut_adj_13 (.A(L_count[2]), .B(L_count[0]), .C(L_count[1]), 
         .D(n29561), .Z(n3993)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut_adj_13.init = 16'hfbff;
    LUT4 i1_3_lut (.A(n3993), .B(rstn_c), .C(n9), .Z(n4216)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    LUT4 i27014_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26_adj_39[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i27014_2_lut_3_lut_4_lut.init = 16'h78f0;
    INV i1828 (.A(H_count[0]), .Z(n1));
    LUT4 i27007_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26_adj_39[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i27007_2_lut_3_lut.init = 16'h7878;
    LUT4 i27003_2_lut_rep_143 (.A(L_count[1]), .B(L_count[0]), .Z(n30174)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27003_2_lut_rep_143.init = 16'h8888;
    LUT4 i27000_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26_adj_39[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i27000_2_lut.init = 16'h6666;
    INV i26998 (.A(L_count[0]), .Z(n26_adj_39[0]));
    
endmodule

//
// Verilog Description of module ir_car_detect_U4
//

module ir_car_detect_U4 (clk_c, \f2n_irx_temp[5] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[5] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n24;
    wire [11:0]n11;
    
    wire n28764, n28763, n28762, n28761, n28760, n28759;
    
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[5] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    LUT4 i14821_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14821_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28764), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28763), .COUT(n28764), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28762), .COUT(n28763), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28761), .COUT(n28762), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28760), .COUT(n28761), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28759), .COUT(n28760), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28759), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i14988_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14988_2_lut.init = 16'heeee;
    LUT4 i14987_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14987_2_lut.init = 16'heeee;
    LUT4 i14986_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14986_2_lut.init = 16'heeee;
    LUT4 i14985_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14985_2_lut.init = 16'heeee;
    LUT4 i14984_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14984_2_lut.init = 16'heeee;
    LUT4 i14983_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14983_2_lut.init = 16'heeee;
    LUT4 i14982_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14982_2_lut.init = 16'heeee;
    LUT4 i14981_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14981_2_lut.init = 16'heeee;
    LUT4 i14980_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14980_2_lut.init = 16'heeee;
    LUT4 i14979_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14979_2_lut.init = 16'heeee;
    LUT4 i14978_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14978_2_lut.init = 16'heeee;
    
endmodule

//
// Verilog Description of module pc2mcu_uart_top
//

module pc2mcu_uart_top (clk_c, n30301, n30300, rs232_send_to_mcu_changebaud, 
            GND_net, n1175, to_db9_baud, rx_baud_count, n134, VCC_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input n30301;
    input n30300;
    output rs232_send_to_mcu_changebaud;
    input GND_net;
    input n1175;
    input [2:0]to_db9_baud;
    input [13:0]rx_baud_count;
    input n134;
    input VCC_net;
    
    wire txen;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(58[6:10])
    wire rdy;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(27[12:15])
    wire rx_int0;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(68[6:13])
    wire rx_int1;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(69[6:13])
    wire tx_int;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(51[12:18])
    wire rden;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(29[12:16])
    wire empty;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(28[12:17])
    wire [7:0]tx_dat;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(25[12:18])
    wire rx_ok;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(14[6:11])
    wire [7:0]rx_dat;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(13[12:18])
    
    wire n31;
    
    FD1P3IX txen_20 (.D(n30301), .SP(n31), .CD(rdy), .CK(clk_c), .Q(txen));
    defparam txen_20.GSR = "ENABLED";
    FD1S3AX rx_int0_21 (.D(txen), .CK(clk_c), .Q(rx_int0));
    defparam rx_int0_21.GSR = "ENABLED";
    FD1S3AX rx_int1_22 (.D(rx_int0), .CK(clk_c), .Q(rx_int1));
    defparam rx_int1_22.GSR = "ENABLED";
    FD1S3AX tx_int_23 (.D(rden), .CK(clk_c), .Q(tx_int));
    defparam tx_int_23.GSR = "DISABLED";
    LUT4 i27997_2_lut (.A(rdy), .B(empty), .Z(n31)) /* synthesis lut_function=(!(A+(B))) */ ;
    defparam i27997_2_lut.init = 16'h1111;
    LUT4 i18_2_lut (.A(rx_int0), .B(rx_int1), .Z(rden)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i18_2_lut.init = 16'h2222;
    uart_tx pc2mcu_tx (.clk_c(clk_c), .tx_int(tx_int), .tx_dat({tx_dat}), 
            .rdy(rdy), .n30300(n30300), .rs232_send_to_mcu_changebaud(rs232_send_to_mcu_changebaud), 
            .GND_net(GND_net)) /* synthesis syn_module_defined=1 */ ;
    uart_rx pc2mcu_rx (.rx_ok(rx_ok), .rx_dat({rx_dat}), .clk_c(clk_c), 
            .n1175(n1175), .to_db9_baud({to_db9_baud}), .rx_baud_count({rx_baud_count}), 
            .GND_net(GND_net)) /* synthesis syn_module_defined=1 */ ;
    fifo64x8 pc2mcu_fifo (.rx_dat({rx_dat}), .clk_c(clk_c), .rx_ok(rx_ok), 
            .rden(rden), .n134(n134), .tx_dat({tx_dat}), .empty(empty), 
            .GND_net(GND_net), .VCC_net(VCC_net)) /* synthesis syn_module_defined=1 */ ;
    
endmodule

//
// Verilog Description of module uart_tx
//

module uart_tx (clk_c, tx_int, tx_dat, rdy, n30300, rs232_send_to_mcu_changebaud, 
            GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input tx_int;
    input [7:0]tx_dat;
    output rdy;
    input n30300;
    output rs232_send_to_mcu_changebaud;
    input GND_net;
    
    wire [7:0]tx_data;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(157[10:17])
    wire [13:0]rx_clk_count;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(144[12:24])
    wire [3:0]tx_step;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(169[11:18])
    wire tx_samp_clk;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(150[11:22])
    
    wire n34;
    wire [13:0]n62;
    wire [3:0]n22;
    
    wire n7, n30123, n4432, n3577, n111, n1, n2, n29650, n28877, 
        n28876, n28875, n28874, n28873, n28872, n28817, n28816, 
        n28815, n28814, n28813, n28812, n28811, n4, n5, n29651, 
        n29652, n29009;
    
    FD1P3AX tx_data_i0_i0 (.D(tx_dat[0]), .SP(tx_int), .CK(clk_c), .Q(tx_data[0]));
    defparam tx_data_i0_i0.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i13 (.D(n62[13]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[13]));
    defparam rx_clk_count_1506__i13.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i12 (.D(n62[12]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[12]));
    defparam rx_clk_count_1506__i12.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i11 (.D(n62[11]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[11]));
    defparam rx_clk_count_1506__i11.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i10 (.D(n62[10]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[10]));
    defparam rx_clk_count_1506__i10.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i9 (.D(n62[9]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[9]));
    defparam rx_clk_count_1506__i9.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i8 (.D(n62[8]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[8]));
    defparam rx_clk_count_1506__i8.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i7 (.D(n62[7]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[7]));
    defparam rx_clk_count_1506__i7.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i6 (.D(n62[6]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[6]));
    defparam rx_clk_count_1506__i6.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i5 (.D(n62[5]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[5]));
    defparam rx_clk_count_1506__i5.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i4 (.D(n62[4]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[4]));
    defparam rx_clk_count_1506__i4.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i3 (.D(n62[3]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[3]));
    defparam rx_clk_count_1506__i3.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i2 (.D(n62[2]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[2]));
    defparam rx_clk_count_1506__i2.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i1 (.D(n62[1]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[1]));
    defparam rx_clk_count_1506__i1.GSR = "DISABLED";
    LUT4 i26676_2_lut_3_lut (.A(tx_step[0]), .B(tx_step[1]), .C(tx_step[2]), 
         .Z(n22[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26676_2_lut_3_lut.init = 16'h7878;
    LUT4 i26683_3_lut_4_lut (.A(tx_step[0]), .B(tx_step[1]), .C(tx_step[2]), 
         .D(tx_step[3]), .Z(n22[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26683_3_lut_4_lut.init = 16'h7f80;
    LUT4 i28009_3_lut_4_lut (.A(tx_step[0]), .B(tx_step[1]), .C(tx_step[3]), 
         .D(tx_step[2]), .Z(n7)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i28009_3_lut_4_lut.init = 16'h0080;
    FD1P3IX tx_step_1507__i0 (.D(n22[0]), .SP(n30123), .CD(n4432), .CK(clk_c), 
            .Q(tx_step[0]));
    defparam tx_step_1507__i0.GSR = "DISABLED";
    FD1P3JX tx_en_32 (.D(n30300), .SP(n7), .PD(tx_int), .CK(clk_c), 
            .Q(rdy));
    defparam tx_en_32.GSR = "DISABLED";
    FD1S3AX tx_samp_clk_30 (.D(n34), .CK(clk_c), .Q(tx_samp_clk));
    defparam tx_samp_clk_30.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1506__i0 (.D(n62[0]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[0]));
    defparam rx_clk_count_1506__i0.GSR = "DISABLED";
    FD1P3AX tx_buf_34 (.D(n111), .SP(n3577), .CK(clk_c), .Q(rs232_send_to_mcu_changebaud));
    defparam tx_buf_34.GSR = "DISABLED";
    FD1P3IX tx_step_1507__i3 (.D(n22[3]), .SP(n30123), .CD(n4432), .CK(clk_c), 
            .Q(tx_step[3]));
    defparam tx_step_1507__i3.GSR = "DISABLED";
    FD1P3IX tx_step_1507__i2 (.D(n22[2]), .SP(n30123), .CD(n4432), .CK(clk_c), 
            .Q(tx_step[2]));
    defparam tx_step_1507__i2.GSR = "DISABLED";
    FD1P3IX tx_step_1507__i1 (.D(n22[1]), .SP(n30123), .CD(n4432), .CK(clk_c), 
            .Q(tx_step[1]));
    defparam tx_step_1507__i1.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i7 (.D(tx_dat[7]), .SP(tx_int), .CK(clk_c), .Q(tx_data[7]));
    defparam tx_data_i0_i7.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i6 (.D(tx_dat[6]), .SP(tx_int), .CK(clk_c), .Q(tx_data[6]));
    defparam tx_data_i0_i6.GSR = "DISABLED";
    PFUMX i27877 (.BLUT(n1), .ALUT(n2), .C0(tx_step[1]), .Z(n29650));
    FD1P3AX tx_data_i0_i5 (.D(tx_dat[5]), .SP(tx_int), .CK(clk_c), .Q(tx_data[5]));
    defparam tx_data_i0_i5.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i4 (.D(tx_dat[4]), .SP(tx_int), .CK(clk_c), .Q(tx_data[4]));
    defparam tx_data_i0_i4.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i3 (.D(tx_dat[3]), .SP(tx_int), .CK(clk_c), .Q(tx_data[3]));
    defparam tx_data_i0_i3.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i2 (.D(tx_dat[2]), .SP(tx_int), .CK(clk_c), .Q(tx_data[2]));
    defparam tx_data_i0_i2.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i1 (.D(tx_dat[1]), .SP(tx_int), .CK(clk_c), .Q(tx_data[1]));
    defparam tx_data_i0_i1.GSR = "DISABLED";
    LUT4 Mux_18_i2_3_lut (.A(tx_data[1]), .B(tx_data[2]), .C(tx_step[0]), 
         .Z(n2)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i2_3_lut.init = 16'hcaca;
    LUT4 i14850_2_lut (.A(tx_data[0]), .B(tx_step[0]), .Z(n1)) /* synthesis lut_function=(A (B)) */ ;
    defparam i14850_2_lut.init = 16'h8888;
    LUT4 i26669_2_lut (.A(tx_step[1]), .B(tx_step[0]), .Z(n22[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26669_2_lut.init = 16'h6666;
    CCU2D add_26288_14 (.A0(rx_clk_count[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28877), .S1(n34));
    defparam add_26288_14.INIT0 = 16'h5555;
    defparam add_26288_14.INIT1 = 16'h0000;
    defparam add_26288_14.INJECT1_0 = "NO";
    defparam add_26288_14.INJECT1_1 = "NO";
    CCU2D add_26288_12 (.A0(rx_clk_count[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28876), .COUT(n28877));
    defparam add_26288_12.INIT0 = 16'h5aaa;
    defparam add_26288_12.INIT1 = 16'h5555;
    defparam add_26288_12.INJECT1_0 = "NO";
    defparam add_26288_12.INJECT1_1 = "NO";
    CCU2D add_26288_10 (.A0(rx_clk_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28875), .COUT(n28876));
    defparam add_26288_10.INIT0 = 16'h5aaa;
    defparam add_26288_10.INIT1 = 16'h5555;
    defparam add_26288_10.INJECT1_0 = "NO";
    defparam add_26288_10.INJECT1_1 = "NO";
    CCU2D add_26288_8 (.A0(rx_clk_count[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28874), .COUT(n28875));
    defparam add_26288_8.INIT0 = 16'h5555;
    defparam add_26288_8.INIT1 = 16'h5555;
    defparam add_26288_8.INJECT1_0 = "NO";
    defparam add_26288_8.INJECT1_1 = "NO";
    CCU2D add_26288_6 (.A0(rx_clk_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28873), .COUT(n28874));
    defparam add_26288_6.INIT0 = 16'h5aaa;
    defparam add_26288_6.INIT1 = 16'h5555;
    defparam add_26288_6.INJECT1_0 = "NO";
    defparam add_26288_6.INJECT1_1 = "NO";
    CCU2D add_26288_4 (.A0(rx_clk_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28872), .COUT(n28873));
    defparam add_26288_4.INIT0 = 16'h5aaa;
    defparam add_26288_4.INIT1 = 16'h5555;
    defparam add_26288_4.INJECT1_0 = "NO";
    defparam add_26288_4.INJECT1_1 = "NO";
    CCU2D add_26288_2 (.A0(rx_clk_count[1]), .B0(rx_clk_count[0]), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n28872));
    defparam add_26288_2.INIT0 = 16'h7000;
    defparam add_26288_2.INIT1 = 16'h5555;
    defparam add_26288_2.INJECT1_0 = "NO";
    defparam add_26288_2.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_15 (.A0(rx_clk_count[13]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28817), .S0(n62[13]));
    defparam rx_clk_count_1506_add_4_15.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_15.INIT1 = 16'h0000;
    defparam rx_clk_count_1506_add_4_15.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_15.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_13 (.A0(rx_clk_count[11]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[12]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28816), .COUT(n28817), .S0(n62[11]), 
          .S1(n62[12]));
    defparam rx_clk_count_1506_add_4_13.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_13.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_13.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_13.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_11 (.A0(rx_clk_count[9]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[10]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28815), .COUT(n28816), .S0(n62[9]), 
          .S1(n62[10]));
    defparam rx_clk_count_1506_add_4_11.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_11.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_11.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_11.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_9 (.A0(rx_clk_count[7]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[8]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28814), .COUT(n28815), .S0(n62[7]), 
          .S1(n62[8]));
    defparam rx_clk_count_1506_add_4_9.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_9.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_9.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_9.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_7 (.A0(rx_clk_count[5]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[6]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28813), .COUT(n28814), .S0(n62[5]), 
          .S1(n62[6]));
    defparam rx_clk_count_1506_add_4_7.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_7.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_7.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_7.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_5 (.A0(rx_clk_count[3]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[4]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28812), .COUT(n28813), .S0(n62[3]), 
          .S1(n62[4]));
    defparam rx_clk_count_1506_add_4_5.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_5.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_5.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_5.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_3 (.A0(rx_clk_count[1]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[2]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28811), .COUT(n28812), .S0(n62[1]), 
          .S1(n62[2]));
    defparam rx_clk_count_1506_add_4_3.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_3.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1506_add_4_3.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_3.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1506_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n28811), .S1(n62[0]));
    defparam rx_clk_count_1506_add_4_1.INIT0 = 16'hF000;
    defparam rx_clk_count_1506_add_4_1.INIT1 = 16'h0555;
    defparam rx_clk_count_1506_add_4_1.INJECT1_0 = "NO";
    defparam rx_clk_count_1506_add_4_1.INJECT1_1 = "NO";
    PFUMX i27878 (.BLUT(n4), .ALUT(n5), .C0(tx_step[1]), .Z(n29651));
    LUT4 Mux_18_i5_3_lut (.A(tx_data[5]), .B(tx_data[6]), .C(tx_step[0]), 
         .Z(n5)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i5_3_lut.init = 16'hcaca;
    LUT4 Mux_18_i4_3_lut (.A(tx_data[3]), .B(tx_data[4]), .C(tx_step[0]), 
         .Z(n4)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i4_3_lut.init = 16'hcaca;
    L6MUX21 i27879 (.D0(n29650), .D1(n29651), .SD(tx_step[2]), .Z(n29652));
    LUT4 i3_4_lut (.A(tx_data[7]), .B(tx_step[0]), .C(tx_step[2]), .D(tx_step[1]), 
         .Z(n29009)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3_4_lut.init = 16'hfffe;
    LUT4 Mux_18_i15_3_lut (.A(n29652), .B(n29009), .C(tx_step[3]), .Z(n111)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i15_3_lut.init = 16'hcaca;
    LUT4 i2047_2_lut (.A(tx_samp_clk), .B(rdy), .Z(n3577)) /* synthesis lut_function=(A (B)) */ ;
    defparam i2047_2_lut.init = 16'h8888;
    LUT4 i28019_2_lut_3_lut (.A(n7), .B(rdy), .C(tx_samp_clk), .Z(n4432)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i28019_2_lut_3_lut.init = 16'h0808;
    LUT4 i28003_3_lut_rep_92 (.A(n7), .B(rdy), .C(tx_samp_clk), .Z(n30123)) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i28003_3_lut_rep_92.init = 16'hc8c8;
    INV i26667 (.A(tx_step[0]), .Z(n22[0]));
    
endmodule

//
// Verilog Description of module uart_rx
//

module uart_rx (rx_ok, rx_dat, clk_c, n1175, to_db9_baud, rx_baud_count, 
            GND_net) /* synthesis syn_module_defined=1 */ ;
    output rx_ok;
    output [7:0]rx_dat;
    input clk_c;
    input n1175;
    input [2:0]to_db9_baud;
    input [13:0]rx_baud_count;
    input GND_net;
    
    wire [3:0]rx_step;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(29[11:18])
    wire rx_samp_clk;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(31[5:16])
    wire [2:0]baud_back;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(48[11:20])
    wire [13:0]rx_clk_count;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(30[12:24])
    
    wire n30280, n30281, n16729, n5, n30127, n30129, n30133, n30150, 
        n29464, n29479, n29541, n28914, n30149, n29465, n16395, 
        n27852;
    wire [13:0]n66;
    
    wire n27853, n75;
    wire [3:0]n1732;
    
    wire n30192, n29053;
    wire [3:0]n201;
    wire [13:0]n36;
    
    wire n30124, n2476;
    wire [13:0]n85;
    
    wire n81, n30282, n3631, n29480, n30170, n17071, n30207, n4, 
        n29481, n29537, n29466, n30152, n28852, n28851, n28850, 
        n28849, n28848, n28847, n28846, n28824, n28823, n28822, 
        n28821, n28820, n28819, n28818, n27855, n28803, n28802, 
        n28801, n28800, n28799, n28798, n28797;
    wire [3:0]n1707;
    
    wire n30210, n30279, n30208, n30209, n30205, n30206, n27854, 
        n30193, n30194, n30190, n30191, n29624;
    
    LUT4 n30280_bdd_3_lut (.A(n30280), .B(rx_ok), .C(rx_step[2]), .Z(n30281)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam n30280_bdd_3_lut.init = 16'hcaca;
    FD1P3IX rx_dat_buf_i0_i0 (.D(n1175), .SP(n16729), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[0]));
    defparam rx_dat_buf_i0_i0.GSR = "DISABLED";
    LUT4 i2_3_lut_rep_96_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), .C(rx_samp_clk), 
         .D(rx_step[3]), .Z(n30127)) /* synthesis lut_function=(A (B+((D)+!C))+!A (B+!(C (D)))) */ ;
    defparam i2_3_lut_rep_96_4_lut_4_lut.init = 16'hefdf;
    LUT4 i1_2_lut_rep_98_3_lut (.A(rx_step[2]), .B(rx_step[1]), .C(rx_step[3]), 
         .Z(n30129)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i1_2_lut_rep_98_3_lut.init = 16'hfefe;
    LUT4 i27925_2_lut_rep_102_3_lut (.A(rx_step[2]), .B(rx_step[1]), .C(rx_step[3]), 
         .Z(n30133)) /* synthesis lut_function=(!(A (C)+!A (B (C)+!B !(C)))) */ ;
    defparam i27925_2_lut_rep_102_3_lut.init = 16'h1e1e;
    LUT4 i2_2_lut_rep_119 (.A(rx_step[2]), .B(rx_step[1]), .Z(n30150)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i2_2_lut_rep_119.init = 16'heeee;
    LUT4 i1_2_lut_3_lut (.A(rx_step[2]), .B(rx_step[1]), .C(rx_step[0]), 
         .Z(n29464)) /* synthesis lut_function=(!(A (B+!(C))+!A !(B (C)))) */ ;
    defparam i1_2_lut_3_lut.init = 16'h6060;
    LUT4 i1_2_lut_3_lut_adj_9 (.A(rx_step[2]), .B(rx_step[1]), .C(rx_step[0]), 
         .Z(n29479)) /* synthesis lut_function=(A (B (C))+!A !(B+!(C))) */ ;
    defparam i1_2_lut_3_lut_adj_9.init = 16'h9090;
    LUT4 i28014_2_lut_2_lut_3_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), 
         .C(n30127), .D(rx_step[0]), .Z(n29541)) /* synthesis lut_function=(!(A ((C+(D))+!B)+!A (B+(C+(D))))) */ ;
    defparam i28014_2_lut_2_lut_3_lut_4_lut.init = 16'h0009;
    LUT4 i27949_2_lut_2_lut_3_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), 
         .C(n30127), .D(rx_step[0]), .Z(n28914)) /* synthesis lut_function=(!(A (B+(C+(D)))+!A ((C+(D))+!B))) */ ;
    defparam i27949_2_lut_2_lut_3_lut_4_lut.init = 16'h0006;
    LUT4 i1_2_lut_rep_118 (.A(rx_step[2]), .B(rx_step[1]), .Z(n30149)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i1_2_lut_rep_118.init = 16'h6666;
    LUT4 i1_2_lut_4_lut (.A(n30133), .B(rx_samp_clk), .C(rx_step[1]), 
         .D(n29464), .Z(n29465)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i1_2_lut_4_lut.init = 16'h0800;
    FD1P3AX baud_back_i0_i0 (.D(to_db9_baud[0]), .SP(n5), .CK(clk_c), 
            .Q(baud_back[0]));
    defparam baud_back_i0_i0.GSR = "DISABLED";
    LUT4 i28000_2_lut (.A(rx_step[2]), .B(rx_step[3]), .Z(n16395)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i28000_2_lut.init = 16'h7777;
    CCU2D equal_9_10 (.A0(n66[11]), .B0(rx_clk_count[11]), .C0(n66[10]), 
          .D0(rx_clk_count[10]), .A1(n66[9]), .B1(rx_clk_count[9]), .C1(n66[8]), 
          .D1(rx_clk_count[8]), .CIN(n27852), .COUT(n27853));
    defparam equal_9_10.INIT0 = 16'h9009;
    defparam equal_9_10.INIT1 = 16'h9009;
    defparam equal_9_10.INJECT1_0 = "YES";
    defparam equal_9_10.INJECT1_1 = "YES";
    FD1P3IX rx_step_i3 (.D(n1732[3]), .SP(n75), .CD(n5), .CK(clk_c), 
            .Q(rx_step[3]));
    defparam rx_step_i3.GSR = "DISABLED";
    FD1S3IX rx_step_i2 (.D(n30192), .CK(clk_c), .CD(n5), .Q(rx_step[2]));
    defparam rx_step_i2.GSR = "DISABLED";
    FD1P3AX rx_step_i1 (.D(n201[1]), .SP(n29053), .CK(clk_c), .Q(rx_step[1]));
    defparam rx_step_i1.GSR = "DISABLED";
    LUT4 mux_13_i1_4_lut (.A(n36[0]), .B(rx_baud_count[1]), .C(n30124), 
         .D(n2476), .Z(n85[0])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i1_4_lut.init = 16'hc0ca;
    FD1S3AX rx_clk_count_i0 (.D(n85[0]), .CK(clk_c), .Q(rx_clk_count[0]));
    defparam rx_clk_count_i0.GSR = "DISABLED";
    FD1S3IX rx_samp_clk_54 (.D(n81), .CK(clk_c), .CD(n30124), .Q(rx_samp_clk));
    defparam rx_samp_clk_54.GSR = "DISABLED";
    FD1P3IX rx_ok_buf_55 (.D(n30282), .SP(n16395), .CD(n5), .CK(clk_c), 
            .Q(rx_ok));
    defparam rx_ok_buf_55.GSR = "DISABLED";
    FD1S3IX rx_clk_count_i13 (.D(n36[13]), .CK(clk_c), .CD(n3631), .Q(rx_clk_count[13]));
    defparam rx_clk_count_i13.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i12 (.D(n85[12]), .CK(clk_c), .Q(rx_clk_count[12]));
    defparam rx_clk_count_i12.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i11 (.D(n85[11]), .CK(clk_c), .Q(rx_clk_count[11]));
    defparam rx_clk_count_i11.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i10 (.D(n85[10]), .CK(clk_c), .Q(rx_clk_count[10]));
    defparam rx_clk_count_i10.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i9 (.D(n85[9]), .CK(clk_c), .Q(rx_clk_count[9]));
    defparam rx_clk_count_i9.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i8 (.D(n85[8]), .CK(clk_c), .Q(rx_clk_count[8]));
    defparam rx_clk_count_i8.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i7 (.D(n85[7]), .CK(clk_c), .Q(rx_clk_count[7]));
    defparam rx_clk_count_i7.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i6 (.D(n85[6]), .CK(clk_c), .Q(rx_clk_count[6]));
    defparam rx_clk_count_i6.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i5 (.D(n85[5]), .CK(clk_c), .Q(rx_clk_count[5]));
    defparam rx_clk_count_i5.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i4 (.D(n85[4]), .CK(clk_c), .Q(rx_clk_count[4]));
    defparam rx_clk_count_i4.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i3 (.D(n85[3]), .CK(clk_c), .Q(rx_clk_count[3]));
    defparam rx_clk_count_i3.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i2 (.D(n85[2]), .CK(clk_c), .Q(rx_clk_count[2]));
    defparam rx_clk_count_i2.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i1 (.D(n85[1]), .CK(clk_c), .Q(rx_clk_count[1]));
    defparam rx_clk_count_i1.GSR = "DISABLED";
    LUT4 i1_2_lut_4_lut_adj_10 (.A(n30133), .B(rx_samp_clk), .C(rx_step[1]), 
         .D(n29479), .Z(n29480)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i1_2_lut_4_lut_adj_10.init = 16'h0800;
    LUT4 i2_3_lut_4_lut (.A(rx_step[3]), .B(n30170), .C(n17071), .D(n5), 
         .Z(n29053)) /* synthesis lut_function=(A (C+(D))+!A (B+(C+(D)))) */ ;
    defparam i2_3_lut_4_lut.init = 16'hfff4;
    FD1P3IX rx_step_i0 (.D(n30207), .SP(n17071), .CD(n5), .CK(clk_c), 
            .Q(rx_step[0]));
    defparam rx_step_i0.GSR = "DISABLED";
    CCU2D equal_9_0 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(n66[13]), .B1(rx_clk_count[13]), .C1(n66[12]), .D1(rx_clk_count[12]), 
          .COUT(n27852));
    defparam equal_9_0.INIT0 = 16'hF000;
    defparam equal_9_0.INIT1 = 16'h9009;
    defparam equal_9_0.INJECT1_0 = "NO";
    defparam equal_9_0.INJECT1_1 = "YES";
    FD1P3AX baud_back_i0_i2 (.D(to_db9_baud[2]), .SP(n5), .CK(clk_c), 
            .Q(baud_back[2]));
    defparam baud_back_i0_i2.GSR = "DISABLED";
    FD1P3AX baud_back_i0_i1 (.D(to_db9_baud[1]), .SP(n5), .CK(clk_c), 
            .Q(baud_back[1]));
    defparam baud_back_i0_i1.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i7 (.D(n1175), .SP(n29480), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[7]));
    defparam rx_dat_buf_i0_i7.GSR = "DISABLED";
    LUT4 i1_4_lut (.A(baud_back[2]), .B(baud_back[0]), .C(to_db9_baud[2]), 
         .D(to_db9_baud[0]), .Z(n4)) /* synthesis lut_function=(!(A (B (C (D))+!B !((D)+!C))+!A !(B (C+!(D))+!B (C+(D))))) */ ;
    defparam i1_4_lut.init = 16'h7bde;
    LUT4 i2_3_lut (.A(baud_back[1]), .B(n4), .C(to_db9_baud[1]), .Z(n5)) /* synthesis lut_function=(A (B+!(C))+!A (B+(C))) */ ;
    defparam i2_3_lut.init = 16'hdede;
    FD1P3IX rx_dat_buf_i0_i6 (.D(n1175), .SP(n29541), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[6]));
    defparam rx_dat_buf_i0_i6.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i5 (.D(n1175), .SP(n29481), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[5]));
    defparam rx_dat_buf_i0_i5.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i4 (.D(n1175), .SP(n29537), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[4]));
    defparam rx_dat_buf_i0_i4.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i3 (.D(n1175), .SP(n29465), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[3]));
    defparam rx_dat_buf_i0_i3.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i2 (.D(n1175), .SP(n28914), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[2]));
    defparam rx_dat_buf_i0_i2.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i1 (.D(n1175), .SP(n29466), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[1]));
    defparam rx_dat_buf_i0_i1.GSR = "DISABLED";
    LUT4 i1_2_lut_3_lut_4_lut (.A(n30150), .B(rx_step[3]), .C(n29464), 
         .D(n30152), .Z(n29466)) /* synthesis lut_function=(!(A (B+!(C (D)))+!A !(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'h6000;
    LUT4 i1_2_lut_3_lut_4_lut_adj_11 (.A(n30150), .B(rx_step[3]), .C(n29479), 
         .D(n30152), .Z(n29481)) /* synthesis lut_function=(!(A (B+!(C (D)))+!A !(B (C (D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_11.init = 16'h6000;
    CCU2D sub_8_add_2_15 (.A0(rx_baud_count[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28852), .S0(n66[13]));
    defparam sub_8_add_2_15.INIT0 = 16'h5555;
    defparam sub_8_add_2_15.INIT1 = 16'h0000;
    defparam sub_8_add_2_15.INJECT1_0 = "NO";
    defparam sub_8_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_13 (.A0(rx_baud_count[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28851), .COUT(n28852), .S0(n66[11]), .S1(n66[12]));
    defparam sub_8_add_2_13.INIT0 = 16'h5555;
    defparam sub_8_add_2_13.INIT1 = 16'h5555;
    defparam sub_8_add_2_13.INJECT1_0 = "NO";
    defparam sub_8_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_11 (.A0(rx_baud_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28850), .COUT(n28851), .S0(n66[9]), .S1(n66[10]));
    defparam sub_8_add_2_11.INIT0 = 16'h5555;
    defparam sub_8_add_2_11.INIT1 = 16'h5555;
    defparam sub_8_add_2_11.INJECT1_0 = "NO";
    defparam sub_8_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_9 (.A0(rx_baud_count[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28849), .COUT(n28850), .S0(n66[7]), .S1(n66[8]));
    defparam sub_8_add_2_9.INIT0 = 16'h5555;
    defparam sub_8_add_2_9.INIT1 = 16'h5555;
    defparam sub_8_add_2_9.INJECT1_0 = "NO";
    defparam sub_8_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_7 (.A0(rx_baud_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28848), .COUT(n28849), .S0(n66[5]), .S1(n66[6]));
    defparam sub_8_add_2_7.INIT0 = 16'h5555;
    defparam sub_8_add_2_7.INIT1 = 16'h5555;
    defparam sub_8_add_2_7.INJECT1_0 = "NO";
    defparam sub_8_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_5 (.A0(rx_baud_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28847), .COUT(n28848), .S0(n66[3]), .S1(n66[4]));
    defparam sub_8_add_2_5.INIT0 = 16'h5555;
    defparam sub_8_add_2_5.INIT1 = 16'h5555;
    defparam sub_8_add_2_5.INJECT1_0 = "NO";
    defparam sub_8_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_3 (.A0(rx_baud_count[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28846), .COUT(n28847), .S0(n66[1]), .S1(n66[2]));
    defparam sub_8_add_2_3.INIT0 = 16'h5555;
    defparam sub_8_add_2_3.INIT1 = 16'h5555;
    defparam sub_8_add_2_3.INJECT1_0 = "NO";
    defparam sub_8_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_baud_count[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28846), .S1(n66[0]));
    defparam sub_8_add_2_1.INIT0 = 16'hF000;
    defparam sub_8_add_2_1.INIT1 = 16'h5555;
    defparam sub_8_add_2_1.INJECT1_0 = "NO";
    defparam sub_8_add_2_1.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_15 (.A0(rx_clk_count[13]), .B0(rx_baud_count[13]), 
          .C0(GND_net), .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28824), .S1(n2476));
    defparam sub_1291_add_2_15.INIT0 = 16'h5999;
    defparam sub_1291_add_2_15.INIT1 = 16'h0000;
    defparam sub_1291_add_2_15.INJECT1_0 = "NO";
    defparam sub_1291_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_13 (.A0(rx_clk_count[11]), .B0(rx_baud_count[11]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[12]), .B1(rx_baud_count[12]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28823), .COUT(n28824));
    defparam sub_1291_add_2_13.INIT0 = 16'h5999;
    defparam sub_1291_add_2_13.INIT1 = 16'h5999;
    defparam sub_1291_add_2_13.INJECT1_0 = "NO";
    defparam sub_1291_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_11 (.A0(rx_clk_count[9]), .B0(rx_baud_count[9]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[10]), .B1(rx_baud_count[10]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28822), .COUT(n28823));
    defparam sub_1291_add_2_11.INIT0 = 16'h5999;
    defparam sub_1291_add_2_11.INIT1 = 16'h5999;
    defparam sub_1291_add_2_11.INJECT1_0 = "NO";
    defparam sub_1291_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_9 (.A0(rx_clk_count[7]), .B0(rx_baud_count[7]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[8]), .B1(rx_baud_count[8]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28821), .COUT(n28822));
    defparam sub_1291_add_2_9.INIT0 = 16'h5999;
    defparam sub_1291_add_2_9.INIT1 = 16'h5999;
    defparam sub_1291_add_2_9.INJECT1_0 = "NO";
    defparam sub_1291_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_7 (.A0(rx_clk_count[5]), .B0(rx_baud_count[5]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[6]), .B1(rx_baud_count[6]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28820), .COUT(n28821));
    defparam sub_1291_add_2_7.INIT0 = 16'h5999;
    defparam sub_1291_add_2_7.INIT1 = 16'h5999;
    defparam sub_1291_add_2_7.INJECT1_0 = "NO";
    defparam sub_1291_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_5 (.A0(rx_clk_count[3]), .B0(rx_baud_count[3]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[4]), .B1(rx_baud_count[4]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28819), .COUT(n28820));
    defparam sub_1291_add_2_5.INIT0 = 16'h5999;
    defparam sub_1291_add_2_5.INIT1 = 16'h5999;
    defparam sub_1291_add_2_5.INJECT1_0 = "NO";
    defparam sub_1291_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_3 (.A0(rx_clk_count[1]), .B0(rx_baud_count[1]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[2]), .B1(rx_baud_count[2]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28818), .COUT(n28819));
    defparam sub_1291_add_2_3.INIT0 = 16'h5999;
    defparam sub_1291_add_2_3.INIT1 = 16'h5999;
    defparam sub_1291_add_2_3.INJECT1_0 = "NO";
    defparam sub_1291_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_1291_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[0]), .B1(rx_baud_count[0]), .C1(GND_net), 
          .D1(GND_net), .COUT(n28818));
    defparam sub_1291_add_2_1.INIT0 = 16'h0000;
    defparam sub_1291_add_2_1.INIT1 = 16'h5999;
    defparam sub_1291_add_2_1.INJECT1_0 = "NO";
    defparam sub_1291_add_2_1.INJECT1_1 = "NO";
    CCU2D equal_9_14 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n27855), 
          .S0(n81));
    defparam equal_9_14.INIT0 = 16'hFFFF;
    defparam equal_9_14.INIT1 = 16'h0000;
    defparam equal_9_14.INJECT1_0 = "NO";
    defparam equal_9_14.INJECT1_1 = "NO";
    LUT4 i28011_2_lut_2_lut_3_lut_4_lut (.A(n30133), .B(n30152), .C(n30149), 
         .D(rx_step[0]), .Z(n16729)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i28011_2_lut_2_lut_3_lut_4_lut.init = 16'h0080;
    CCU2D add_6_15 (.A0(rx_clk_count[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28803), .S0(n36[13]));
    defparam add_6_15.INIT0 = 16'h5aaa;
    defparam add_6_15.INIT1 = 16'h0000;
    defparam add_6_15.INJECT1_0 = "NO";
    defparam add_6_15.INJECT1_1 = "NO";
    LUT4 i27991_2_lut_2_lut_3_lut_4_lut (.A(n30133), .B(n30152), .C(n30149), 
         .D(rx_step[0]), .Z(n29537)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;
    defparam i27991_2_lut_2_lut_3_lut_4_lut.init = 16'h0008;
    CCU2D add_6_13 (.A0(rx_clk_count[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28802), .COUT(n28803), .S0(n36[11]), .S1(n36[12]));
    defparam add_6_13.INIT0 = 16'h5aaa;
    defparam add_6_13.INIT1 = 16'h5aaa;
    defparam add_6_13.INJECT1_0 = "NO";
    defparam add_6_13.INJECT1_1 = "NO";
    CCU2D add_6_11 (.A0(rx_clk_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28801), .COUT(n28802), .S0(n36[9]), .S1(n36[10]));
    defparam add_6_11.INIT0 = 16'h5aaa;
    defparam add_6_11.INIT1 = 16'h5aaa;
    defparam add_6_11.INJECT1_0 = "NO";
    defparam add_6_11.INJECT1_1 = "NO";
    CCU2D add_6_9 (.A0(rx_clk_count[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28800), .COUT(n28801), .S0(n36[7]), .S1(n36[8]));
    defparam add_6_9.INIT0 = 16'h5aaa;
    defparam add_6_9.INIT1 = 16'h5aaa;
    defparam add_6_9.INJECT1_0 = "NO";
    defparam add_6_9.INJECT1_1 = "NO";
    LUT4 i28051_3_lut_rep_93_4_lut (.A(rx_step[3]), .B(n30150), .C(n1175), 
         .D(rx_step[0]), .Z(n30124)) /* synthesis lut_function=(!(A+(B+(C+(D))))) */ ;
    defparam i28051_3_lut_rep_93_4_lut.init = 16'h0001;
    CCU2D add_6_7 (.A0(rx_clk_count[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28799), .COUT(n28800), .S0(n36[5]), .S1(n36[6]));
    defparam add_6_7.INIT0 = 16'h5aaa;
    defparam add_6_7.INIT1 = 16'h5aaa;
    defparam add_6_7.INJECT1_0 = "NO";
    defparam add_6_7.INJECT1_1 = "NO";
    CCU2D add_6_5 (.A0(rx_clk_count[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28798), .COUT(n28799), .S0(n36[3]), .S1(n36[4]));
    defparam add_6_5.INIT0 = 16'h5aaa;
    defparam add_6_5.INIT1 = 16'h5aaa;
    defparam add_6_5.INJECT1_0 = "NO";
    defparam add_6_5.INJECT1_1 = "NO";
    CCU2D add_6_3 (.A0(rx_clk_count[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28797), .COUT(n28798), .S0(n36[1]), .S1(n36[2]));
    defparam add_6_3.INIT0 = 16'h5aaa;
    defparam add_6_3.INIT1 = 16'h5aaa;
    defparam add_6_3.INJECT1_0 = "NO";
    defparam add_6_3.INJECT1_1 = "NO";
    CCU2D add_6_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28797), .S1(n36[0]));
    defparam add_6_1.INIT0 = 16'hF000;
    defparam add_6_1.INIT1 = 16'h5555;
    defparam add_6_1.INJECT1_0 = "NO";
    defparam add_6_1.INJECT1_1 = "NO";
    LUT4 i1_3_lut (.A(rx_step[0]), .B(rx_step[1]), .C(rx_samp_clk), .Z(n1707[1])) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;
    defparam i1_3_lut.init = 16'h2020;
    LUT4 i14817_4_lut (.A(n30210), .B(n5), .C(n1707[1]), .D(rx_step[3]), 
         .Z(n201[1])) /* synthesis lut_function=(!(A (B+!(C+!(D)))+!A (B+!(C (D))))) */ ;
    defparam i14817_4_lut.init = 16'h3022;
    LUT4 i1_2_lut_3_lut_4_lut_adj_12 (.A(rx_step[2]), .B(rx_step[1]), .C(n17071), 
         .D(rx_step[3]), .Z(n75)) /* synthesis lut_function=(A (B (C+!(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut_adj_12.init = 16'hf0f8;
    LUT4 i1_2_lut_rep_139 (.A(rx_step[2]), .B(rx_step[1]), .Z(n30170)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_rep_139.init = 16'h8888;
    LUT4 i2099_2_lut_4_lut (.A(n30129), .B(rx_step[0]), .C(n1175), .D(n2476), 
         .Z(n3631)) /* synthesis lut_function=(A (D)+!A (B (D)+!B ((D)+!C))) */ ;
    defparam i2099_2_lut_4_lut.init = 16'hff01;
    LUT4 mux_13_i2_4_lut (.A(n36[1]), .B(rx_baud_count[2]), .C(n30124), 
         .D(n2476), .Z(n85[1])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i2_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i3_4_lut (.A(n36[2]), .B(rx_baud_count[3]), .C(n30124), 
         .D(n2476), .Z(n85[2])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i3_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i4_4_lut (.A(n36[3]), .B(rx_baud_count[4]), .C(n30124), 
         .D(n2476), .Z(n85[3])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i4_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i5_4_lut (.A(n36[4]), .B(rx_baud_count[5]), .C(n30124), 
         .D(n2476), .Z(n85[4])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i5_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i6_4_lut (.A(n36[5]), .B(rx_baud_count[6]), .C(n30124), 
         .D(n2476), .Z(n85[5])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i6_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i7_4_lut (.A(n36[6]), .B(rx_baud_count[7]), .C(n30124), 
         .D(n2476), .Z(n85[6])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i7_4_lut.init = 16'hc0ca;
    PFUMX i28245 (.BLUT(n30281), .ALUT(n30279), .C0(rx_step[1]), .Z(n30282));
    LUT4 mux_13_i8_4_lut (.A(n36[7]), .B(rx_baud_count[8]), .C(n30124), 
         .D(n2476), .Z(n85[7])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i8_4_lut.init = 16'hc0ca;
    LUT4 rx_step_1__bdd_3_lut (.A(rx_step[0]), .B(rx_ok), .C(rx_step[3]), 
         .Z(n30279)) /* synthesis lut_function=(A (B)+!A !((C)+!B)) */ ;
    defparam rx_step_1__bdd_3_lut.init = 16'h8c8c;
    LUT4 rx_step_0__bdd_4_lut (.A(rx_step[0]), .B(rx_ok), .C(rx_samp_clk), 
         .D(rx_step[3]), .Z(n30280)) /* synthesis lut_function=(A (B+(C (D)))+!A (B (D))) */ ;
    defparam rx_step_0__bdd_4_lut.init = 16'hec88;
    LUT4 mux_13_i9_4_lut (.A(n36[8]), .B(rx_baud_count[9]), .C(n30124), 
         .D(n2476), .Z(n85[8])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i9_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i10_4_lut (.A(n36[9]), .B(rx_baud_count[10]), .C(n30124), 
         .D(n2476), .Z(n85[9])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i10_4_lut.init = 16'hc0ca;
    PFUMX i28204 (.BLUT(n30208), .ALUT(n30209), .C0(rx_step[0]), .Z(n30210));
    LUT4 mux_13_i11_4_lut (.A(n36[10]), .B(rx_baud_count[11]), .C(n30124), 
         .D(n2476), .Z(n85[10])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i11_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i12_4_lut (.A(n36[11]), .B(rx_baud_count[12]), .C(n30124), 
         .D(n2476), .Z(n85[11])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i12_4_lut.init = 16'hc0ca;
    LUT4 mux_718_i2_else_4_lut (.A(rx_step[1]), .Z(n30208)) /* synthesis lut_function=(A) */ ;
    defparam mux_718_i2_else_4_lut.init = 16'haaaa;
    LUT4 mux_13_i13_4_lut (.A(n36[12]), .B(rx_baud_count[13]), .C(n30124), 
         .D(n2476), .Z(n85[12])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i13_4_lut.init = 16'hc0ca;
    LUT4 mux_718_i2_then_4_lut (.A(rx_step[1]), .B(rx_step[2]), .C(rx_samp_clk), 
         .D(n1175), .Z(n30209)) /* synthesis lut_function=(!(A (C)+!A !(B (C)+!B !((D)+!C)))) */ ;
    defparam mux_718_i2_then_4_lut.init = 16'h4a5a;
    PFUMX i28202 (.BLUT(n30205), .ALUT(n30206), .C0(rx_step[1]), .Z(n30207));
    LUT4 i28067_4_lut_else_3_lut (.A(rx_samp_clk), .B(rx_step[0]), .C(rx_step[3]), 
         .D(rx_step[2]), .Z(n30205)) /* synthesis lut_function=(!(A (B)+!A !(B+!(C+(D))))) */ ;
    defparam i28067_4_lut_else_3_lut.init = 16'h6667;
    LUT4 i28067_4_lut_then_3_lut (.A(rx_samp_clk), .B(rx_step[0]), .C(rx_step[3]), 
         .Z(n30206)) /* synthesis lut_function=(!(A (B+(C))+!A ((C)+!B))) */ ;
    defparam i28067_4_lut_then_3_lut.init = 16'h0606;
    CCU2D equal_9_14_26288 (.A0(n66[3]), .B0(rx_clk_count[3]), .C0(n66[2]), 
          .D0(rx_clk_count[2]), .A1(n66[1]), .B1(rx_clk_count[1]), .C1(n66[0]), 
          .D1(rx_clk_count[0]), .CIN(n27854), .COUT(n27855));
    defparam equal_9_14_26288.INIT0 = 16'h9009;
    defparam equal_9_14_26288.INIT1 = 16'h9009;
    defparam equal_9_14_26288.INJECT1_0 = "YES";
    defparam equal_9_14_26288.INJECT1_1 = "YES";
    CCU2D equal_9_12 (.A0(n66[7]), .B0(rx_clk_count[7]), .C0(n66[6]), 
          .D0(rx_clk_count[6]), .A1(n66[5]), .B1(rx_clk_count[5]), .C1(n66[4]), 
          .D1(rx_clk_count[4]), .CIN(n27853), .COUT(n27854));
    defparam equal_9_12.INIT0 = 16'h9009;
    defparam equal_9_12.INIT1 = 16'h9009;
    defparam equal_9_12.INJECT1_0 = "YES";
    defparam equal_9_12.INJECT1_1 = "YES";
    PFUMX i28194 (.BLUT(n30193), .ALUT(n30194), .C0(rx_step[0]), .Z(n17071));
    LUT4 i28150_else_4_lut (.A(n1175), .B(rx_step[1]), .C(rx_step[2]), 
         .D(rx_step[3]), .Z(n30193)) /* synthesis lut_function=(!(A (B (C (D))+!B (C (D)+!C !(D)))+!A (B (D)+!B (C (D))))) */ ;
    defparam i28150_else_4_lut.init = 16'h0bfd;
    LUT4 i28150_then_4_lut (.A(rx_step[1]), .B(rx_step[2]), .C(rx_step[3]), 
         .D(rx_samp_clk), .Z(n30194)) /* synthesis lut_function=(!(A (C)+!A (B (C)+!B !(C+(D))))) */ ;
    defparam i28150_then_4_lut.init = 16'h1f1e;
    PFUMX i28192 (.BLUT(n30190), .ALUT(n30191), .C0(rx_step[2]), .Z(n30192));
    LUT4 i1_4_lut_4_lut_else_4_lut (.A(rx_step[1]), .B(rx_samp_clk), .C(rx_step[3]), 
         .D(rx_step[0]), .Z(n30190)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i1_4_lut_4_lut_else_4_lut.init = 16'h0800;
    LUT4 i27858_2_lut_3_lut (.A(rx_step[1]), .B(rx_samp_clk), .C(rx_step[0]), 
         .Z(n29624)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i27858_2_lut_3_lut.init = 16'h8080;
    LUT4 i27829_2_lut_rep_121 (.A(rx_step[1]), .B(rx_samp_clk), .Z(n30152)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27829_2_lut_rep_121.init = 16'h8888;
    LUT4 i1_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), .C(rx_step[3]), 
         .D(n29624), .Z(n1732[3])) /* synthesis lut_function=(!(A (B (C+!(D))+!B (C (D)+!C !(D)))+!A (B+!(C)))) */ ;
    defparam i1_4_lut_4_lut.init = 16'h1a30;
    LUT4 i1_4_lut_4_lut_then_4_lut (.A(rx_step[1]), .B(rx_samp_clk), .C(rx_step[3]), 
         .D(rx_step[0]), .Z(n30191)) /* synthesis lut_function=(((C+!(D))+!B)+!A) */ ;
    defparam i1_4_lut_4_lut_then_4_lut.init = 16'hf7ff;
    
endmodule

//
// Verilog Description of module fifo64x8
//

module fifo64x8 (rx_dat, clk_c, rx_ok, rden, n134, tx_dat, empty, 
            GND_net, VCC_net) /* synthesis syn_module_defined=1 */ ;
    input [7:0]rx_dat;
    input clk_c;
    input rx_ok;
    input rden;
    input n134;
    output [7:0]tx_dat;
    output empty;
    input GND_net;
    input VCC_net;
    
    wire Full;   // g:/zjf/mhub4k862-cpld/fifo64x8.v(19[17:21])
    
    FIFO8KB fifo64x8_0_0 (.DI0(rx_dat[0]), .DI1(rx_dat[1]), .DI2(rx_dat[2]), 
            .DI3(rx_dat[3]), .DI4(rx_dat[4]), .DI5(rx_dat[5]), .DI6(rx_dat[6]), 
            .DI7(rx_dat[7]), .DI8(GND_net), .DI9(GND_net), .DI10(GND_net), 
            .DI11(GND_net), .DI12(GND_net), .DI13(GND_net), .DI14(GND_net), 
            .DI15(GND_net), .DI16(GND_net), .DI17(GND_net), .FULLI(Full), 
            .EMPTYI(empty), .CSW1(VCC_net), .CSW0(VCC_net), .CSR1(VCC_net), 
            .CSR0(VCC_net), .WE(rx_ok), .RE(rden), .ORE(rden), .CLKW(clk_c), 
            .CLKR(clk_c), .RST(n134), .RPRST(n134), .DO9(tx_dat[0]), 
            .DO10(tx_dat[1]), .DO11(tx_dat[2]), .DO12(tx_dat[3]), .DO13(tx_dat[4]), 
            .DO14(tx_dat[5]), .DO15(tx_dat[6]), .DO16(tx_dat[7]), .EF(empty), 
            .FF(Full)) /* synthesis syn_instantiated=1 */ ;   // g:/zjf/mhub4k862-cpld/new_uart/pc2mcu_uart_top.v(31[10] 45[2])
    defparam fifo64x8_0_0.DATA_WIDTH_W = 18;
    defparam fifo64x8_0_0.DATA_WIDTH_R = 18;
    defparam fifo64x8_0_0.REGMODE = "NOREG";
    defparam fifo64x8_0_0.RESETMODE = "ASYNC";
    defparam fifo64x8_0_0.ASYNC_RESET_RELEASE = "SYNC";
    defparam fifo64x8_0_0.CSDECODE_W = "0b11";
    defparam fifo64x8_0_0.CSDECODE_R = "0b11";
    defparam fifo64x8_0_0.AEPOINTER = "0b00000001010000";
    defparam fifo64x8_0_0.AEPOINTER1 = "0b00000001100000";
    defparam fifo64x8_0_0.AFPOINTER = "0b00001111000000";
    defparam fifo64x8_0_0.AFPOINTER1 = "0b00001110110000";
    defparam fifo64x8_0_0.FULLPOINTER = "0b10000000000000";
    defparam fifo64x8_0_0.FULLPOINTER1 = "0b01111111110000";
    defparam fifo64x8_0_0.GSR = "DISABLED";
    
endmodule

//
// Verilog Description of module check_IR_defv_U11
//

module check_IR_defv_U11 (ir_402x_i_c_6, \clock_count[15] , \def_voltag[6] , 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input ir_402x_i_c_6;
    input \clock_count[15] ;
    output \def_voltag[6] ;
    input rstn_c;
    
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    
    wire n29575, n9, n6, n3995, n1909;
    wire [4:0]n26;
    
    wire n4437, n1, n4219, n30171;
    wire [4:0]n26_adj_33;
    
    wire n30167;
    
    LUT4 i27810_2_lut (.A(H_count[3]), .B(H_count[0]), .Z(n29575)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27810_2_lut.init = 16'h8888;
    LUT4 i4_4_lut (.A(n29575), .B(H_count[2]), .C(H_count[1]), .D(H_count[4]), 
         .Z(n9)) /* synthesis lut_function=((B+(C+!(D)))+!A) */ ;
    defparam i4_4_lut.init = 16'hfdff;
    LUT4 i1_2_lut (.A(L_count[4]), .B(L_count[2]), .Z(n6)) /* synthesis lut_function=((B)+!A) */ ;
    defparam i1_2_lut.init = 16'hdddd;
    LUT4 i4_4_lut_adj_8 (.A(L_count[1]), .B(L_count[3]), .C(L_count[0]), 
         .D(n6), .Z(n3995)) /* synthesis lut_function=(A+(((D)+!C)+!B)) */ ;
    defparam i4_4_lut_adj_8.init = 16'hffbf;
    LUT4 i890_3_lut (.A(ir_402x_i_c_6), .B(n3995), .C(n9), .Z(n1909)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i890_3_lut.init = 16'hbaba;
    FD1S3IX L_count_1496__i0 (.D(n26[0]), .CK(\clock_count[15] ), .CD(n1909), 
            .Q(L_count[0]));
    defparam L_count_1496__i0.GSR = "ENABLED";
    FD1S3IX H_count_1495__i0 (.D(n1), .CK(\clock_count[15] ), .CD(n4437), 
            .Q(H_count[0]));
    defparam H_count_1495__i0.GSR = "ENABLED";
    FD1P3AX defv_23 (.D(n9), .SP(n4219), .CK(\clock_count[15] ), .Q(\def_voltag[6] ));
    defparam defv_23.GSR = "DISABLED";
    LUT4 i27129_3_lut_4_lut (.A(H_count[2]), .B(n30171), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26_adj_33[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i27129_3_lut_4_lut.init = 16'h7f80;
    LUT4 i26816_3_lut_4_lut (.A(L_count[2]), .B(n30167), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26816_3_lut_4_lut.init = 16'h7f80;
    FD1S3IX L_count_1496__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n1909), 
            .Q(L_count[4]));
    defparam L_count_1496__i4.GSR = "ENABLED";
    FD1S3IX L_count_1496__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n1909), 
            .Q(L_count[3]));
    defparam L_count_1496__i3.GSR = "ENABLED";
    FD1S3IX L_count_1496__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n1909), 
            .Q(L_count[2]));
    defparam L_count_1496__i2.GSR = "ENABLED";
    FD1S3IX L_count_1496__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n1909), 
            .Q(L_count[1]));
    defparam L_count_1496__i1.GSR = "ENABLED";
    FD1S3IX H_count_1495__i1 (.D(n26_adj_33[1]), .CK(\clock_count[15] ), 
            .CD(n4437), .Q(H_count[1]));
    defparam H_count_1495__i1.GSR = "ENABLED";
    FD1S3IX H_count_1495__i2 (.D(n26_adj_33[2]), .CK(\clock_count[15] ), 
            .CD(n4437), .Q(H_count[2]));
    defparam H_count_1495__i2.GSR = "ENABLED";
    FD1S3IX H_count_1495__i3 (.D(n26_adj_33[3]), .CK(\clock_count[15] ), 
            .CD(n4437), .Q(H_count[3]));
    defparam H_count_1495__i3.GSR = "ENABLED";
    FD1S3IX H_count_1495__i4 (.D(n26_adj_33[4]), .CK(\clock_count[15] ), 
            .CD(n4437), .Q(H_count[4]));
    defparam H_count_1495__i4.GSR = "ENABLED";
    LUT4 i28048_2_lut (.A(ir_402x_i_c_6), .B(n9), .Z(n4437)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i28048_2_lut.init = 16'h7777;
    LUT4 i1_3_lut (.A(n3995), .B(rstn_c), .C(n9), .Z(n4219)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    LUT4 i26795_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26795_2_lut.init = 16'h6666;
    INV i1830 (.A(H_count[0]), .Z(n1));
    LUT4 i27108_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26_adj_33[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i27108_2_lut.init = 16'h6666;
    LUT4 i27122_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26_adj_33[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i27122_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i27115_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26_adj_33[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i27115_2_lut_3_lut.init = 16'h7878;
    LUT4 i27111_2_lut_rep_140 (.A(H_count[1]), .B(H_count[0]), .Z(n30171)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27111_2_lut_rep_140.init = 16'h8888;
    LUT4 i26809_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26809_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26802_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26802_2_lut_3_lut.init = 16'h7878;
    LUT4 i26798_2_lut_rep_136 (.A(L_count[1]), .B(L_count[0]), .Z(n30167)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26798_2_lut_rep_136.init = 16'h8888;
    INV i26793 (.A(L_count[0]), .Z(n26[0]));
    
endmodule

//
// Verilog Description of module ir_car_detect_U6
//

module ir_car_detect_U6 (clk_c, \f2n_irx_temp[3] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[3] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n24;
    
    wire n28745;
    wire [11:0]n11;
    
    wire n28744, n28743, n28742, n28741, n28740;
    
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[3] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28745), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28744), .COUT(n28745), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28743), .COUT(n28744), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28742), .COUT(n28743), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28741), .COUT(n28742), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28740), .COUT(n28741), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28740), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i14818_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14818_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    LUT4 i15010_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15010_2_lut.init = 16'heeee;
    LUT4 i15009_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15009_2_lut.init = 16'heeee;
    LUT4 i15008_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15008_2_lut.init = 16'heeee;
    LUT4 i15007_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15007_2_lut.init = 16'heeee;
    LUT4 i15006_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15006_2_lut.init = 16'heeee;
    LUT4 i15005_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15005_2_lut.init = 16'heeee;
    LUT4 i15004_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15004_2_lut.init = 16'heeee;
    LUT4 i15003_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15003_2_lut.init = 16'heeee;
    LUT4 i15002_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15002_2_lut.init = 16'heeee;
    LUT4 i15001_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15001_2_lut.init = 16'heeee;
    LUT4 i15000_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i15000_2_lut.init = 16'heeee;
    
endmodule

//
// Verilog Description of module check_IR_defv_U15
//

module check_IR_defv_U15 (ir_402x_i_c_2, \def_voltag[2] , \clock_count[15] , 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input ir_402x_i_c_2;
    output \def_voltag[2] ;
    input \clock_count[15] ;
    input rstn_c;
    
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    
    wire n30189;
    wire [4:0]n26;
    
    wire n9, n4422, n30173;
    wire [4:0]n26_adj_28;
    
    wire n4204, n1873, n1, n3977, n29589, n29567;
    
    LUT4 i26589_3_lut_4_lut (.A(H_count[2]), .B(n30189), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26589_3_lut_4_lut.init = 16'h7f80;
    LUT4 i27978_2_lut (.A(ir_402x_i_c_2), .B(n9), .Z(n4422)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i27978_2_lut.init = 16'h7777;
    LUT4 i26960_3_lut_4_lut (.A(L_count[2]), .B(n30173), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26_adj_28[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26960_3_lut_4_lut.init = 16'h7f80;
    FD1P3AX defv_23 (.D(n9), .SP(n4204), .CK(\clock_count[15] ), .Q(\def_voltag[2] ));
    defparam defv_23.GSR = "DISABLED";
    FD1S3IX L_count_1484__i0 (.D(n26_adj_28[0]), .CK(\clock_count[15] ), 
            .CD(n1873), .Q(L_count[0]));
    defparam L_count_1484__i0.GSR = "ENABLED";
    FD1S3IX H_count_1483__i0 (.D(n1), .CK(\clock_count[15] ), .CD(n4422), 
            .Q(H_count[0]));
    defparam H_count_1483__i0.GSR = "ENABLED";
    FD1S3IX H_count_1483__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n4422), 
            .Q(H_count[4]));
    defparam H_count_1483__i4.GSR = "ENABLED";
    FD1S3IX H_count_1483__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n4422), 
            .Q(H_count[3]));
    defparam H_count_1483__i3.GSR = "ENABLED";
    FD1S3IX H_count_1483__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n4422), 
            .Q(H_count[2]));
    defparam H_count_1483__i2.GSR = "ENABLED";
    FD1S3IX H_count_1483__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n4422), 
            .Q(H_count[1]));
    defparam H_count_1483__i1.GSR = "ENABLED";
    FD1S3IX L_count_1484__i1 (.D(n26_adj_28[1]), .CK(\clock_count[15] ), 
            .CD(n1873), .Q(L_count[1]));
    defparam L_count_1484__i1.GSR = "ENABLED";
    FD1S3IX L_count_1484__i2 (.D(n26_adj_28[2]), .CK(\clock_count[15] ), 
            .CD(n1873), .Q(L_count[2]));
    defparam L_count_1484__i2.GSR = "ENABLED";
    FD1S3IX L_count_1484__i3 (.D(n26_adj_28[3]), .CK(\clock_count[15] ), 
            .CD(n1873), .Q(L_count[3]));
    defparam L_count_1484__i3.GSR = "ENABLED";
    FD1S3IX L_count_1484__i4 (.D(n26_adj_28[4]), .CK(\clock_count[15] ), 
            .CD(n1873), .Q(L_count[4]));
    defparam L_count_1484__i4.GSR = "ENABLED";
    LUT4 i26582_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26582_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26575_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26575_2_lut_3_lut.init = 16'h7878;
    LUT4 i26571_2_lut_rep_158 (.A(H_count[1]), .B(H_count[0]), .Z(n30189)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26571_2_lut_rep_158.init = 16'h8888;
    LUT4 i854_3_lut (.A(ir_402x_i_c_2), .B(n3977), .C(n9), .Z(n1873)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i854_3_lut.init = 16'hbaba;
    LUT4 i27823_2_lut (.A(L_count[4]), .B(L_count[3]), .Z(n29589)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27823_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(L_count[2]), .B(L_count[0]), .C(L_count[1]), .D(n29589), 
         .Z(n3977)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut.init = 16'hfbff;
    LUT4 i27802_2_lut (.A(H_count[4]), .B(H_count[3]), .Z(n29567)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27802_2_lut.init = 16'h8888;
    LUT4 i3_4_lut_adj_7 (.A(H_count[1]), .B(H_count[2]), .C(H_count[0]), 
         .D(n29567), .Z(n9)) /* synthesis lut_function=(A+(B+!(C (D)))) */ ;
    defparam i3_4_lut_adj_7.init = 16'hefff;
    LUT4 i26953_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26_adj_28[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26953_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26946_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26_adj_28[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26946_2_lut_3_lut.init = 16'h7878;
    LUT4 i26942_2_lut_rep_142 (.A(L_count[1]), .B(L_count[0]), .Z(n30173)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26942_2_lut_rep_142.init = 16'h8888;
    LUT4 i1_3_lut (.A(n3977), .B(rstn_c), .C(n9), .Z(n4204)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    LUT4 i26939_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26_adj_28[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26939_2_lut.init = 16'h6666;
    LUT4 i26568_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26568_2_lut.init = 16'h6666;
    INV i1822 (.A(H_count[0]), .Z(n1));
    INV i26937 (.A(L_count[0]), .Z(n26_adj_28[0]));
    
endmodule

//
// Verilog Description of module check_IR_defv_U17
//

module check_IR_defv_U17 (ir_402x_i_c_0, \clock_count[15] , \def_voltag[0] , 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input ir_402x_i_c_0;
    input \clock_count[15] ;
    output \def_voltag[0] ;
    input rstn_c;
    
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    wire [4:0]n26;
    
    wire n30144;
    wire [4:0]n26_adj_23;
    
    wire n30142, n6, n9, n4433, n1855, n1, n4198, n3863, n29539;
    
    LUT4 i27086_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i27086_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i27079_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i27079_2_lut_3_lut.init = 16'h7878;
    LUT4 i27075_2_lut_rep_113 (.A(L_count[1]), .B(L_count[0]), .Z(n30144)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27075_2_lut_rep_113.init = 16'h8888;
    LUT4 i26748_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26_adj_23[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26748_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26741_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26_adj_23[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26741_2_lut_3_lut.init = 16'h7878;
    LUT4 i26737_2_lut_rep_111 (.A(H_count[1]), .B(H_count[0]), .Z(n30142)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26737_2_lut_rep_111.init = 16'h8888;
    LUT4 i26734_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26_adj_23[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26734_2_lut.init = 16'h6666;
    LUT4 i1_2_lut (.A(H_count[4]), .B(H_count[2]), .Z(n6)) /* synthesis lut_function=((B)+!A) */ ;
    defparam i1_2_lut.init = 16'hdddd;
    LUT4 i4_4_lut (.A(H_count[1]), .B(H_count[3]), .C(H_count[0]), .D(n6), 
         .Z(n9)) /* synthesis lut_function=(A+(((D)+!C)+!B)) */ ;
    defparam i4_4_lut.init = 16'hffbf;
    LUT4 i28022_2_lut (.A(ir_402x_i_c_0), .B(n9), .Z(n4433)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i28022_2_lut.init = 16'h7777;
    FD1S3IX H_count_1477__i0 (.D(n26_adj_23[0]), .CK(\clock_count[15] ), 
            .CD(n4433), .Q(H_count[0]));
    defparam H_count_1477__i0.GSR = "ENABLED";
    FD1S3IX L_count_1478__i0 (.D(n1), .CK(\clock_count[15] ), .CD(n1855), 
            .Q(L_count[0]));
    defparam L_count_1478__i0.GSR = "ENABLED";
    LUT4 i27072_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i27072_2_lut.init = 16'h6666;
    FD1P3AX defv_23 (.D(n9), .SP(n4198), .CK(\clock_count[15] ), .Q(\def_voltag[0] ));
    defparam defv_23.GSR = "DISABLED";
    FD1S3IX L_count_1478__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n1855), 
            .Q(L_count[1]));
    defparam L_count_1478__i1.GSR = "ENABLED";
    FD1S3IX L_count_1478__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n1855), 
            .Q(L_count[2]));
    defparam L_count_1478__i2.GSR = "ENABLED";
    FD1S3IX L_count_1478__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n1855), 
            .Q(L_count[3]));
    defparam L_count_1478__i3.GSR = "ENABLED";
    FD1S3IX L_count_1478__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n1855), 
            .Q(L_count[4]));
    defparam L_count_1478__i4.GSR = "ENABLED";
    FD1S3IX H_count_1477__i1 (.D(n26_adj_23[1]), .CK(\clock_count[15] ), 
            .CD(n4433), .Q(H_count[1]));
    defparam H_count_1477__i1.GSR = "ENABLED";
    FD1S3IX H_count_1477__i2 (.D(n26_adj_23[2]), .CK(\clock_count[15] ), 
            .CD(n4433), .Q(H_count[2]));
    defparam H_count_1477__i2.GSR = "ENABLED";
    FD1S3IX H_count_1477__i3 (.D(n26_adj_23[3]), .CK(\clock_count[15] ), 
            .CD(n4433), .Q(H_count[3]));
    defparam H_count_1477__i3.GSR = "ENABLED";
    FD1S3IX H_count_1477__i4 (.D(n26_adj_23[4]), .CK(\clock_count[15] ), 
            .CD(n4433), .Q(H_count[4]));
    defparam H_count_1477__i4.GSR = "ENABLED";
    LUT4 i27093_3_lut_4_lut (.A(L_count[2]), .B(n30144), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i27093_3_lut_4_lut.init = 16'h7f80;
    INV i1819 (.A(L_count[0]), .Z(n1));
    LUT4 i26755_3_lut_4_lut (.A(H_count[2]), .B(n30142), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26_adj_23[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26755_3_lut_4_lut.init = 16'h7f80;
    LUT4 i1_3_lut (.A(n3863), .B(rstn_c), .C(n9), .Z(n4198)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    INV i26732 (.A(H_count[0]), .Z(n26_adj_23[0]));
    LUT4 i27774_2_lut (.A(L_count[4]), .B(L_count[3]), .Z(n29539)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27774_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(L_count[1]), .B(L_count[2]), .C(L_count[0]), .D(n29539), 
         .Z(n3863)) /* synthesis lut_function=(A+(B+!(C (D)))) */ ;
    defparam i3_4_lut.init = 16'hefff;
    LUT4 i836_3_lut (.A(ir_402x_i_c_0), .B(n3863), .C(n9), .Z(n1855)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i836_3_lut.init = 16'hbaba;
    
endmodule

//
// Verilog Description of module hc74595
//

module hc74595 (qb0, srclk_c, qa0, qc0, qd0, qe0, \con[5] , rclk_c, 
            \con[6] , \con[7] , VCC_net, GND_net, ser_c) /* synthesis syn_module_defined=1 */ ;
    output qb0;
    input srclk_c;
    output qa0;
    output qc0;
    output qd0;
    output qe0;
    output \con[5] ;
    input rclk_c;
    output \con[6] ;
    output \con[7] ;
    input VCC_net;
    input GND_net;
    input ser_c;
    
    wire qf0;   // g:/zjf/mhub4k862-cpld/hc74595.v(36[5:8])
    wire qg0;   // g:/zjf/mhub4k862-cpld/hc74595.v(37[5:8])
    wire qh0;   // g:/zjf/mhub4k862-cpld/hc74595.v(38[5:8])
    
    FD1S3AX qb0_24 (.D(qa0), .CK(srclk_c), .Q(qb0));
    defparam qb0_24.GSR = "ENABLED";
    FD1S3AX qc0_25 (.D(qb0), .CK(srclk_c), .Q(qc0));
    defparam qc0_25.GSR = "ENABLED";
    FD1S3AX qd0_26 (.D(qc0), .CK(srclk_c), .Q(qd0));
    defparam qd0_26.GSR = "ENABLED";
    FD1S3AX qe0_27 (.D(qd0), .CK(srclk_c), .Q(qe0));
    defparam qe0_27.GSR = "ENABLED";
    FD1S3AX qf0_28 (.D(qe0), .CK(srclk_c), .Q(qf0));
    defparam qf0_28.GSR = "ENABLED";
    FD1S3AX qg0_29 (.D(qf0), .CK(srclk_c), .Q(qg0));
    defparam qg0_29.GSR = "ENABLED";
    FD1S3AX qh0_30 (.D(qg0), .CK(srclk_c), .Q(qh0));
    defparam qh0_30.GSR = "ENABLED";
    FD1S3AX qf_37 (.D(qf0), .CK(rclk_c), .Q(\con[5] ));
    defparam qf_37.GSR = "DISABLED";
    FD1S3AX qg_38 (.D(qg0), .CK(rclk_c), .Q(\con[6] ));
    defparam qg_38.GSR = "DISABLED";
    FD1S3AX qh_39 (.D(qh0), .CK(rclk_c), .Q(\con[7] ));
    defparam qh_39.GSR = "DISABLED";
    IFS1P3DX qa0_23 (.D(ser_c), .SP(VCC_net), .SCLK(srclk_c), .CD(GND_net), 
            .Q(qa0));
    defparam qa0_23.GSR = "ENABLED";
    
endmodule

//
// Verilog Description of module check_IR_defv_U16
//

module check_IR_defv_U16 (\clock_count[15] , \def_voltag[1] , ir_402x_i_c_1, 
            rstn_c) /* synthesis syn_module_defined=1 */ ;
    input \clock_count[15] ;
    output \def_voltag[1] ;
    input ir_402x_i_c_1;
    input rstn_c;
    
    wire [4:0]L_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(30[11:18])
    wire [4:0]H_count;   // g:/zjf/mhub4k862-cpld/check_ir_defv.v(29[11:18])
    wire [4:0]n26;
    
    wire n30141, n4435;
    wire [4:0]n26_adj_18;
    
    wire n1864, n30175, n4201, n9, n29549, n3981, n29583;
    
    LUT4 i26867_2_lut (.A(L_count[1]), .B(L_count[0]), .Z(n26[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26867_2_lut.init = 16'h6666;
    LUT4 i26881_2_lut_3_lut_4_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[3]), 
         .D(L_count[2]), .Z(n26[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26881_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26874_2_lut_3_lut (.A(L_count[1]), .B(L_count[0]), .C(L_count[2]), 
         .Z(n26[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26874_2_lut_3_lut.init = 16'h7878;
    LUT4 i26870_2_lut_rep_110 (.A(L_count[1]), .B(L_count[0]), .Z(n30141)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26870_2_lut_rep_110.init = 16'h8888;
    FD1S3IX H_count_1480__i0 (.D(n26_adj_18[0]), .CK(\clock_count[15] ), 
            .CD(n4435), .Q(H_count[0]));
    defparam H_count_1480__i0.GSR = "ENABLED";
    FD1S3IX L_count_1481__i0 (.D(n26[0]), .CK(\clock_count[15] ), .CD(n1864), 
            .Q(L_count[0]));
    defparam L_count_1481__i0.GSR = "ENABLED";
    LUT4 i26924_3_lut_4_lut (.A(H_count[2]), .B(n30175), .C(H_count[3]), 
         .D(H_count[4]), .Z(n26_adj_18[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26924_3_lut_4_lut.init = 16'h7f80;
    FD1P3AX defv_23 (.D(n9), .SP(n4201), .CK(\clock_count[15] ), .Q(\def_voltag[1] ));
    defparam defv_23.GSR = "DISABLED";
    LUT4 i27784_2_lut (.A(H_count[4]), .B(H_count[3]), .Z(n29549)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27784_2_lut.init = 16'h8888;
    LUT4 i3_4_lut (.A(H_count[2]), .B(H_count[0]), .C(H_count[1]), .D(n29549), 
         .Z(n9)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut.init = 16'hfbff;
    LUT4 i28040_2_lut (.A(ir_402x_i_c_1), .B(n9), .Z(n4435)) /* synthesis lut_function=(!(A (B))) */ ;
    defparam i28040_2_lut.init = 16'h7777;
    FD1S3IX L_count_1481__i1 (.D(n26[1]), .CK(\clock_count[15] ), .CD(n1864), 
            .Q(L_count[1]));
    defparam L_count_1481__i1.GSR = "ENABLED";
    FD1S3IX L_count_1481__i2 (.D(n26[2]), .CK(\clock_count[15] ), .CD(n1864), 
            .Q(L_count[2]));
    defparam L_count_1481__i2.GSR = "ENABLED";
    FD1S3IX L_count_1481__i3 (.D(n26[3]), .CK(\clock_count[15] ), .CD(n1864), 
            .Q(L_count[3]));
    defparam L_count_1481__i3.GSR = "ENABLED";
    FD1S3IX L_count_1481__i4 (.D(n26[4]), .CK(\clock_count[15] ), .CD(n1864), 
            .Q(L_count[4]));
    defparam L_count_1481__i4.GSR = "ENABLED";
    FD1S3IX H_count_1480__i1 (.D(n26_adj_18[1]), .CK(\clock_count[15] ), 
            .CD(n4435), .Q(H_count[1]));
    defparam H_count_1480__i1.GSR = "ENABLED";
    FD1S3IX H_count_1480__i2 (.D(n26_adj_18[2]), .CK(\clock_count[15] ), 
            .CD(n4435), .Q(H_count[2]));
    defparam H_count_1480__i2.GSR = "ENABLED";
    FD1S3IX H_count_1480__i3 (.D(n26_adj_18[3]), .CK(\clock_count[15] ), 
            .CD(n4435), .Q(H_count[3]));
    defparam H_count_1480__i3.GSR = "ENABLED";
    FD1S3IX H_count_1480__i4 (.D(n26_adj_18[4]), .CK(\clock_count[15] ), 
            .CD(n4435), .Q(H_count[4]));
    defparam H_count_1480__i4.GSR = "ENABLED";
    LUT4 i26917_2_lut_3_lut_4_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[3]), 
         .D(H_count[2]), .Z(n26_adj_18[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(C))+!A !(C))) */ ;
    defparam i26917_2_lut_3_lut_4_lut.init = 16'h78f0;
    LUT4 i26910_2_lut_3_lut (.A(H_count[1]), .B(H_count[0]), .C(H_count[2]), 
         .Z(n26_adj_18[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26910_2_lut_3_lut.init = 16'h7878;
    LUT4 i26906_2_lut_rep_144 (.A(H_count[1]), .B(H_count[0]), .Z(n30175)) /* synthesis lut_function=(A (B)) */ ;
    defparam i26906_2_lut_rep_144.init = 16'h8888;
    LUT4 i26903_2_lut (.A(H_count[1]), .B(H_count[0]), .Z(n26_adj_18[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26903_2_lut.init = 16'h6666;
    LUT4 i1_3_lut (.A(n3981), .B(rstn_c), .C(n9), .Z(n4201)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B))) */ ;
    defparam i1_3_lut.init = 16'h4c4c;
    LUT4 i26888_3_lut_4_lut (.A(L_count[2]), .B(n30141), .C(L_count[3]), 
         .D(L_count[4]), .Z(n26[4])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26888_3_lut_4_lut.init = 16'h7f80;
    LUT4 i27818_2_lut (.A(L_count[4]), .B(L_count[3]), .Z(n29583)) /* synthesis lut_function=(A (B)) */ ;
    defparam i27818_2_lut.init = 16'h8888;
    LUT4 i3_4_lut_adj_6 (.A(L_count[2]), .B(L_count[0]), .C(L_count[1]), 
         .D(n29583), .Z(n3981)) /* synthesis lut_function=(A+((C+!(D))+!B)) */ ;
    defparam i3_4_lut_adj_6.init = 16'hfbff;
    LUT4 i845_3_lut (.A(ir_402x_i_c_1), .B(n3981), .C(n9), .Z(n1864)) /* synthesis lut_function=(A+!(B+!(C))) */ ;
    defparam i845_3_lut.init = 16'hbaba;
    INV i26865 (.A(L_count[0]), .Z(n26[0]));
    INV i26901 (.A(H_count[0]), .Z(n26_adj_18[0]));
    
endmodule

//
// Verilog Description of module ir_car_detect_U3
//

module ir_car_detect_U3 (clk_c, \f2n_irx_temp[6] , ir_car_null, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input \f2n_irx_temp[6] ;
    output ir_car_null;
    input GND_net;
    
    wire [11:0]ir_cnt;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(26[13:19])
    wire ir_edge_chg;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(24[6:41])
    wire ir_sd_reg1;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(18[7:17])
    wire ir_sd_reg0;   // g:/zjf/mhub4k862-cpld/ir_car_detect.v(17[7:17])
    wire [11:0]n24;
    wire [11:0]n11;
    
    wire n28770, n28769, n28768, n28767, n28766, n28765;
    
    FD1S3IX ir_cnt_i0 (.D(n24[0]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[0]));
    defparam ir_cnt_i0.GSR = "ENABLED";
    FD1S3AX ir_sd_reg1_20 (.D(ir_sd_reg0), .CK(clk_c), .Q(ir_sd_reg1));
    defparam ir_sd_reg1_20.GSR = "DISABLED";
    FD1S3AX ir_sd_reg0_19 (.D(\f2n_irx_temp[6] ), .CK(clk_c), .Q(ir_sd_reg0));
    defparam ir_sd_reg0_19.GSR = "DISABLED";
    FD1P3AX ir_car_null_22 (.D(ir_cnt[11]), .SP(ir_edge_chg), .CK(clk_c), 
            .Q(ir_car_null));
    defparam ir_car_null_22.GSR = "DISABLED";
    FD1S3IX ir_cnt_i11 (.D(n24[11]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[11]));
    defparam ir_cnt_i11.GSR = "ENABLED";
    FD1S3IX ir_cnt_i10 (.D(n24[10]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[10]));
    defparam ir_cnt_i10.GSR = "ENABLED";
    FD1S3IX ir_cnt_i9 (.D(n24[9]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[9]));
    defparam ir_cnt_i9.GSR = "ENABLED";
    FD1S3IX ir_cnt_i8 (.D(n24[8]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[8]));
    defparam ir_cnt_i8.GSR = "ENABLED";
    FD1S3IX ir_cnt_i7 (.D(n24[7]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[7]));
    defparam ir_cnt_i7.GSR = "ENABLED";
    FD1S3IX ir_cnt_i6 (.D(n24[6]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[6]));
    defparam ir_cnt_i6.GSR = "ENABLED";
    FD1S3IX ir_cnt_i5 (.D(n24[5]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[5]));
    defparam ir_cnt_i5.GSR = "ENABLED";
    FD1S3IX ir_cnt_i4 (.D(n24[4]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[4]));
    defparam ir_cnt_i4.GSR = "ENABLED";
    FD1S3IX ir_cnt_i3 (.D(n24[3]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[3]));
    defparam ir_cnt_i3.GSR = "ENABLED";
    FD1S3IX ir_cnt_i2 (.D(n24[2]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[2]));
    defparam ir_cnt_i2.GSR = "ENABLED";
    FD1S3IX ir_cnt_i1 (.D(n24[1]), .CK(clk_c), .CD(ir_edge_chg), .Q(ir_cnt[1]));
    defparam ir_cnt_i1.GSR = "ENABLED";
    LUT4 i14822_2_lut (.A(n11[0]), .B(ir_cnt[11]), .Z(n24[0])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14822_2_lut.init = 16'heeee;
    LUT4 i6_2_lut (.A(ir_sd_reg0), .B(ir_sd_reg1), .Z(ir_edge_chg)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i6_2_lut.init = 16'h6666;
    CCU2D add_9_13 (.A0(ir_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28770), 
          .S0(n11[11]));
    defparam add_9_13.INIT0 = 16'h5aaa;
    defparam add_9_13.INIT1 = 16'h0000;
    defparam add_9_13.INJECT1_0 = "NO";
    defparam add_9_13.INJECT1_1 = "NO";
    CCU2D add_9_11 (.A0(ir_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28769), .COUT(n28770), .S0(n11[9]), .S1(n11[10]));
    defparam add_9_11.INIT0 = 16'h5aaa;
    defparam add_9_11.INIT1 = 16'h5aaa;
    defparam add_9_11.INJECT1_0 = "NO";
    defparam add_9_11.INJECT1_1 = "NO";
    CCU2D add_9_9 (.A0(ir_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28768), .COUT(n28769), .S0(n11[7]), .S1(n11[8]));
    defparam add_9_9.INIT0 = 16'h5aaa;
    defparam add_9_9.INIT1 = 16'h5aaa;
    defparam add_9_9.INJECT1_0 = "NO";
    defparam add_9_9.INJECT1_1 = "NO";
    CCU2D add_9_7 (.A0(ir_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28767), .COUT(n28768), .S0(n11[5]), .S1(n11[6]));
    defparam add_9_7.INIT0 = 16'h5aaa;
    defparam add_9_7.INIT1 = 16'h5aaa;
    defparam add_9_7.INJECT1_0 = "NO";
    defparam add_9_7.INJECT1_1 = "NO";
    CCU2D add_9_5 (.A0(ir_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28766), .COUT(n28767), .S0(n11[3]), .S1(n11[4]));
    defparam add_9_5.INIT0 = 16'h5aaa;
    defparam add_9_5.INIT1 = 16'h5aaa;
    defparam add_9_5.INJECT1_0 = "NO";
    defparam add_9_5.INJECT1_1 = "NO";
    CCU2D add_9_3 (.A0(ir_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28765), .COUT(n28766), .S0(n11[1]), .S1(n11[2]));
    defparam add_9_3.INIT0 = 16'h5aaa;
    defparam add_9_3.INIT1 = 16'h5aaa;
    defparam add_9_3.INJECT1_0 = "NO";
    defparam add_9_3.INJECT1_1 = "NO";
    CCU2D add_9_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(ir_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28765), .S1(n11[0]));
    defparam add_9_1.INIT0 = 16'hF000;
    defparam add_9_1.INIT1 = 16'h5555;
    defparam add_9_1.INJECT1_0 = "NO";
    defparam add_9_1.INJECT1_1 = "NO";
    LUT4 i14977_2_lut (.A(n11[1]), .B(ir_cnt[11]), .Z(n24[1])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14977_2_lut.init = 16'heeee;
    LUT4 i14976_2_lut (.A(n11[2]), .B(ir_cnt[11]), .Z(n24[2])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14976_2_lut.init = 16'heeee;
    LUT4 i14975_2_lut (.A(n11[3]), .B(ir_cnt[11]), .Z(n24[3])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14975_2_lut.init = 16'heeee;
    LUT4 i14974_2_lut (.A(n11[4]), .B(ir_cnt[11]), .Z(n24[4])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14974_2_lut.init = 16'heeee;
    LUT4 i14973_2_lut (.A(n11[5]), .B(ir_cnt[11]), .Z(n24[5])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14973_2_lut.init = 16'heeee;
    LUT4 i14972_2_lut (.A(n11[6]), .B(ir_cnt[11]), .Z(n24[6])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14972_2_lut.init = 16'heeee;
    LUT4 i14971_2_lut (.A(n11[7]), .B(ir_cnt[11]), .Z(n24[7])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14971_2_lut.init = 16'heeee;
    LUT4 i14970_2_lut (.A(n11[8]), .B(ir_cnt[11]), .Z(n24[8])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14970_2_lut.init = 16'heeee;
    LUT4 i14969_2_lut (.A(n11[9]), .B(ir_cnt[11]), .Z(n24[9])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14969_2_lut.init = 16'heeee;
    LUT4 i14968_2_lut (.A(n11[10]), .B(ir_cnt[11]), .Z(n24[10])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14968_2_lut.init = 16'heeee;
    LUT4 i14967_2_lut (.A(n11[11]), .B(ir_cnt[11]), .Z(n24[11])) /* synthesis lut_function=(A+(B)) */ ;
    defparam i14967_2_lut.init = 16'heeee;
    
endmodule

//
// Verilog Description of module ir_rx
//

module ir_rx (clk_c, n111, GND_net, n11, ir_front_c, n12, \f2n_irx_level[5] , 
            n30180, n30178, n10) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    output [0:0]n111;
    input GND_net;
    input n11;
    input ir_front_c;
    input n12;
    input \f2n_irx_level[5] ;
    input n30180;
    input n30178;
    input n10;
    
    wire [11:0]hig_cnt;   // g:/zjf/mhub4k862-cpld/ir_rx.v(32[13:20])
    wire [11:0]n9;
    
    wire n30122, n30128;
    wire [11:0]n35;
    
    wire n3587, n14_adj_7, n15_adj_8, n29383, n28782, n28781, n28780, 
        n28779, n28778, n28777, n16_adj_10;
    
    LUT4 i14944_3_lut (.A(n9[1]), .B(n30122), .C(n30128), .Z(n35[1])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14944_3_lut.init = 16'hc8c8;
    LUT4 i14942_3_lut (.A(n9[3]), .B(n30122), .C(n30128), .Z(n35[3])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14942_3_lut.init = 16'hc8c8;
    LUT4 i14941_3_lut (.A(n9[4]), .B(n30122), .C(n30128), .Z(n35[4])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14941_3_lut.init = 16'hc8c8;
    LUT4 i14940_3_lut (.A(n9[5]), .B(n30122), .C(n30128), .Z(n35[5])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14940_3_lut.init = 16'hc8c8;
    LUT4 i14939_3_lut (.A(n9[6]), .B(n30122), .C(n30128), .Z(n35[6])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14939_3_lut.init = 16'hc8c8;
    LUT4 i14938_3_lut (.A(n9[7]), .B(n30122), .C(n30128), .Z(n35[7])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14938_3_lut.init = 16'hc8c8;
    LUT4 i14937_3_lut (.A(n9[8]), .B(n30122), .C(n30128), .Z(n35[8])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14937_3_lut.init = 16'hc8c8;
    LUT4 i14936_3_lut (.A(n9[9]), .B(n30122), .C(n30128), .Z(n35[9])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14936_3_lut.init = 16'hc8c8;
    LUT4 i14935_3_lut (.A(n9[10]), .B(n30122), .C(n30128), .Z(n35[10])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14935_3_lut.init = 16'hc8c8;
    LUT4 i14934_3_lut (.A(n9[11]), .B(n30122), .C(n30128), .Z(n35[11])) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i14934_3_lut.init = 16'hc8c8;
    FD1S3IX hig_cnt_i0 (.D(n9[0]), .CK(clk_c), .CD(n3587), .Q(hig_cnt[0]));
    defparam hig_cnt_i0.GSR = "ENABLED";
    FD1S3AY hig_cnt_i11 (.D(n35[11]), .CK(clk_c), .Q(hig_cnt[11]));
    defparam hig_cnt_i11.GSR = "ENABLED";
    FD1S3AY hig_cnt_i10 (.D(n35[10]), .CK(clk_c), .Q(hig_cnt[10]));
    defparam hig_cnt_i10.GSR = "ENABLED";
    FD1S3AY hig_cnt_i9 (.D(n35[9]), .CK(clk_c), .Q(hig_cnt[9]));
    defparam hig_cnt_i9.GSR = "ENABLED";
    FD1S3AY hig_cnt_i8 (.D(n35[8]), .CK(clk_c), .Q(hig_cnt[8]));
    defparam hig_cnt_i8.GSR = "ENABLED";
    FD1S3AY hig_cnt_i7 (.D(n35[7]), .CK(clk_c), .Q(hig_cnt[7]));
    defparam hig_cnt_i7.GSR = "ENABLED";
    FD1S3AY hig_cnt_i6 (.D(n35[6]), .CK(clk_c), .Q(hig_cnt[6]));
    defparam hig_cnt_i6.GSR = "ENABLED";
    FD1S3AY hig_cnt_i5 (.D(n35[5]), .CK(clk_c), .Q(hig_cnt[5]));
    defparam hig_cnt_i5.GSR = "ENABLED";
    FD1S3AY hig_cnt_i4 (.D(n35[4]), .CK(clk_c), .Q(hig_cnt[4]));
    defparam hig_cnt_i4.GSR = "ENABLED";
    FD1S3AY hig_cnt_i3 (.D(n35[3]), .CK(clk_c), .Q(hig_cnt[3]));
    defparam hig_cnt_i3.GSR = "ENABLED";
    FD1S3IX hig_cnt_i2 (.D(n9[2]), .CK(clk_c), .CD(n3587), .Q(hig_cnt[2]));
    defparam hig_cnt_i2.GSR = "ENABLED";
    FD1S3AY hig_cnt_i1 (.D(n35[1]), .CK(clk_c), .Q(hig_cnt[1]));
    defparam hig_cnt_i1.GSR = "ENABLED";
    LUT4 i5_3_lut (.A(hig_cnt[8]), .B(hig_cnt[7]), .C(hig_cnt[11]), .Z(n14_adj_7)) /* synthesis lut_function=(A (B (C))) */ ;
    defparam i5_3_lut.init = 16'h8080;
    LUT4 i6_4_lut (.A(hig_cnt[6]), .B(hig_cnt[9]), .C(hig_cnt[10]), .D(hig_cnt[4]), 
         .Z(n15_adj_8)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i6_4_lut.init = 16'h8000;
    LUT4 i8_4_lut (.A(n15_adj_8), .B(hig_cnt[5]), .C(n14_adj_7), .D(hig_cnt[3]), 
         .Z(n29383)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i8_4_lut.init = 16'h8000;
    LUT4 i1_4_lut (.A(hig_cnt[2]), .B(hig_cnt[1]), .C(hig_cnt[0]), .D(n29383), 
         .Z(n111[0])) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam i1_4_lut.init = 16'h0400;
    LUT4 i1_3_lut_rep_97 (.A(hig_cnt[2]), .B(n29383), .C(hig_cnt[1]), 
         .Z(n30128)) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i1_3_lut_rep_97.init = 16'hc8c8;
    LUT4 i27988_2_lut_2_lut_4_lut (.A(hig_cnt[2]), .B(n29383), .C(hig_cnt[1]), 
         .D(n30122), .Z(n3587)) /* synthesis lut_function=(A (B+!(D))+!A (B (C+!(D))+!B !(D))) */ ;
    defparam i27988_2_lut_2_lut_4_lut.init = 16'hc8ff;
    CCU2D add_7_13 (.A0(hig_cnt[11]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n28782), 
          .S0(n9[11]));
    defparam add_7_13.INIT0 = 16'h5aaa;
    defparam add_7_13.INIT1 = 16'h0000;
    defparam add_7_13.INJECT1_0 = "NO";
    defparam add_7_13.INJECT1_1 = "NO";
    CCU2D add_7_11 (.A0(hig_cnt[9]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(hig_cnt[10]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28781), .COUT(n28782), .S0(n9[9]), .S1(n9[10]));
    defparam add_7_11.INIT0 = 16'h5aaa;
    defparam add_7_11.INIT1 = 16'h5aaa;
    defparam add_7_11.INJECT1_0 = "NO";
    defparam add_7_11.INJECT1_1 = "NO";
    CCU2D add_7_9 (.A0(hig_cnt[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(hig_cnt[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28780), .COUT(n28781), .S0(n9[7]), .S1(n9[8]));
    defparam add_7_9.INIT0 = 16'h5aaa;
    defparam add_7_9.INIT1 = 16'h5aaa;
    defparam add_7_9.INJECT1_0 = "NO";
    defparam add_7_9.INJECT1_1 = "NO";
    CCU2D add_7_7 (.A0(hig_cnt[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(hig_cnt[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28779), .COUT(n28780), .S0(n9[5]), .S1(n9[6]));
    defparam add_7_7.INIT0 = 16'h5aaa;
    defparam add_7_7.INIT1 = 16'h5aaa;
    defparam add_7_7.INJECT1_0 = "NO";
    defparam add_7_7.INJECT1_1 = "NO";
    CCU2D add_7_5 (.A0(hig_cnt[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(hig_cnt[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28778), .COUT(n28779), .S0(n9[3]), .S1(n9[4]));
    defparam add_7_5.INIT0 = 16'h5aaa;
    defparam add_7_5.INIT1 = 16'h5aaa;
    defparam add_7_5.INJECT1_0 = "NO";
    defparam add_7_5.INJECT1_1 = "NO";
    CCU2D add_7_3 (.A0(hig_cnt[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(hig_cnt[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28777), .COUT(n28778), .S0(n9[1]), .S1(n9[2]));
    defparam add_7_3.INIT0 = 16'h5aaa;
    defparam add_7_3.INIT1 = 16'h5aaa;
    defparam add_7_3.INJECT1_0 = "NO";
    defparam add_7_3.INJECT1_1 = "NO";
    CCU2D add_7_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(hig_cnt[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28777), .S1(n9[0]));
    defparam add_7_1.INIT0 = 16'hF000;
    defparam add_7_1.INIT1 = 16'h5555;
    defparam add_7_1.INJECT1_0 = "NO";
    defparam add_7_1.INJECT1_1 = "NO";
    LUT4 i1_4_lut_rep_91 (.A(n11), .B(ir_front_c), .C(n16_adj_10), .D(n12), 
         .Z(n30122)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i1_4_lut_rep_91.init = 16'h8000;
    LUT4 i7_4_lut (.A(\f2n_irx_level[5] ), .B(n30180), .C(n30178), .D(n10), 
         .Z(n16_adj_10)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i7_4_lut.init = 16'h8000;
    
endmodule

//
// Verilog Description of module PUR
// module not written out since it is a black-box. 
//


//
// Verilog Description of module mcu2pc_uart_top
//

module mcu2pc_uart_top (clk_c, n30301, to_db9_baud, rx_baud_count, rs232_db9_rx_c, 
            n30300, GND_net, rxbaud_fifo_tx_db9, \rx_step[3] , n30134, 
            n30121, n30125, n30155, n29096, n30166, n134, VCC_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input n30301;
    input [2:0]to_db9_baud;
    output [13:0]rx_baud_count;
    output rs232_db9_rx_c;
    input n30300;
    input GND_net;
    input [2:0]rxbaud_fifo_tx_db9;
    output \rx_step[3] ;
    output n30134;
    input n30121;
    input n30125;
    output n30155;
    input n29096;
    output n30166;
    input n134;
    input VCC_net;
    
    wire rx_int0;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(72[6:13])
    wire txen;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(26[12:16])
    wire rx_int1;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(73[6:13])
    wire tx_int;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(53[12:18])
    wire rden;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(32[12:16])
    wire rdy;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(29[12:15])
    wire empty;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(30[12:17])
    wire [7:0]tx_dat;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(27[12:18])
    wire rx_ok;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(15[6:11])
    wire [7:0]rx_dat;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(14[12:18])
    
    wire n31;
    
    FD1S3AX rx_int0_21 (.D(txen), .CK(clk_c), .Q(rx_int0));
    defparam rx_int0_21.GSR = "ENABLED";
    FD1S3AX rx_int1_22 (.D(rx_int0), .CK(clk_c), .Q(rx_int1));
    defparam rx_int1_22.GSR = "ENABLED";
    FD1S3AX tx_int_23 (.D(rden), .CK(clk_c), .Q(tx_int));
    defparam tx_int_23.GSR = "DISABLED";
    FD1P3IX txen_20 (.D(n30301), .SP(n31), .CD(rdy), .CK(clk_c), .Q(txen));
    defparam txen_20.GSR = "ENABLED";
    LUT4 i27994_2_lut (.A(rdy), .B(empty), .Z(n31)) /* synthesis lut_function=(!(A+(B))) */ ;
    defparam i27994_2_lut.init = 16'h1111;
    LUT4 i18_2_lut (.A(rx_int0), .B(rx_int1), .Z(rden)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i18_2_lut.init = 16'h2222;
    uart_tx_U0 mcu2pc_tx (.clk_c(clk_c), .tx_int(tx_int), .tx_dat({tx_dat}), 
            .to_db9_baud({to_db9_baud}), .rx_baud_count({rx_baud_count}), 
            .rs232_db9_rx_c(rs232_db9_rx_c), .rdy(rdy), .n30300(n30300), 
            .GND_net(GND_net)) /* synthesis syn_module_defined=1 */ ;
    uart_rx_U1 mcu2pc_rx (.clk_c(clk_c), .rxbaud_fifo_tx_db9({rxbaud_fifo_tx_db9}), 
            .\rx_step[3] (\rx_step[3] ), .n30134(n30134), .n30121(n30121), 
            .rx_ok(rx_ok), .GND_net(GND_net), .rx_dat({rx_dat}), .n30125(n30125), 
            .n30155(n30155), .n29096(n29096), .n30166(n30166)) /* synthesis syn_module_defined=1 */ ;
    fifo64x8_U2 mcu2pc_fifo (.rx_dat({rx_dat}), .clk_c(clk_c), .rx_ok(rx_ok), 
            .rden(rden), .n134(n134), .tx_dat({tx_dat}), .empty(empty), 
            .GND_net(GND_net), .VCC_net(VCC_net)) /* synthesis syn_module_defined=1 */ ;
    
endmodule

//
// Verilog Description of module uart_tx_U0
//

module uart_tx_U0 (clk_c, tx_int, tx_dat, to_db9_baud, rx_baud_count, 
            rs232_db9_rx_c, rdy, n30300, GND_net) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input tx_int;
    input [7:0]tx_dat;
    input [2:0]to_db9_baud;
    output [13:0]rx_baud_count;
    output rs232_db9_rx_c;
    output rdy;
    input n30300;
    input GND_net;
    
    wire [7:0]tx_data;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(157[10:17])
    wire [13:0]rx_clk_count;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(144[12:24])
    wire [3:0]tx_step;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(169[11:18])
    wire tx_samp_clk;   // g:/zjf/mhub4k862-cpld/new_uart/uart_tx.v(150[11:22])
    
    wire n1, n34;
    wire [13:0]n62;
    
    wire n30126, n4419;
    wire [3:0]n22;
    
    wire n5, n2, n30188, n30187, n30186, n1811, n1799, n2_adj_3, 
        n30184, n1753, n1755, n30163, n1745, n3519, n111, n29653, 
        n29654, n29655, n7, n28831, n28830, n28829, n28828, n28827, 
        n28826, n28825, n28993, n2_adj_4, n28796, n1_adj_5, n28795, 
        n28794, n28793, n28792, n28791, n5_adj_6, n4, n28790;
    
    FD1P3AX tx_data_i0_i0 (.D(tx_dat[0]), .SP(tx_int), .CK(clk_c), .Q(tx_data[0]));
    defparam tx_data_i0_i0.GSR = "DISABLED";
    INV i1 (.A(to_db9_baud[0]), .Z(n1));
    FD1S3IX rx_clk_count_1504__i0 (.D(n62[0]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[0]));
    defparam rx_clk_count_1504__i0.GSR = "DISABLED";
    FD1P3IX tx_step_1505__i2 (.D(n22[2]), .SP(n30126), .CD(n4419), .CK(clk_c), 
            .Q(tx_step[2]));
    defparam tx_step_1505__i2.GSR = "DISABLED";
    FD1S3AX tx_samp_clk_30 (.D(n34), .CK(clk_c), .Q(tx_samp_clk));
    defparam tx_samp_clk_30.GSR = "DISABLED";
    FD1S3JX rx_baud_count_i0 (.D(n1), .CK(clk_c), .PD(n5), .Q(rx_baud_count[0]));
    defparam rx_baud_count_i0.GSR = "DISABLED";
    FD1P3IX tx_step_1505__i3 (.D(n22[3]), .SP(n30126), .CD(n4419), .CK(clk_c), 
            .Q(tx_step[3]));
    defparam tx_step_1505__i3.GSR = "DISABLED";
    FD1P3IX tx_step_1505__i0 (.D(n22[0]), .SP(n30126), .CD(n4419), .CK(clk_c), 
            .Q(tx_step[0]));
    defparam tx_step_1505__i0.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i13 (.D(n2), .CK(clk_c), .CD(to_db9_baud[2]), 
            .Q(rx_baud_count[13]));
    defparam rx_baud_count_i13.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i12 (.D(n30188), .CK(clk_c), .CD(to_db9_baud[2]), 
            .Q(rx_baud_count[12]));
    defparam rx_baud_count_i12.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i11 (.D(n30187), .CK(clk_c), .CD(to_db9_baud[2]), 
            .Q(rx_baud_count[11]));
    defparam rx_baud_count_i11.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i10 (.D(n30186), .CK(clk_c), .CD(to_db9_baud[0]), 
            .Q(rx_baud_count[10]));
    defparam rx_baud_count_i10.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i9 (.D(n1811), .CK(clk_c), .Q(rx_baud_count[9]));
    defparam rx_baud_count_i9.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i8 (.D(n1), .CK(clk_c), .CD(n5), .Q(rx_baud_count[8]));
    defparam rx_baud_count_i8.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i7 (.D(n1799), .CK(clk_c), .Q(rx_baud_count[7]));
    defparam rx_baud_count_i7.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i6 (.D(n30184), .CK(clk_c), .CD(n2_adj_3), .Q(rx_baud_count[6]));
    defparam rx_baud_count_i6.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i5 (.D(n1753), .CK(clk_c), .Q(rx_baud_count[5]));
    defparam rx_baud_count_i5.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i4 (.D(n1755), .CK(clk_c), .Q(rx_baud_count[4]));
    defparam rx_baud_count_i4.GSR = "DISABLED";
    FD1S3JX rx_baud_count_i3 (.D(n30163), .CK(clk_c), .PD(to_db9_baud[0]), 
            .Q(rx_baud_count[3]));
    defparam rx_baud_count_i3.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i2 (.D(n1745), .CK(clk_c), .Q(rx_baud_count[2]));
    defparam rx_baud_count_i2.GSR = "DISABLED";
    FD1S3JX rx_baud_count_i1 (.D(n2), .CK(clk_c), .PD(n5), .Q(rx_baud_count[1]));
    defparam rx_baud_count_i1.GSR = "DISABLED";
    FD1P3AX tx_buf_34 (.D(n111), .SP(n3519), .CK(clk_c), .Q(rs232_db9_rx_c));
    defparam tx_buf_34.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i13 (.D(n62[13]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[13]));
    defparam rx_clk_count_1504__i13.GSR = "DISABLED";
    L6MUX21 i27882 (.D0(n29653), .D1(n29654), .SD(tx_step[2]), .Z(n29655));
    FD1S3IX rx_clk_count_1504__i12 (.D(n62[12]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[12]));
    defparam rx_clk_count_1504__i12.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i11 (.D(n62[11]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[11]));
    defparam rx_clk_count_1504__i11.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i10 (.D(n62[10]), .CK(clk_c), .CD(n34), 
            .Q(rx_clk_count[10]));
    defparam rx_clk_count_1504__i10.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i9 (.D(n62[9]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[9]));
    defparam rx_clk_count_1504__i9.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i8 (.D(n62[8]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[8]));
    defparam rx_clk_count_1504__i8.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i7 (.D(n62[7]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[7]));
    defparam rx_clk_count_1504__i7.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i6 (.D(n62[6]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[6]));
    defparam rx_clk_count_1504__i6.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i5 (.D(n62[5]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[5]));
    defparam rx_clk_count_1504__i5.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i4 (.D(n62[4]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[4]));
    defparam rx_clk_count_1504__i4.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i3 (.D(n62[3]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[3]));
    defparam rx_clk_count_1504__i3.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i2 (.D(n62[2]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[2]));
    defparam rx_clk_count_1504__i2.GSR = "DISABLED";
    FD1S3IX rx_clk_count_1504__i1 (.D(n62[1]), .CK(clk_c), .CD(n34), .Q(rx_clk_count[1]));
    defparam rx_clk_count_1504__i1.GSR = "DISABLED";
    FD1P3IX tx_step_1505__i1 (.D(n22[1]), .SP(n30126), .CD(n4419), .CK(clk_c), 
            .Q(tx_step[1]));
    defparam tx_step_1505__i1.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i7 (.D(tx_dat[7]), .SP(tx_int), .CK(clk_c), .Q(tx_data[7]));
    defparam tx_data_i0_i7.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i6 (.D(tx_dat[6]), .SP(tx_int), .CK(clk_c), .Q(tx_data[6]));
    defparam tx_data_i0_i6.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i5 (.D(tx_dat[5]), .SP(tx_int), .CK(clk_c), .Q(tx_data[5]));
    defparam tx_data_i0_i5.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i4 (.D(tx_dat[4]), .SP(tx_int), .CK(clk_c), .Q(tx_data[4]));
    defparam tx_data_i0_i4.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i3 (.D(tx_dat[3]), .SP(tx_int), .CK(clk_c), .Q(tx_data[3]));
    defparam tx_data_i0_i3.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i2 (.D(tx_dat[2]), .SP(tx_int), .CK(clk_c), .Q(tx_data[2]));
    defparam tx_data_i0_i2.GSR = "DISABLED";
    FD1P3AX tx_data_i0_i1 (.D(tx_dat[1]), .SP(tx_int), .CK(clk_c), .Q(tx_data[1]));
    defparam tx_data_i0_i1.GSR = "DISABLED";
    FD1P3JX tx_en_32 (.D(n30300), .SP(n7), .PD(tx_int), .CK(clk_c), 
            .Q(rdy));
    defparam tx_en_32.GSR = "DISABLED";
    LUT4 i27975_2_lut_3_lut (.A(n7), .B(rdy), .C(tx_samp_clk), .Z(n4419)) /* synthesis lut_function=(!(((C)+!B)+!A)) */ ;
    defparam i27975_2_lut_3_lut.init = 16'h0808;
    LUT4 i28029_3_lut_rep_95 (.A(n7), .B(rdy), .C(tx_samp_clk), .Z(n30126)) /* synthesis lut_function=(A (B)+!A (B (C))) */ ;
    defparam i28029_3_lut_rep_95.init = 16'hc8c8;
    LUT4 i20_3_lut_4_lut_3_lut (.A(to_db9_baud[0]), .B(to_db9_baud[1]), 
         .C(to_db9_baud[2]), .Z(n1753)) /* synthesis lut_function=(!(A (C)+!A !(B (C)+!B !(C)))) */ ;
    defparam i20_3_lut_4_lut_3_lut.init = 16'h4b4b;
    LUT4 i1_2_lut_rep_157 (.A(to_db9_baud[0]), .B(to_db9_baud[1]), .Z(n30188)) /* synthesis lut_function=(!(A+!(B))) */ ;
    defparam i1_2_lut_rep_157.init = 16'h4444;
    LUT4 i1_2_lut_rep_156 (.A(to_db9_baud[0]), .B(to_db9_baud[1]), .Z(n30187)) /* synthesis lut_function=(A+!(B)) */ ;
    defparam i1_2_lut_rep_156.init = 16'hbbbb;
    LUT4 i24_3_lut_4_lut_3_lut (.A(to_db9_baud[2]), .B(to_db9_baud[1]), 
         .C(to_db9_baud[0]), .Z(n1745)) /* synthesis lut_function=(!(A (B+(C))+!A (B (C)+!B !(C)))) */ ;
    defparam i24_3_lut_4_lut_3_lut.init = 16'h1616;
    LUT4 i29_2_lut_rep_155 (.A(to_db9_baud[2]), .B(to_db9_baud[1]), .Z(n30186)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i29_2_lut_rep_155.init = 16'h6666;
    LUT4 i23_4_lut_3_lut (.A(to_db9_baud[2]), .B(to_db9_baud[0]), .C(to_db9_baud[1]), 
         .Z(n1811)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B (C)+!B !(C)))) */ ;
    defparam i23_4_lut_3_lut.init = 16'h4949;
    LUT4 i1926_2_lut_rep_153 (.A(to_db9_baud[2]), .B(to_db9_baud[0]), .Z(n30184)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1926_2_lut_rep_153.init = 16'h9999;
    CCU2D sub_1290_add_2_15 (.A0(rx_clk_count[13]), .B0(rx_baud_count[13]), 
          .C0(GND_net), .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28831), .S1(n34));
    defparam sub_1290_add_2_15.INIT0 = 16'h5999;
    defparam sub_1290_add_2_15.INIT1 = 16'h0000;
    defparam sub_1290_add_2_15.INJECT1_0 = "NO";
    defparam sub_1290_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_13 (.A0(rx_clk_count[11]), .B0(rx_baud_count[11]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[12]), .B1(rx_baud_count[12]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28830), .COUT(n28831));
    defparam sub_1290_add_2_13.INIT0 = 16'h5999;
    defparam sub_1290_add_2_13.INIT1 = 16'h5999;
    defparam sub_1290_add_2_13.INJECT1_0 = "NO";
    defparam sub_1290_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_11 (.A0(rx_clk_count[9]), .B0(rx_baud_count[9]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[10]), .B1(rx_baud_count[10]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28829), .COUT(n28830));
    defparam sub_1290_add_2_11.INIT0 = 16'h5999;
    defparam sub_1290_add_2_11.INIT1 = 16'h5999;
    defparam sub_1290_add_2_11.INJECT1_0 = "NO";
    defparam sub_1290_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_9 (.A0(rx_clk_count[7]), .B0(rx_baud_count[7]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[8]), .B1(rx_baud_count[8]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28828), .COUT(n28829));
    defparam sub_1290_add_2_9.INIT0 = 16'h5999;
    defparam sub_1290_add_2_9.INIT1 = 16'h5999;
    defparam sub_1290_add_2_9.INJECT1_0 = "NO";
    defparam sub_1290_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_7 (.A0(rx_clk_count[5]), .B0(rx_baud_count[5]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[6]), .B1(rx_baud_count[6]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28827), .COUT(n28828));
    defparam sub_1290_add_2_7.INIT0 = 16'h5999;
    defparam sub_1290_add_2_7.INIT1 = 16'h5999;
    defparam sub_1290_add_2_7.INJECT1_0 = "NO";
    defparam sub_1290_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_5 (.A0(rx_clk_count[3]), .B0(rx_baud_count[3]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[4]), .B1(rx_baud_count[4]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28826), .COUT(n28827));
    defparam sub_1290_add_2_5.INIT0 = 16'h5999;
    defparam sub_1290_add_2_5.INIT1 = 16'h5999;
    defparam sub_1290_add_2_5.INJECT1_0 = "NO";
    defparam sub_1290_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_3 (.A0(rx_clk_count[1]), .B0(rx_baud_count[1]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[2]), .B1(rx_baud_count[2]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28825), .COUT(n28826));
    defparam sub_1290_add_2_3.INIT0 = 16'h5999;
    defparam sub_1290_add_2_3.INIT1 = 16'h5999;
    defparam sub_1290_add_2_3.INJECT1_0 = "NO";
    defparam sub_1290_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_1290_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[0]), .B1(rx_baud_count[0]), .C1(GND_net), 
          .D1(GND_net), .COUT(n28825));
    defparam sub_1290_add_2_1.INIT0 = 16'h0000;
    defparam sub_1290_add_2_1.INIT1 = 16'h5999;
    defparam sub_1290_add_2_1.INJECT1_0 = "NO";
    defparam sub_1290_add_2_1.INJECT1_1 = "NO";
    LUT4 i26640_2_lut (.A(tx_step[1]), .B(tx_step[0]), .Z(n22[1])) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i26640_2_lut.init = 16'h6666;
    LUT4 i3_4_lut (.A(tx_data[7]), .B(tx_step[0]), .C(tx_step[2]), .D(tx_step[1]), 
         .Z(n28993)) /* synthesis lut_function=(A+(B+(C+(D)))) */ ;
    defparam i3_4_lut.init = 16'hfffe;
    LUT4 Mux_18_i15_3_lut (.A(n29655), .B(n28993), .C(tx_step[3]), .Z(n111)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i15_3_lut.init = 16'hcaca;
    LUT4 i1991_2_lut (.A(tx_samp_clk), .B(rdy), .Z(n3519)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1991_2_lut.init = 16'h8888;
    LUT4 Mux_18_i2_3_lut (.A(tx_data[1]), .B(tx_data[2]), .C(tx_step[0]), 
         .Z(n2_adj_4)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i2_3_lut.init = 16'hcaca;
    CCU2D rx_clk_count_1504_add_4_15 (.A0(rx_clk_count[13]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28796), .S0(n62[13]));
    defparam rx_clk_count_1504_add_4_15.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_15.INIT1 = 16'h0000;
    defparam rx_clk_count_1504_add_4_15.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_15.INJECT1_1 = "NO";
    LUT4 i14844_2_lut (.A(tx_data[0]), .B(tx_step[0]), .Z(n1_adj_5)) /* synthesis lut_function=(A (B)) */ ;
    defparam i14844_2_lut.init = 16'h8888;
    CCU2D rx_clk_count_1504_add_4_13 (.A0(rx_clk_count[11]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[12]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28795), .COUT(n28796), .S0(n62[11]), 
          .S1(n62[12]));
    defparam rx_clk_count_1504_add_4_13.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_13.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_13.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_13.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1504_add_4_11 (.A0(rx_clk_count[9]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[10]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28794), .COUT(n28795), .S0(n62[9]), 
          .S1(n62[10]));
    defparam rx_clk_count_1504_add_4_11.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_11.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_11.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_11.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1504_add_4_9 (.A0(rx_clk_count[7]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[8]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28793), .COUT(n28794), .S0(n62[7]), 
          .S1(n62[8]));
    defparam rx_clk_count_1504_add_4_9.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_9.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_9.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_9.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1504_add_4_7 (.A0(rx_clk_count[5]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[6]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28792), .COUT(n28793), .S0(n62[5]), 
          .S1(n62[6]));
    defparam rx_clk_count_1504_add_4_7.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_7.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_7.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_7.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1504_add_4_5 (.A0(rx_clk_count[3]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[4]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28791), .COUT(n28792), .S0(n62[3]), 
          .S1(n62[4]));
    defparam rx_clk_count_1504_add_4_5.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_5.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_5.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_5.INJECT1_1 = "NO";
    LUT4 Mux_18_i5_3_lut (.A(tx_data[5]), .B(tx_data[6]), .C(tx_step[0]), 
         .Z(n5_adj_6)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i5_3_lut.init = 16'hcaca;
    LUT4 Mux_18_i4_3_lut (.A(tx_data[3]), .B(tx_data[4]), .C(tx_step[0]), 
         .Z(n4)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam Mux_18_i4_3_lut.init = 16'hcaca;
    CCU2D rx_clk_count_1504_add_4_3 (.A0(rx_clk_count[1]), .B0(GND_net), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[2]), .B1(GND_net), 
          .C1(GND_net), .D1(GND_net), .CIN(n28790), .COUT(n28791), .S0(n62[1]), 
          .S1(n62[2]));
    defparam rx_clk_count_1504_add_4_3.INIT0 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_3.INIT1 = 16'hfaaa;
    defparam rx_clk_count_1504_add_4_3.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_3.INJECT1_1 = "NO";
    CCU2D rx_clk_count_1504_add_4_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[0]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .COUT(n28790), .S1(n62[0]));
    defparam rx_clk_count_1504_add_4_1.INIT0 = 16'hF000;
    defparam rx_clk_count_1504_add_4_1.INIT1 = 16'h0555;
    defparam rx_clk_count_1504_add_4_1.INJECT1_0 = "NO";
    defparam rx_clk_count_1504_add_4_1.INJECT1_1 = "NO";
    INV i1_adj_5 (.A(to_db9_baud[1]), .Z(n2_adj_3));
    LUT4 i11_3_lut (.A(to_db9_baud[0]), .B(to_db9_baud[2]), .C(to_db9_baud[1]), 
         .Z(n1799)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i11_3_lut.init = 16'hcaca;
    INV i2 (.A(to_db9_baud[2]), .Z(n5));
    LUT4 i1_2_lut (.A(to_db9_baud[0]), .B(to_db9_baud[1]), .Z(n2)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i1_2_lut.init = 16'h2222;
    INV i26638 (.A(tx_step[0]), .Z(n22[0]));
    LUT4 i1_4_lut_3_lut (.A(to_db9_baud[1]), .B(to_db9_baud[2]), .C(to_db9_baud[0]), 
         .Z(n1755)) /* synthesis lut_function=(A (B+!(C))+!A !((C)+!B)) */ ;
    defparam i1_4_lut_3_lut.init = 16'h8e8e;
    LUT4 i27959_2_lut_rep_132 (.A(to_db9_baud[1]), .B(to_db9_baud[2]), .Z(n30163)) /* synthesis lut_function=(!(A+(B))) */ ;
    defparam i27959_2_lut_rep_132.init = 16'h1111;
    LUT4 i26654_3_lut_4_lut (.A(tx_step[0]), .B(tx_step[1]), .C(tx_step[2]), 
         .D(tx_step[3]), .Z(n22[3])) /* synthesis lut_function=(!(A (B (C (D)+!C !(D))+!B !(D))+!A !(D))) */ ;
    defparam i26654_3_lut_4_lut.init = 16'h7f80;
    LUT4 i28006_3_lut_4_lut (.A(tx_step[0]), .B(tx_step[1]), .C(tx_step[3]), 
         .D(tx_step[2]), .Z(n7)) /* synthesis lut_function=(!((((D)+!C)+!B)+!A)) */ ;
    defparam i28006_3_lut_4_lut.init = 16'h0080;
    LUT4 i26647_2_lut_3_lut (.A(tx_step[0]), .B(tx_step[1]), .C(tx_step[2]), 
         .Z(n22[2])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i26647_2_lut_3_lut.init = 16'h7878;
    PFUMX i27881 (.BLUT(n4), .ALUT(n5_adj_6), .C0(tx_step[1]), .Z(n29654));
    PFUMX i27880 (.BLUT(n1_adj_5), .ALUT(n2_adj_4), .C0(tx_step[1]), .Z(n29653));
    
endmodule

//
// Verilog Description of module uart_rx_U1
//

module uart_rx_U1 (clk_c, rxbaud_fifo_tx_db9, \rx_step[3] , n30134, 
            n30121, rx_ok, GND_net, rx_dat, n30125, n30155, n29096, 
            n30166) /* synthesis syn_module_defined=1 */ ;
    input clk_c;
    input [2:0]rxbaud_fifo_tx_db9;
    output \rx_step[3] ;
    output n30134;
    input n30121;
    output rx_ok;
    input GND_net;
    output [7:0]rx_dat;
    input n30125;
    output n30155;
    input n29096;
    output n30166;
    
    wire [2:0]baud_back;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(48[11:20])
    wire [3:0]rx_step;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(29[11:18])
    wire rx_samp_clk;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(31[5:16])
    wire [13:0]rx_clk_count;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(30[12:24])
    wire [13:0]rx_baud_count;   // g:/zjf/mhub4k862-cpld/new_uart/uart_rx.v(12[12:25])
    
    wire n5, n28944;
    wire [3:0]n1657;
    
    wire n30135, n30148;
    wire [13:0]n85;
    
    wire n81, n30273, n2, n1;
    wire [13:0]n36;
    
    wire n2443, n4, n30160, n30159, n30158, n1761, n1763, n1_adj_1, 
        n30157, n1767, n1769, n30156, n1773, n61, n30204, n29054;
    wire [3:0]n201;
    
    wire n3667, n27851, n29527, n29478, n29535, n29469, n28916, 
        n29468, n29477, n16765, n16994, n30198, n27850;
    wire [13:0]n66;
    
    wire n27849, n30270;
    wire [3:0]n1632;
    wire [3:0]n1652;
    
    wire n28752, n27848, n4_adj_2, n28838, n28837, n28836, n28835, 
        n28834, n28833, n28832, n30272;
    wire [3:0]n174;
    
    wire n28789, n28788, n28787, n28786, n28785, n28784, n28783, 
        n28758, n28757, n28756, n28755, n28753, n30271, n28754, 
        n30202, n30203, n30199, n30200, n30196, n30197;
    
    FD1P3AX baud_back_i0_i0 (.D(rxbaud_fifo_tx_db9[0]), .SP(n5), .CK(clk_c), 
            .Q(baud_back[0]));
    defparam baud_back_i0_i0.GSR = "DISABLED";
    LUT4 i1_4_lut_4_lut (.A(rx_step[1]), .B(rx_step[2]), .C(n28944), .D(\rx_step[3] ), 
         .Z(n1657[3])) /* synthesis lut_function=(!(A ((D)+!C)+!A (B (C (D)+!C !(D))+!B (C+!(D))))) */ ;
    defparam i1_4_lut_4_lut.init = 16'h05e0;
    LUT4 i1_2_lut_rep_103_3_lut (.A(rx_step[1]), .B(rx_step[2]), .C(rx_step[0]), 
         .Z(n30134)) /* synthesis lut_function=(A+(B+(C))) */ ;
    defparam i1_2_lut_rep_103_3_lut.init = 16'hfefe;
    LUT4 i1_3_lut_rep_104_4_lut (.A(rx_step[1]), .B(rx_step[2]), .C(\rx_step[3] ), 
         .D(rx_samp_clk), .Z(n30135)) /* synthesis lut_function=(A (C+!(D))+!A (B (C+!(D))+!B !(C (D)))) */ ;
    defparam i1_3_lut_rep_104_4_lut.init = 16'he1ff;
    LUT4 i2132_2_lut_rep_117 (.A(rx_step[1]), .B(rx_step[2]), .Z(n30148)) /* synthesis lut_function=(A+(B)) */ ;
    defparam i2132_2_lut_rep_117.init = 16'heeee;
    FD1S3AX rx_clk_count_i0 (.D(n85[0]), .CK(clk_c), .Q(rx_clk_count[0]));
    defparam rx_clk_count_i0.GSR = "DISABLED";
    FD1S3IX rx_samp_clk_54 (.D(n81), .CK(clk_c), .CD(n30121), .Q(rx_samp_clk));
    defparam rx_samp_clk_54.GSR = "DISABLED";
    FD1S3IX rx_ok_buf_55 (.D(n30273), .CK(clk_c), .CD(n5), .Q(rx_ok));
    defparam rx_ok_buf_55.GSR = "DISABLED";
    FD1S3JX rx_baud_count_i0 (.D(n1), .CK(clk_c), .PD(n2), .Q(rx_baud_count[0]));
    defparam rx_baud_count_i0.GSR = "DISABLED";
    INV i2 (.A(rxbaud_fifo_tx_db9[2]), .Z(n2));
    LUT4 mux_13_i2_4_lut (.A(n36[1]), .B(rx_baud_count[2]), .C(n30121), 
         .D(n2443), .Z(n85[1])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i2_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i3_4_lut (.A(n36[2]), .B(rx_baud_count[3]), .C(n30121), 
         .D(n2443), .Z(n85[2])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i3_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i4_4_lut (.A(n36[3]), .B(rx_baud_count[4]), .C(n30121), 
         .D(n2443), .Z(n85[3])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i4_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i5_4_lut (.A(n36[4]), .B(rx_baud_count[5]), .C(n30121), 
         .D(n2443), .Z(n85[4])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i5_4_lut.init = 16'hc0ca;
    FD1S3IX rx_baud_count_i13 (.D(n4), .CK(clk_c), .CD(rxbaud_fifo_tx_db9[2]), 
            .Q(rx_baud_count[13]));
    defparam rx_baud_count_i13.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i12 (.D(n30160), .CK(clk_c), .CD(rxbaud_fifo_tx_db9[2]), 
            .Q(rx_baud_count[12]));
    defparam rx_baud_count_i12.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i11 (.D(n30159), .CK(clk_c), .CD(rxbaud_fifo_tx_db9[2]), 
            .Q(rx_baud_count[11]));
    defparam rx_baud_count_i11.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i10 (.D(n30158), .CK(clk_c), .CD(rxbaud_fifo_tx_db9[0]), 
            .Q(rx_baud_count[10]));
    defparam rx_baud_count_i10.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i9 (.D(n1761), .CK(clk_c), .Q(rx_baud_count[9]));
    defparam rx_baud_count_i9.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i8 (.D(n1), .CK(clk_c), .CD(n2), .Q(rx_baud_count[8]));
    defparam rx_baud_count_i8.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i7 (.D(n1763), .CK(clk_c), .Q(rx_baud_count[7]));
    defparam rx_baud_count_i7.GSR = "DISABLED";
    FD1S3IX rx_baud_count_i6 (.D(n30157), .CK(clk_c), .CD(n1_adj_1), .Q(rx_baud_count[6]));
    defparam rx_baud_count_i6.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i5 (.D(n1767), .CK(clk_c), .Q(rx_baud_count[5]));
    defparam rx_baud_count_i5.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i4 (.D(n1769), .CK(clk_c), .Q(rx_baud_count[4]));
    defparam rx_baud_count_i4.GSR = "DISABLED";
    FD1S3JX rx_baud_count_i3 (.D(n30156), .CK(clk_c), .PD(rxbaud_fifo_tx_db9[0]), 
            .Q(rx_baud_count[3]));
    defparam rx_baud_count_i3.GSR = "DISABLED";
    FD1S3AX rx_baud_count_i2 (.D(n1773), .CK(clk_c), .Q(rx_baud_count[2]));
    defparam rx_baud_count_i2.GSR = "DISABLED";
    FD1S3JX rx_baud_count_i1 (.D(n4), .CK(clk_c), .PD(n2), .Q(rx_baud_count[1]));
    defparam rx_baud_count_i1.GSR = "DISABLED";
    FD1P3IX rx_step_i3 (.D(n1657[3]), .SP(n61), .CD(n5), .CK(clk_c), 
            .Q(\rx_step[3] ));
    defparam rx_step_i3.GSR = "DISABLED";
    FD1S3IX rx_step_i2 (.D(n30204), .CK(clk_c), .CD(n5), .Q(rx_step[2]));
    defparam rx_step_i2.GSR = "DISABLED";
    FD1P3AX rx_step_i1 (.D(n201[1]), .SP(n29054), .CK(clk_c), .Q(rx_step[1]));
    defparam rx_step_i1.GSR = "DISABLED";
    LUT4 mux_13_i6_4_lut (.A(n36[5]), .B(rx_baud_count[6]), .C(n30121), 
         .D(n2443), .Z(n85[5])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i6_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i1_4_lut (.A(n36[0]), .B(rx_baud_count[1]), .C(n30121), 
         .D(n2443), .Z(n85[0])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i1_4_lut.init = 16'hc0ca;
    FD1S3IX rx_clk_count_i13 (.D(n36[13]), .CK(clk_c), .CD(n3667), .Q(rx_clk_count[13]));
    defparam rx_clk_count_i13.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i12 (.D(n85[12]), .CK(clk_c), .Q(rx_clk_count[12]));
    defparam rx_clk_count_i12.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i11 (.D(n85[11]), .CK(clk_c), .Q(rx_clk_count[11]));
    defparam rx_clk_count_i11.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i10 (.D(n85[10]), .CK(clk_c), .Q(rx_clk_count[10]));
    defparam rx_clk_count_i10.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i9 (.D(n85[9]), .CK(clk_c), .Q(rx_clk_count[9]));
    defparam rx_clk_count_i9.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i8 (.D(n85[8]), .CK(clk_c), .Q(rx_clk_count[8]));
    defparam rx_clk_count_i8.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i7 (.D(n85[7]), .CK(clk_c), .Q(rx_clk_count[7]));
    defparam rx_clk_count_i7.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i6 (.D(n85[6]), .CK(clk_c), .Q(rx_clk_count[6]));
    defparam rx_clk_count_i6.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i5 (.D(n85[5]), .CK(clk_c), .Q(rx_clk_count[5]));
    defparam rx_clk_count_i5.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i4 (.D(n85[4]), .CK(clk_c), .Q(rx_clk_count[4]));
    defparam rx_clk_count_i4.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i3 (.D(n85[3]), .CK(clk_c), .Q(rx_clk_count[3]));
    defparam rx_clk_count_i3.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i2 (.D(n85[2]), .CK(clk_c), .Q(rx_clk_count[2]));
    defparam rx_clk_count_i2.GSR = "DISABLED";
    FD1S3AX rx_clk_count_i1 (.D(n85[1]), .CK(clk_c), .Q(rx_clk_count[1]));
    defparam rx_clk_count_i1.GSR = "DISABLED";
    LUT4 mux_13_i7_4_lut (.A(n36[6]), .B(rx_baud_count[7]), .C(n30121), 
         .D(n2443), .Z(n85[6])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i7_4_lut.init = 16'hc0ca;
    CCU2D equal_9_14 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), .CIN(n27851), 
          .S0(n81));
    defparam equal_9_14.INIT0 = 16'hFFFF;
    defparam equal_9_14.INIT1 = 16'h0000;
    defparam equal_9_14.INJECT1_0 = "NO";
    defparam equal_9_14.INJECT1_1 = "NO";
    LUT4 mux_13_i8_4_lut (.A(n36[7]), .B(rx_baud_count[8]), .C(n30121), 
         .D(n2443), .Z(n85[7])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i8_4_lut.init = 16'hc0ca;
    FD1P3AX baud_back_i0_i2 (.D(rxbaud_fifo_tx_db9[2]), .SP(n5), .CK(clk_c), 
            .Q(baud_back[2]));
    defparam baud_back_i0_i2.GSR = "DISABLED";
    FD1P3AX baud_back_i0_i1 (.D(rxbaud_fifo_tx_db9[1]), .SP(n5), .CK(clk_c), 
            .Q(baud_back[1]));
    defparam baud_back_i0_i1.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i6 (.D(n30125), .SP(n29527), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[6]));
    defparam rx_dat_buf_i0_i6.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i5 (.D(n30125), .SP(n29478), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[5]));
    defparam rx_dat_buf_i0_i5.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i4 (.D(n30125), .SP(n29535), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[4]));
    defparam rx_dat_buf_i0_i4.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i3 (.D(n30125), .SP(n29469), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[3]));
    defparam rx_dat_buf_i0_i3.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i2 (.D(n30125), .SP(n28916), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[2]));
    defparam rx_dat_buf_i0_i2.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i1 (.D(n30125), .SP(n29468), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[1]));
    defparam rx_dat_buf_i0_i1.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i7 (.D(n30125), .SP(n29477), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[7]));
    defparam rx_dat_buf_i0_i7.GSR = "DISABLED";
    FD1P3IX rx_dat_buf_i0_i0 (.D(n30125), .SP(n16765), .CD(n5), .CK(clk_c), 
            .Q(rx_dat[0]));
    defparam rx_dat_buf_i0_i0.GSR = "DISABLED";
    FD1P3IX rx_step_i0 (.D(n30198), .SP(n16994), .CD(n5), .CK(clk_c), 
            .Q(rx_step[0]));
    defparam rx_step_i0.GSR = "DISABLED";
    LUT4 i2_3_lut_4_lut (.A(\rx_step[3] ), .B(n30155), .C(n16994), .D(n5), 
         .Z(n29054)) /* synthesis lut_function=(A (C+(D))+!A (B+(C+(D)))) */ ;
    defparam i2_3_lut_4_lut.init = 16'hfff4;
    LUT4 mux_13_i9_4_lut (.A(n36[8]), .B(rx_baud_count[9]), .C(n30121), 
         .D(n2443), .Z(n85[8])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i9_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i10_4_lut (.A(n36[9]), .B(rx_baud_count[10]), .C(n30121), 
         .D(n2443), .Z(n85[9])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i10_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i11_4_lut (.A(n36[10]), .B(rx_baud_count[11]), .C(n30121), 
         .D(n2443), .Z(n85[10])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i11_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i12_4_lut (.A(n36[11]), .B(rx_baud_count[12]), .C(n30121), 
         .D(n2443), .Z(n85[11])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i12_4_lut.init = 16'hc0ca;
    LUT4 mux_13_i13_4_lut (.A(n36[12]), .B(rx_baud_count[13]), .C(n30121), 
         .D(n2443), .Z(n85[12])) /* synthesis lut_function=(A (B (C+!(D))+!B !(C+(D)))+!A (B (C))) */ ;
    defparam mux_13_i13_4_lut.init = 16'hc0ca;
    CCU2D equal_9_14_26287 (.A0(n66[3]), .B0(rx_clk_count[3]), .C0(n66[2]), 
          .D0(rx_clk_count[2]), .A1(n66[1]), .B1(rx_clk_count[1]), .C1(n66[0]), 
          .D1(rx_clk_count[0]), .CIN(n27850), .COUT(n27851));
    defparam equal_9_14_26287.INIT0 = 16'h9009;
    defparam equal_9_14_26287.INIT1 = 16'h9009;
    defparam equal_9_14_26287.INJECT1_0 = "YES";
    defparam equal_9_14_26287.INJECT1_1 = "YES";
    CCU2D equal_9_12 (.A0(n66[7]), .B0(rx_clk_count[7]), .C0(n66[6]), 
          .D0(rx_clk_count[6]), .A1(n66[5]), .B1(rx_clk_count[5]), .C1(n66[4]), 
          .D1(rx_clk_count[4]), .CIN(n27849), .COUT(n27850));
    defparam equal_9_12.INIT0 = 16'h9009;
    defparam equal_9_12.INIT1 = 16'h9009;
    defparam equal_9_12.INJECT1_0 = "YES";
    defparam equal_9_12.INJECT1_1 = "YES";
    LUT4 rx_ok_bdd_3_lut (.A(rx_step[0]), .B(rx_step[1]), .C(\rx_step[3] ), 
         .Z(n30270)) /* synthesis lut_function=(A+!(B (C)+!B !(C))) */ ;
    defparam rx_ok_bdd_3_lut.init = 16'hbebe;
    LUT4 i1_3_lut (.A(rx_step[0]), .B(rx_step[1]), .C(rx_samp_clk), .Z(n1632[1])) /* synthesis lut_function=(!((B+!(C))+!A)) */ ;
    defparam i1_3_lut.init = 16'h2020;
    LUT4 i14912_4_lut (.A(n1652[1]), .B(n5), .C(n1632[1]), .D(\rx_step[3] ), 
         .Z(n201[1])) /* synthesis lut_function=(!(A (B+!(C+!(D)))+!A (B+!(C (D))))) */ ;
    defparam i14912_4_lut.init = 16'h3022;
    CCU2D sub_8_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_baud_count[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28752), .S1(n66[0]));
    defparam sub_8_add_2_1.INIT0 = 16'hF000;
    defparam sub_8_add_2_1.INIT1 = 16'h5555;
    defparam sub_8_add_2_1.INJECT1_0 = "NO";
    defparam sub_8_add_2_1.INJECT1_1 = "NO";
    CCU2D equal_9_10 (.A0(n66[11]), .B0(rx_clk_count[11]), .C0(n66[10]), 
          .D0(rx_clk_count[10]), .A1(n66[9]), .B1(rx_clk_count[9]), .C1(n66[8]), 
          .D1(rx_clk_count[8]), .CIN(n27848), .COUT(n27849));
    defparam equal_9_10.INIT0 = 16'h9009;
    defparam equal_9_10.INIT1 = 16'h9009;
    defparam equal_9_10.INJECT1_0 = "YES";
    defparam equal_9_10.INJECT1_1 = "YES";
    LUT4 i1_4_lut (.A(baud_back[2]), .B(baud_back[0]), .C(rxbaud_fifo_tx_db9[2]), 
         .D(rxbaud_fifo_tx_db9[0]), .Z(n4_adj_2)) /* synthesis lut_function=(!(A (B (C (D))+!B !((D)+!C))+!A !(B (C+!(D))+!B (C+(D))))) */ ;
    defparam i1_4_lut.init = 16'h7bde;
    LUT4 i2_3_lut (.A(baud_back[1]), .B(n4_adj_2), .C(rxbaud_fifo_tx_db9[1]), 
         .Z(n5)) /* synthesis lut_function=(A (B+!(C))+!A (B+(C))) */ ;
    defparam i2_3_lut.init = 16'hdede;
    LUT4 i11_3_lut (.A(rxbaud_fifo_tx_db9[0]), .B(rxbaud_fifo_tx_db9[2]), 
         .C(rxbaud_fifo_tx_db9[1]), .Z(n1763)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam i11_3_lut.init = 16'hcaca;
    LUT4 i1_2_lut (.A(rxbaud_fifo_tx_db9[0]), .B(rxbaud_fifo_tx_db9[1]), 
         .Z(n4)) /* synthesis lut_function=(!((B)+!A)) */ ;
    defparam i1_2_lut.init = 16'h2222;
    CCU2D sub_1288_add_2_15 (.A0(rx_clk_count[13]), .B0(rx_baud_count[13]), 
          .C0(GND_net), .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28838), .S1(n2443));
    defparam sub_1288_add_2_15.INIT0 = 16'h5999;
    defparam sub_1288_add_2_15.INIT1 = 16'h0000;
    defparam sub_1288_add_2_15.INJECT1_0 = "NO";
    defparam sub_1288_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_13 (.A0(rx_clk_count[11]), .B0(rx_baud_count[11]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[12]), .B1(rx_baud_count[12]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28837), .COUT(n28838));
    defparam sub_1288_add_2_13.INIT0 = 16'h5999;
    defparam sub_1288_add_2_13.INIT1 = 16'h5999;
    defparam sub_1288_add_2_13.INJECT1_0 = "NO";
    defparam sub_1288_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_11 (.A0(rx_clk_count[9]), .B0(rx_baud_count[9]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[10]), .B1(rx_baud_count[10]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28836), .COUT(n28837));
    defparam sub_1288_add_2_11.INIT0 = 16'h5999;
    defparam sub_1288_add_2_11.INIT1 = 16'h5999;
    defparam sub_1288_add_2_11.INJECT1_0 = "NO";
    defparam sub_1288_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_9 (.A0(rx_clk_count[7]), .B0(rx_baud_count[7]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[8]), .B1(rx_baud_count[8]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28835), .COUT(n28836));
    defparam sub_1288_add_2_9.INIT0 = 16'h5999;
    defparam sub_1288_add_2_9.INIT1 = 16'h5999;
    defparam sub_1288_add_2_9.INJECT1_0 = "NO";
    defparam sub_1288_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_7 (.A0(rx_clk_count[5]), .B0(rx_baud_count[5]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[6]), .B1(rx_baud_count[6]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28834), .COUT(n28835));
    defparam sub_1288_add_2_7.INIT0 = 16'h5999;
    defparam sub_1288_add_2_7.INIT1 = 16'h5999;
    defparam sub_1288_add_2_7.INJECT1_0 = "NO";
    defparam sub_1288_add_2_7.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_5 (.A0(rx_clk_count[3]), .B0(rx_baud_count[3]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[4]), .B1(rx_baud_count[4]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28833), .COUT(n28834));
    defparam sub_1288_add_2_5.INIT0 = 16'h5999;
    defparam sub_1288_add_2_5.INIT1 = 16'h5999;
    defparam sub_1288_add_2_5.INJECT1_0 = "NO";
    defparam sub_1288_add_2_5.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_3 (.A0(rx_clk_count[1]), .B0(rx_baud_count[1]), 
          .C0(GND_net), .D0(GND_net), .A1(rx_clk_count[2]), .B1(rx_baud_count[2]), 
          .C1(GND_net), .D1(GND_net), .CIN(n28832), .COUT(n28833));
    defparam sub_1288_add_2_3.INIT0 = 16'h5999;
    defparam sub_1288_add_2_3.INIT1 = 16'h5999;
    defparam sub_1288_add_2_3.INJECT1_0 = "NO";
    defparam sub_1288_add_2_3.INJECT1_1 = "NO";
    CCU2D sub_1288_add_2_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[0]), .B1(rx_baud_count[0]), .C1(GND_net), 
          .D1(GND_net), .COUT(n28832));
    defparam sub_1288_add_2_1.INIT0 = 16'h0000;
    defparam sub_1288_add_2_1.INIT1 = 16'h5999;
    defparam sub_1288_add_2_1.INJECT1_0 = "NO";
    defparam sub_1288_add_2_1.INJECT1_1 = "NO";
    CCU2D equal_9_0 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(n66[13]), .B1(rx_clk_count[13]), .C1(n66[12]), .D1(rx_clk_count[12]), 
          .COUT(n27848));
    defparam equal_9_0.INIT0 = 16'hF000;
    defparam equal_9_0.INIT1 = 16'h9009;
    defparam equal_9_0.INJECT1_0 = "NO";
    defparam equal_9_0.INJECT1_1 = "YES";
    LUT4 n30272_bdd_3_lut (.A(n30272), .B(rx_ok), .C(rx_step[2]), .Z(n30273)) /* synthesis lut_function=(A (B+!(C))+!A (B (C))) */ ;
    defparam n30272_bdd_3_lut.init = 16'hcaca;
    PFUMX mux_703_i2 (.BLUT(n29096), .ALUT(n174[1]), .C0(n30148), .Z(n1652[1]));
    INV i1 (.A(rxbaud_fifo_tx_db9[0]), .Z(n1));
    CCU2D add_6_15 (.A0(rx_clk_count[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28789), .S0(n36[13]));
    defparam add_6_15.INIT0 = 16'h5aaa;
    defparam add_6_15.INIT1 = 16'h0000;
    defparam add_6_15.INJECT1_0 = "NO";
    defparam add_6_15.INJECT1_1 = "NO";
    INV i3 (.A(rxbaud_fifo_tx_db9[1]), .Z(n1_adj_1));
    CCU2D add_6_13 (.A0(rx_clk_count[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28788), .COUT(n28789), .S0(n36[11]), .S1(n36[12]));
    defparam add_6_13.INIT0 = 16'h5aaa;
    defparam add_6_13.INIT1 = 16'h5aaa;
    defparam add_6_13.INJECT1_0 = "NO";
    defparam add_6_13.INJECT1_1 = "NO";
    CCU2D add_6_11 (.A0(rx_clk_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_clk_count[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28787), .COUT(n28788), .S0(n36[9]), .S1(n36[10]));
    defparam add_6_11.INIT0 = 16'h5aaa;
    defparam add_6_11.INIT1 = 16'h5aaa;
    defparam add_6_11.INJECT1_0 = "NO";
    defparam add_6_11.INJECT1_1 = "NO";
    CCU2D add_6_9 (.A0(rx_clk_count[7]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[8]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28786), .COUT(n28787), .S0(n36[7]), .S1(n36[8]));
    defparam add_6_9.INIT0 = 16'h5aaa;
    defparam add_6_9.INIT1 = 16'h5aaa;
    defparam add_6_9.INJECT1_0 = "NO";
    defparam add_6_9.INJECT1_1 = "NO";
    CCU2D add_6_7 (.A0(rx_clk_count[5]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[6]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28785), .COUT(n28786), .S0(n36[5]), .S1(n36[6]));
    defparam add_6_7.INIT0 = 16'h5aaa;
    defparam add_6_7.INIT1 = 16'h5aaa;
    defparam add_6_7.INJECT1_0 = "NO";
    defparam add_6_7.INJECT1_1 = "NO";
    CCU2D add_6_5 (.A0(rx_clk_count[3]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[4]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28784), .COUT(n28785), .S0(n36[3]), .S1(n36[4]));
    defparam add_6_5.INIT0 = 16'h5aaa;
    defparam add_6_5.INIT1 = 16'h5aaa;
    defparam add_6_5.INJECT1_0 = "NO";
    defparam add_6_5.INJECT1_1 = "NO";
    CCU2D add_6_3 (.A0(rx_clk_count[1]), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[2]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28783), .COUT(n28784), .S0(n36[1]), .S1(n36[2]));
    defparam add_6_3.INIT0 = 16'h5aaa;
    defparam add_6_3.INIT1 = 16'h5aaa;
    defparam add_6_3.INJECT1_0 = "NO";
    defparam add_6_3.INJECT1_1 = "NO";
    CCU2D add_6_1 (.A0(GND_net), .B0(GND_net), .C0(GND_net), .D0(GND_net), 
          .A1(rx_clk_count[0]), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .COUT(n28783), .S1(n36[0]));
    defparam add_6_1.INIT0 = 16'hF000;
    defparam add_6_1.INIT1 = 16'h5555;
    defparam add_6_1.INJECT1_0 = "NO";
    defparam add_6_1.INJECT1_1 = "NO";
    LUT4 i1646_2_lut_3_lut (.A(rx_step[0]), .B(rx_samp_clk), .C(rx_step[1]), 
         .Z(n174[1])) /* synthesis lut_function=(!(A (B (C)+!B !(C))+!A !(C))) */ ;
    defparam i1646_2_lut_3_lut.init = 16'h7878;
    LUT4 i1_2_lut_rep_135 (.A(rx_step[0]), .B(rx_samp_clk), .Z(n30166)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_rep_135.init = 16'h8888;
    CCU2D sub_8_add_2_15 (.A0(rx_baud_count[13]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(GND_net), .B1(GND_net), .C1(GND_net), .D1(GND_net), 
          .CIN(n28758), .S0(n66[13]));
    defparam sub_8_add_2_15.INIT0 = 16'h5555;
    defparam sub_8_add_2_15.INIT1 = 16'h0000;
    defparam sub_8_add_2_15.INJECT1_0 = "NO";
    defparam sub_8_add_2_15.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_13 (.A0(rx_baud_count[11]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[12]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28757), .COUT(n28758), .S0(n66[11]), .S1(n66[12]));
    defparam sub_8_add_2_13.INIT0 = 16'h5555;
    defparam sub_8_add_2_13.INIT1 = 16'h5555;
    defparam sub_8_add_2_13.INJECT1_0 = "NO";
    defparam sub_8_add_2_13.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_11 (.A0(rx_baud_count[9]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[10]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28756), .COUT(n28757), .S0(n66[9]), .S1(n66[10]));
    defparam sub_8_add_2_11.INIT0 = 16'h5555;
    defparam sub_8_add_2_11.INIT1 = 16'h5555;
    defparam sub_8_add_2_11.INJECT1_0 = "NO";
    defparam sub_8_add_2_11.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_9 (.A0(rx_baud_count[7]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[8]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28755), .COUT(n28756), .S0(n66[7]), .S1(n66[8]));
    defparam sub_8_add_2_9.INIT0 = 16'h5555;
    defparam sub_8_add_2_9.INIT1 = 16'h5555;
    defparam sub_8_add_2_9.INJECT1_0 = "NO";
    defparam sub_8_add_2_9.INJECT1_1 = "NO";
    CCU2D sub_8_add_2_3 (.A0(rx_baud_count[1]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[2]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28752), .COUT(n28753), .S0(n66[1]), .S1(n66[2]));
    defparam sub_8_add_2_3.INIT0 = 16'h5555;
    defparam sub_8_add_2_3.INIT1 = 16'h5555;
    defparam sub_8_add_2_3.INJECT1_0 = "NO";
    defparam sub_8_add_2_3.INJECT1_1 = "NO";
    PFUMX i28240 (.BLUT(n30271), .ALUT(n30270), .C0(rx_ok), .Z(n30272));
    CCU2D sub_8_add_2_5 (.A0(rx_baud_count[3]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[4]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28753), .COUT(n28754), .S0(n66[3]), .S1(n66[4]));
    defparam sub_8_add_2_5.INIT0 = 16'h5555;
    defparam sub_8_add_2_5.INIT1 = 16'h5555;
    defparam sub_8_add_2_5.INJECT1_0 = "NO";
    defparam sub_8_add_2_5.INJECT1_1 = "NO";
    LUT4 i20_3_lut_4_lut_3_lut (.A(rxbaud_fifo_tx_db9[0]), .B(rxbaud_fifo_tx_db9[1]), 
         .C(rxbaud_fifo_tx_db9[2]), .Z(n1767)) /* synthesis lut_function=(!(A (C)+!A !(B (C)+!B !(C)))) */ ;
    defparam i20_3_lut_4_lut_3_lut.init = 16'h4b4b;
    LUT4 i1_2_lut_rep_129 (.A(rxbaud_fifo_tx_db9[0]), .B(rxbaud_fifo_tx_db9[1]), 
         .Z(n30160)) /* synthesis lut_function=(!(A+!(B))) */ ;
    defparam i1_2_lut_rep_129.init = 16'h4444;
    LUT4 i1_2_lut_rep_128 (.A(rxbaud_fifo_tx_db9[0]), .B(rxbaud_fifo_tx_db9[1]), 
         .Z(n30159)) /* synthesis lut_function=(A+!(B)) */ ;
    defparam i1_2_lut_rep_128.init = 16'hbbbb;
    LUT4 i24_3_lut_4_lut_3_lut (.A(rxbaud_fifo_tx_db9[2]), .B(rxbaud_fifo_tx_db9[1]), 
         .C(rxbaud_fifo_tx_db9[0]), .Z(n1773)) /* synthesis lut_function=(!(A (B+(C))+!A (B (C)+!B !(C)))) */ ;
    defparam i24_3_lut_4_lut_3_lut.init = 16'h1616;
    LUT4 i22_2_lut_rep_127 (.A(rxbaud_fifo_tx_db9[2]), .B(rxbaud_fifo_tx_db9[1]), 
         .Z(n30158)) /* synthesis lut_function=(!(A (B)+!A !(B))) */ ;
    defparam i22_2_lut_rep_127.init = 16'h6666;
    LUT4 i23_4_lut_3_lut (.A(rxbaud_fifo_tx_db9[2]), .B(rxbaud_fifo_tx_db9[0]), 
         .C(rxbaud_fifo_tx_db9[1]), .Z(n1761)) /* synthesis lut_function=(!(A ((C)+!B)+!A !(B (C)+!B !(C)))) */ ;
    defparam i23_4_lut_3_lut.init = 16'h4949;
    CCU2D sub_8_add_2_7 (.A0(rx_baud_count[5]), .B0(GND_net), .C0(GND_net), 
          .D0(GND_net), .A1(rx_baud_count[6]), .B1(GND_net), .C1(GND_net), 
          .D1(GND_net), .CIN(n28754), .COUT(n28755), .S0(n66[5]), .S1(n66[6]));
    defparam sub_8_add_2_7.INIT0 = 16'h5555;
    defparam sub_8_add_2_7.INIT1 = 16'h5555;
    defparam sub_8_add_2_7.INJECT1_0 = "NO";
    defparam sub_8_add_2_7.INJECT1_1 = "NO";
    LUT4 i1928_2_lut_rep_126 (.A(rxbaud_fifo_tx_db9[2]), .B(rxbaud_fifo_tx_db9[0]), 
         .Z(n30157)) /* synthesis lut_function=(A (B)+!A !(B)) */ ;
    defparam i1928_2_lut_rep_126.init = 16'h9999;
    LUT4 i1_4_lut_3_lut (.A(rxbaud_fifo_tx_db9[1]), .B(rxbaud_fifo_tx_db9[2]), 
         .C(rxbaud_fifo_tx_db9[0]), .Z(n1769)) /* synthesis lut_function=(A (B+!(C))+!A !((C)+!B)) */ ;
    defparam i1_4_lut_3_lut.init = 16'h8e8e;
    LUT4 i28026_2_lut_rep_125 (.A(rxbaud_fifo_tx_db9[1]), .B(rxbaud_fifo_tx_db9[2]), 
         .Z(n30156)) /* synthesis lut_function=(!(A+(B))) */ ;
    defparam i28026_2_lut_rep_125.init = 16'h1111;
    PFUMX i28200 (.BLUT(n30202), .ALUT(n30203), .C0(rx_step[1]), .Z(n30204));
    LUT4 i2_3_lut_4_lut_adj_1 (.A(rx_step[1]), .B(rx_samp_clk), .C(rx_step[0]), 
         .D(rx_step[2]), .Z(n28944)) /* synthesis lut_function=(A (B (C (D)))) */ ;
    defparam i2_3_lut_4_lut_adj_1.init = 16'h8000;
    LUT4 i1_2_lut_3_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), .C(n16994), 
         .D(\rx_step[3] ), .Z(n61)) /* synthesis lut_function=(A (B (C+!(D))+!B (C))+!A (C)) */ ;
    defparam i1_2_lut_3_lut_4_lut.init = 16'hf0f8;
    LUT4 i29_else_4_lut (.A(rx_step[2]), .Z(n30202)) /* synthesis lut_function=(A) */ ;
    defparam i29_else_4_lut.init = 16'haaaa;
    LUT4 i1_2_lut_rep_124 (.A(rx_step[2]), .B(rx_step[1]), .Z(n30155)) /* synthesis lut_function=(A (B)) */ ;
    defparam i1_2_lut_rep_124.init = 16'h8888;
    LUT4 i29_then_4_lut (.A(rx_step[2]), .B(rx_samp_clk), .C(\rx_step[3] ), 
         .D(rx_step[0]), .Z(n30203)) /* synthesis lut_function=(A ((C+!(D))+!B)+!A !((C+!(D))+!B)) */ ;
    defparam i29_then_4_lut.init = 16'ha6aa;
    PFUMX i28198 (.BLUT(n30199), .ALUT(n30200), .C0(rx_step[0]), .Z(n16994));
    LUT4 i28134_else_4_lut (.A(n30125), .B(rx_step[1]), .C(rx_step[2]), 
         .D(\rx_step[3] ), .Z(n30199)) /* synthesis lut_function=(!(A (B (C (D))+!B (C (D)+!C !(D)))+!A (B (D)+!B (C (D))))) */ ;
    defparam i28134_else_4_lut.init = 16'h0bfd;
    LUT4 i28134_then_4_lut (.A(rx_step[1]), .B(rx_step[2]), .C(\rx_step[3] ), 
         .D(rx_samp_clk), .Z(n30200)) /* synthesis lut_function=(!(A (C)+!A (B (C)+!B !(C+(D))))) */ ;
    defparam i28134_then_4_lut.init = 16'h1f1e;
    PFUMX i28196 (.BLUT(n30196), .ALUT(n30197), .C0(rx_step[1]), .Z(n30198));
    LUT4 i28064_3_lut_else_3_lut (.A(rx_step[0]), .B(rx_samp_clk), .C(\rx_step[3] ), 
         .D(rx_step[2]), .Z(n30196)) /* synthesis lut_function=(!(A (B)+!A !(B+!(C+(D))))) */ ;
    defparam i28064_3_lut_else_3_lut.init = 16'h6667;
    LUT4 i28064_3_lut_then_3_lut (.A(rx_step[0]), .B(rx_samp_clk), .C(\rx_step[3] ), 
         .Z(n30197)) /* synthesis lut_function=(!(A (B+(C))+!A ((C)+!B))) */ ;
    defparam i28064_3_lut_then_3_lut.init = 16'h0606;
    LUT4 i1_2_lut_3_lut_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), .C(n30135), 
         .D(rx_step[0]), .Z(n29468)) /* synthesis lut_function=(!(A+((C+!(D))+!B))) */ ;
    defparam i1_2_lut_3_lut_4_lut_4_lut.init = 16'h0400;
    LUT4 rx_ok_bdd_4_lut (.A(rx_step[0]), .B(rx_step[1]), .C(\rx_step[3] ), 
         .D(rx_samp_clk), .Z(n30271)) /* synthesis lut_function=(!((B+!(C (D)))+!A)) */ ;
    defparam rx_ok_bdd_4_lut.init = 16'h2000;
    LUT4 i27964_2_lut_2_lut_3_lut_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), 
         .C(rx_step[0]), .D(n30135), .Z(n29527)) /* synthesis lut_function=(!(A+(B+(C+(D))))) */ ;
    defparam i27964_2_lut_2_lut_3_lut_4_lut_4_lut.init = 16'h0001;
    LUT4 i27986_2_lut_2_lut_3_lut_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), 
         .C(rx_step[0]), .D(n30135), .Z(n16765)) /* synthesis lut_function=(!(A+((C+(D))+!B))) */ ;
    defparam i27986_2_lut_2_lut_3_lut_4_lut_4_lut.init = 16'h0004;
    LUT4 i1_2_lut_3_lut_4_lut_4_lut_adj_2 (.A(rx_step[2]), .B(rx_step[1]), 
         .C(n30135), .D(rx_step[0]), .Z(n29478)) /* synthesis lut_function=(!(((C+!(D))+!B)+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_4_lut_adj_2.init = 16'h0800;
    LUT4 i1_2_lut_3_lut_4_lut_4_lut_adj_3 (.A(rx_step[2]), .B(rx_step[1]), 
         .C(n30135), .D(rx_step[0]), .Z(n29469)) /* synthesis lut_function=(!((B+(C+!(D)))+!A)) */ ;
    defparam i1_2_lut_3_lut_4_lut_4_lut_adj_3.init = 16'h0200;
    LUT4 i1_2_lut_3_lut_4_lut_4_lut_adj_4 (.A(rx_step[2]), .B(rx_step[1]), 
         .C(n30135), .D(rx_step[0]), .Z(n29477)) /* synthesis lut_function=(!(A+(B+(C+!(D))))) */ ;
    defparam i1_2_lut_3_lut_4_lut_4_lut_adj_4.init = 16'h0100;
    LUT4 i27981_2_lut_2_lut_3_lut_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), 
         .C(rx_step[0]), .D(n30135), .Z(n29535)) /* synthesis lut_function=(!(((C+(D))+!B)+!A)) */ ;
    defparam i27981_2_lut_2_lut_3_lut_4_lut_4_lut.init = 16'h0008;
    LUT4 i27966_2_lut_2_lut_3_lut_4_lut_4_lut (.A(rx_step[2]), .B(rx_step[1]), 
         .C(rx_step[0]), .D(n30135), .Z(n28916)) /* synthesis lut_function=(!((B+(C+(D)))+!A)) */ ;
    defparam i27966_2_lut_2_lut_3_lut_4_lut_4_lut.init = 16'h0002;
    LUT4 i2134_2_lut_4_lut (.A(n30134), .B(\rx_step[3] ), .C(n30125), 
         .D(n2443), .Z(n3667)) /* synthesis lut_function=(A (D)+!A (B (D)+!B ((D)+!C))) */ ;
    defparam i2134_2_lut_4_lut.init = 16'hff01;
    
endmodule

//
// Verilog Description of module fifo64x8_U2
//

module fifo64x8_U2 (rx_dat, clk_c, rx_ok, rden, n134, tx_dat, empty, 
            GND_net, VCC_net) /* synthesis syn_module_defined=1 */ ;
    input [7:0]rx_dat;
    input clk_c;
    input rx_ok;
    input rden;
    input n134;
    output [7:0]tx_dat;
    output empty;
    input GND_net;
    input VCC_net;
    
    wire Full;   // g:/zjf/mhub4k862-cpld/fifo64x8.v(19[17:21])
    
    FIFO8KB fifo64x8_0_0 (.DI0(rx_dat[0]), .DI1(rx_dat[1]), .DI2(rx_dat[2]), 
            .DI3(rx_dat[3]), .DI4(rx_dat[4]), .DI5(rx_dat[5]), .DI6(rx_dat[6]), 
            .DI7(rx_dat[7]), .DI8(GND_net), .DI9(GND_net), .DI10(GND_net), 
            .DI11(GND_net), .DI12(GND_net), .DI13(GND_net), .DI14(GND_net), 
            .DI15(GND_net), .DI16(GND_net), .DI17(GND_net), .FULLI(Full), 
            .EMPTYI(empty), .CSW1(VCC_net), .CSW0(VCC_net), .CSR1(VCC_net), 
            .CSR0(VCC_net), .WE(rx_ok), .RE(rden), .ORE(rden), .CLKW(clk_c), 
            .CLKR(clk_c), .RST(n134), .RPRST(n134), .DO9(tx_dat[0]), 
            .DO10(tx_dat[1]), .DO11(tx_dat[2]), .DO12(tx_dat[3]), .DO13(tx_dat[4]), 
            .DO14(tx_dat[5]), .DO15(tx_dat[6]), .DO16(tx_dat[7]), .EF(empty), 
            .FF(Full)) /* synthesis syn_instantiated=1 */ ;   // g:/zjf/mhub4k862-cpld/new_uart/mcu2pc_uart_top.v(35[10] 48[2])
    defparam fifo64x8_0_0.DATA_WIDTH_W = 18;
    defparam fifo64x8_0_0.DATA_WIDTH_R = 18;
    defparam fifo64x8_0_0.REGMODE = "NOREG";
    defparam fifo64x8_0_0.RESETMODE = "ASYNC";
    defparam fifo64x8_0_0.ASYNC_RESET_RELEASE = "SYNC";
    defparam fifo64x8_0_0.CSDECODE_W = "0b11";
    defparam fifo64x8_0_0.CSDECODE_R = "0b11";
    defparam fifo64x8_0_0.AEPOINTER = "0b00000001010000";
    defparam fifo64x8_0_0.AEPOINTER1 = "0b00000001100000";
    defparam fifo64x8_0_0.AFPOINTER = "0b00001111000000";
    defparam fifo64x8_0_0.AFPOINTER1 = "0b00001110110000";
    defparam fifo64x8_0_0.FULLPOINTER = "0b10000000000000";
    defparam fifo64x8_0_0.FULLPOINTER1 = "0b01111111110000";
    defparam fifo64x8_0_0.GSR = "DISABLED";
    
endmodule
