
//`define USE_RS232_ZONE //不使用zone232串口则屏蔽
`define USE_RS232_402  //不使用402_232串口则屏蔽
//`define IR_N2N  //不使用本地红外切换则屏蔽
    
module IR_RS232_SW 
(
// **************  sys **************
    input     clk                , 
    input     rstn               ,
// **************  595 **************   
    input     rclk               ,//1BYTE 传输完成一个脉冲  
    input     srclk              ,
    input     ser                ,  
// **************  ir **************
    input     ir_eye             ,  //本机控制

    input     ir_front,
              
    input     ir_all_in          ,  //IR ALL IN 
    output    ir_all_out         ,  //IR ALL OUT
              
    input     [7:0] irx_i       ,  //本地IR IN
    output    [7:0] irx_o     ,  //本地IR OUT
              
    input     [7:0] ir_402x_i    , //远端IR IN  
    output    [7:0] ir_402x_o   , //远端IR OUT 
// **************  rs232 **************    
`ifdef USE_RS232_ZONE
    input     [7:0] rs232_zonex_tx, //本地串口
    output    [7:0] rs232_zonex_rx, //本地串口
`endif 
 
`ifdef USE_RS232_402           
    input     [7:0] rs232_402x_tx , //远端串口
    output    [7:0] rs232_402x_rx , //远端串口
`endif  
              
    input     rs232_db9_tx        , //指令串口
    output    rs232_db9_rx        , //指令串口
              
    input     rs232_tcp_tx        , //指令串口
    output    rs232_tcp_rx        , //指令串口  
              
    input     rs232_mcu_tx        , //CPLD连
    output    rs232_mcu_rx        ,	//指令串口
// **************  debug **************    			
//   output    debugpin0           ,
//    output    debugpin1           ,
//    output    debugpin2           ,
// **************  cpld_to_mcu **************    
    output    ir2mcu              ,				
    output    ack2mcu              //595接受数据后反馈给mcu的应答信号 
);

////////////////debug
reg debugreg0;
reg debugreg1;
////////////////
// ****************595****************
wire [7:0] con;	
hc74595 hc595_u(
	.srclrn (rstn  ),
	.rclk   (rclk  ),
	.srclk  (srclk ),
	.ser    (ser   ),
	.qa     (con[0]),
	.qb     (con[1]),
	.qc     (con[2]),
	.qd     (con[3]),
	.qe     (con[4]),
	.qf     (con[5]),
	.qg     (con[6]),
	.qh     (con[7]),
	.qhn    ()
);							
								
reg rclk_r0;
reg rclk_r1;
reg rclk_r2;
reg rclk_r3;
reg rclk_r ;
always@(posedge clk)begin
	rclk_r0 <= rclk;
	rclk_r1 <= rclk_r0;
	rclk_r2 <= rclk_r1;
	if(rclk_r2&&rclk_r1)
		rclk_r3 <= 1'b1;
	else if(~(rclk_r2||rclk_r1))
		rclk_r3 <= 1'b0;
	else
		rclk_r3 <= rclk_r3;
	rclk_r <= rclk_r3;
end
wire rclk_pos = (~rclk_r ) && rclk_r3;
wire rclk_neg = (~rclk_r3) && rclk_r ;
	
reg [7:0] con_reg;
always@(posedge clk)begin
	if(rclk_neg) con_reg <= con;
	else con_reg <= con_reg;
end
//////////////////////////////////////////	
reg ack;
always@(posedge clk)begin
	if(rclk_pos)ack <= 1'b1;
	else if(rclk_neg&&ack)ack <= 1'b0;
	else ack <= ack;
end

assign ack2mcu = ack;	

reg        txen   ;
wire [7:0] txdata ;
wire       rs232tx;
wire       rdy    ;
wire       empty  ;
wire       full   ;
wire       rden   ;

`ifdef USE_RS232_402 //如果有发送到远端402，则使用串口功能
	wire wren_w = wren&&(~full);
	fifo64x8 fifo_u1(
		.Data       (con   ), 
		.WrClock    (clk   ),  
		.RdClock    (clk   ), 
		.WrEn       (wren_w), 
		.RdEn       (rden  ), 
		.Reset      (~rstn ), 
		.RPReset    (~rstn ), 
		.Q          (txdata), 
		.Empty      (empty ), 
		.Full       (full  ),
		.AlmostEmpty(      ), 
		.AlmostFull (      )
	);
	my_uart_top  uart_u2(
		.clk     (clk     ),
		.rst_n   (rstn    ),
		.bpssel  (bps_sel ),
		.tx_int  (tx_int  ),
		.tx_data (txdata  ),
		.rs232_tx(rs232tx ),
		.rdy     (rdy     )
	);	

reg [10:0] wd_disable_402_to_mcu;//一段时间MCU不响应402的反馈信息
always@(posedge clk)begin
	if(!wd_disable_402_to_mcu[10]&&psg_clock_count15)wd_disable_402_to_mcu<=wd_disable_402_to_mcu+11'd1;
	if(!rs232tx)wd_disable_402_to_mcu<=11'd0;
end


`endif
					
always@(posedge clk,negedge rstn)
	if(!rstn)
		txen <= 1'b0;
	else begin	
		if(rdy)
			txen <= 1'b0;
		else if(!(rdy||empty))
			txen <= 1'b1;				
		else	
			txen <= txen;
	end
	
reg  rx_int0;
reg  rx_int1;	
always @ (posedge clk or negedge rstn) 
if(!rstn) begin
	rx_int0 <= 1'b0;
	rx_int1 <= 1'b0;
end
else begin
	rx_int0 <= txen   ;
	rx_int1 <= rx_int0;
end	

assign rden  =  rx_int0 && (~rx_int1);

reg tx_int;
always@(posedge clk)
	tx_int <= rden;
// ************************************************
reg [3:0] step       ;
reg [7:0] sp_com_data;//缓存收到的指令
`ifdef IR_N2N
	reg [7:0] ir_com_i   ;//本地红外输入端口
`endif
reg [7:0] ir_com_o   ;//本地红外输出端口

`ifdef USE_RS232_ZONE
	reg [7:0] rs_t_402_ch; //本地zone切换
`endif
reg [7:0] ir_402_com_i;//远端红外输入端口
//reg [7:0] ir_402_com_o;//远端红外输出端口 远端输出不需要切换

reg irn_rs       ;
reg [3:0] chnum  ;
reg [2:0] bps_sel0;
reg [2:0] bps_sel1;
reg [2:0] bps_sel2;
reg [2:0] bps_sel3;
reg [2:0] bps_sel4;
reg [2:0] bps_sel5;
//reg [2:0] bps_sel6;
//reg [2:0] bps_sel7;
reg [2:0] bps_sel;
reg pcto402_ack  ;
reg wren;
reg getting_com;
// ***************************
// MUC指令解析状态机（一段）
// ***************************
always@(posedge clk,negedge rstn)//指令解析状态机
begin
	if(!rstn)begin
		step<=4'd0;	
	end
	else if (rclk_neg)begin
		case (step)
			4'd0:begin
				
				case(con)
					8'hea  :begin step <= 4'd1;getting_com<=1'b1;end//指令
					`ifdef IR_N2N
						8'heb  :begin step <= 4'd3;getting_com<=1'b1;end//本地到本地
					`endif
					8'hec  :begin step <= 4'd5;getting_com<=1'b1;end//远端到本地
					`ifdef USE_RS232_ZONE
						8'hed  :begin step <= 4'd7;getting_com<=1'b1;end//本地DB9或者本机232信号(MCU) -> zone_232 
					`endif
					8'hee  :begin step <= 4'd9;getting_com<=1'b1;end//本机到远端串口 
					default:step <= 4'd0;
				endcase	
			end	
			//////////////////////////////
			4'd1:begin
				sp_com_data<=con;//特殊指令
				getting_com<=1'b0;
				step<=4'd15;//指令接收完成	
			end
			//////////////////////////////
			`ifdef IR_N2N
				4'd3:begin //本地到本地
				debugreg0<=1'b1;
					ir_com_i<=con-1'b1; 
					step<=4'd4;
				end
				4'd4:begin
					ir_com_o<=con-1'b1;
					getting_com<=1'b0;
					step<=4'd15;//指令接收完成	
				end
			`endif
			//////////////////////////////
			4'd5:begin //远端到本地
			debugreg0<=1'b0;
				ir_402_com_i<=con-1'b1; 
				step<=4'd6;
			end
			4'd6:begin
				ir_com_o<=con-1'b1;//本地输出
				getting_com<=1'b0;
				step<=4'd15;//指令接收完成	
			end
			/////////////////////////////
			`ifdef USE_RS232_ZONE
				4'd7:begin
					rs_t_402_ch<=con; //本地402
					getting_com<=1'b0;
					step<=4'd15;//指令接收完成	
				end
			`endif
			//////////////////////////////
			4'd9:begin
				irn_rs<=1'b1;
				if(con==8'hee)step <= 4'd9;
				else if(con==8'hef)begin step<=4'd0;irn_rs<=1'b0;getting_com<=1'b0;end
				else begin
					chnum<=con[3:0]; //本地in
					step<=4'd10;//指令接收完成	
				end
			end
			4'd10:begin
				if(con==8'hee)step <= 4'd9;
				else if(con==8'hef)begin step<=4'd0;irn_rs<=1'b0;getting_com<=1'b0;end
				else begin
					bps_sel<=con[2:0];
//					case(chnum)
//						4'd1:bps_sel0<=con[2:0];
//						4'd2:bps_sel1<=con[2:0];
//						4'd3:bps_sel2<=con[2:0];
//						4'd4:bps_sel3<=con[2:0];
//						4'd5:bps_sel4<=con[2:0];
//						4'd6:bps_sel5<=con[2:0];
//						4'd7:bps_sel6<=con[2:0];
//						4'd8:bps_sel7<=con[2:0];				
						//4'd9:begin
//							bps_sel0<=con[2:0];
//							bps_sel1<=con[2:0];
//							bps_sel2<=con[2:0];
//							bps_sel3<=con[2:0];
//							bps_sel4<=con[2:0];
//							bps_sel5<=con[2:0];
//							bps_sel6<=con[2:0];
//							bps_sel7<=con[2:0];
						//end
//					endcase 
					pcto402_ack <= 1'b1;
					step<=4'd11;//指令接收完成	
				end
			end
			4'd11:begin
				pcto402_ack <= 1'b0;
				if(con==8'hee)step <= 4'd9;
				else if(con==8'hef)begin step<=4'd0;irn_rs<=1'b0;getting_com<=1'b0;end
				else begin
					wren    <= 1'b1 ; 
				end	
			end
			//////////////////////////////
			4'd15:begin
				if(con==8'hef)begin
				getting_com<=1'b0;
				step<=4'd0;//回到初始
				irn_rs<=1'b0;
				end
			end

		endcase 
	end
	else begin
		pcto402_ack <= 1'b0;
		step        <= step  ;
		irn_rs      <= irn_rs ;
		chnum       <= chnum  ; 	
		bps_sel     <= bps_sel;
		wren        <= 1'b0   ;
	end
end
// ***************************
// 计数器
// ***************************
reg [15:0] clock_count;
always@(posedge clk,negedge rstn) //计数器，产生倍频时钟
begin	
	if(!rstn)	
		clock_count <= 15'd0 ;
	else 
		clock_count <= clock_count+1'd1;
end

// ***************************
// 远端红外默认电平检测
// ***************************
wire [7:0] def_voltag;
check_IR_defv check_IR_defv0
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[0])   ,
	.defv(def_voltag[0])     
);

check_IR_defv check_IR_defv1
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[1])   ,
	.defv(def_voltag[1])     
);

check_IR_defv check_IR_defv2
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[2])   ,
	.defv(def_voltag[2])     
);

check_IR_defv check_IR_defv3
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[3])   ,
	.defv(def_voltag[3])     
);

check_IR_defv check_IR_defv4
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[4])   ,
	.defv(def_voltag[4])     
);

check_IR_defv check_IR_defv5
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[5])   ,
	.defv(def_voltag[5])     
);

check_IR_defv check_IR_defv6
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[6])   ,
	.defv(def_voltag[6])     
);

check_IR_defv check_IR_defv7
(
	.clk(clock_count[15])     ,   //25M
	.rstn(rstn)    ,
	.ir_in(ir_402x_i[7])   ,
	.defv(def_voltag[7])     
);

reg [7:0] sw0_f2n;//远端到本地的红外切换开关
reg [7:0] sw1_f2n;//远端到本地的红外切换开关
reg [7:0] sw2_f2n;//远端到本地的红外切换开关
reg [7:0] sw3_f2n;//远端到本地的红外切换开关
reg [7:0] sw4_f2n;//远端到本地的红外切换开关
reg [7:0] sw5_f2n;//远端到本地的红外切换开关
reg [7:0] sw6_f2n;//远端到本地的红外切换开关
reg [7:0] sw7_f2n;//远端到本地的红外切换开关
always@(posedge clk) //远端到本地红外切换标志位
begin	
	if(!getting_com)begin //接收指令完成才处理
		case (ir_com_o[3:0])
			4'd0:sw0_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd1:sw1_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd2:sw2_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd3:sw3_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd4:sw4_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd5:sw5_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd6:sw6_f2n<=8'b1<<(ir_402_com_i[3:0]);
			4'd7:sw7_f2n<=8'b1<<(ir_402_com_i[3:0]);
			default :begin
				sw0_f2n<=sw0_f2n;
				sw1_f2n<=sw1_f2n;
				sw2_f2n<=sw2_f2n;
				sw3_f2n<=sw3_f2n;
				sw4_f2n<=sw4_f2n;
				sw5_f2n<=sw5_f2n;
				sw6_f2n<=sw6_f2n;
				sw7_f2n<=sw7_f2n;		
			end
		endcase
	end
end

wire [7:0] f2n_irx_level;//远端进来的红外改电平（根据默认电平）
assign f2n_irx_level = def_voltag^ir_402x_i;//由默认电平将远端输入电平转换

wire [7:0] f2n_irx_temp;

assign f2n_irx_temp[0] = (~sw0_f2n[0]|f2n_irx_level[0]) & (~sw1_f2n[0]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[0]|f2n_irx_level[2]) & (~sw3_f2n[0]|f2n_irx_level[3])
                       & (~sw4_f2n[0]|f2n_irx_level[4]) & (~sw5_f2n[0]|f2n_irx_level[5])
                       & (~sw6_f2n[0]|f2n_irx_level[6]) & (~sw7_f2n[0]|f2n_irx_level[7]);
assign f2n_irx_temp[1] = (~sw0_f2n[1]|f2n_irx_level[0]) & (~sw1_f2n[1]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[1]|f2n_irx_level[2]) & (~sw3_f2n[1]|f2n_irx_level[3])
                       & (~sw4_f2n[1]|f2n_irx_level[4]) & (~sw5_f2n[1]|f2n_irx_level[5])
                       & (~sw6_f2n[1]|f2n_irx_level[6]) & (~sw7_f2n[1]|f2n_irx_level[7]);
assign f2n_irx_temp[2] = (~sw0_f2n[2]|f2n_irx_level[0]) & (~sw1_f2n[2]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[2]|f2n_irx_level[2]) & (~sw3_f2n[2]|f2n_irx_level[3])
                       & (~sw4_f2n[2]|f2n_irx_level[4]) & (~sw5_f2n[2]|f2n_irx_level[5])
                       & (~sw6_f2n[2]|f2n_irx_level[6]) & (~sw7_f2n[2]|f2n_irx_level[7]);
assign f2n_irx_temp[3] = (~sw0_f2n[3]|f2n_irx_level[0]) & (~sw1_f2n[3]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[3]|f2n_irx_level[2]) & (~sw3_f2n[3]|f2n_irx_level[3])
                       & (~sw4_f2n[3]|f2n_irx_level[4]) & (~sw5_f2n[3]|f2n_irx_level[5])
                       & (~sw6_f2n[3]|f2n_irx_level[6]) & (~sw7_f2n[3]|f2n_irx_level[7]);
assign f2n_irx_temp[4] = (~sw0_f2n[4]|f2n_irx_level[0]) & (~sw1_f2n[4]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[4]|f2n_irx_level[2]) & (~sw3_f2n[4]|f2n_irx_level[3])
                       & (~sw4_f2n[4]|f2n_irx_level[4]) & (~sw5_f2n[4]|f2n_irx_level[5])
                       & (~sw6_f2n[4]|f2n_irx_level[6]) & (~sw7_f2n[4]|f2n_irx_level[7]);
assign f2n_irx_temp[5] = (~sw0_f2n[5]|f2n_irx_level[0]) & (~sw1_f2n[5]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[5]|f2n_irx_level[2]) & (~sw3_f2n[5]|f2n_irx_level[3])
                       & (~sw4_f2n[5]|f2n_irx_level[4]) & (~sw5_f2n[5]|f2n_irx_level[5])
                       & (~sw6_f2n[5]|f2n_irx_level[6]) & (~sw7_f2n[5]|f2n_irx_level[7]);
assign f2n_irx_temp[6] = (~sw0_f2n[6]|f2n_irx_level[0]) & (~sw1_f2n[6]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[6]|f2n_irx_level[2]) & (~sw3_f2n[6]|f2n_irx_level[3])
                       & (~sw4_f2n[6]|f2n_irx_level[4]) & (~sw5_f2n[6]|f2n_irx_level[5])
                       & (~sw6_f2n[6]|f2n_irx_level[6]) & (~sw7_f2n[6]|f2n_irx_level[7]);
assign f2n_irx_temp[7] = (~sw0_f2n[7]|f2n_irx_level[0]) & (~sw1_f2n[7]|f2n_irx_level[1]) //本地到本地矩阵切换 
                       & (~sw2_f2n[7]|f2n_irx_level[2]) & (~sw3_f2n[7]|f2n_irx_level[3])
                       & (~sw4_f2n[7]|f2n_irx_level[4]) & (~sw5_f2n[7]|f2n_irx_level[5])
                       & (~sw6_f2n[7]|f2n_irx_level[6]) & (~sw7_f2n[7]|f2n_irx_level[7]);   
                                           
// *********************************************************************************
// 解析指令，看是否需要载波，生成38K的载波
// *********************************************************************************
reg [2:0] to_db9_baud;
initial begin
	to_db9_baud=3'd0;
end

wire	rs_cmd_idle= (step==4'b00);
reg		ir_car_care;	
reg     f_to_mcu_en;//远端空本机开关
	always@(posedge clk or negedge rstn)
	begin
		if(!rstn)begin
			to_db9_baud <= 3'd0;
			ir_car_care <=1'd0;
			f_to_mcu_en<=1'd0;//default 0 closed
		end
		else begin
			if(sp_com_data[7:4]==4'hd)begin
				if(sp_com_data[3:0]==4'hf)ir_car_care <=1;
				else if(sp_com_data[3:0]==4'he)ir_car_care <=0;
				else if(sp_com_data[3:0]==4'h0)f_to_mcu_en <=1'd0;
				else if(sp_com_data[3:0]==4'h1)f_to_mcu_en <=1'd1;
			end
			else if(sp_com_data[7:4]==4'hc)begin
				to_db9_baud <= sp_com_data[2:0];
			end
			else begin
				to_db9_baud<=to_db9_baud;
				ir_car_care<=ir_car_care;
			end	
		end	
//		else if(sp_com_data==8'hdf && rs_cmd_idle)//关心载波
//			ir_car_care <=1;
//		else if(sp_com_data==8'hde && rs_cmd_idle)//不关心载波
//			ir_car_care <=0;
	end
reg	[9:0]	car_cnt;
	always@(posedge clk or negedge rstn)
	begin
		if(!rstn)
			car_cnt <=0;
		else if(car_cnt <658)
			car_cnt <=car_cnt +1;
		else
			car_cnt <=1;
	end
wire	car_clk =(car_cnt <329);//38K 载波

// *********************************************************************************
// 检测信号是否带载波
// *********************************************************************************
wire [7:0] irx_car_en;
ir_car_detect	ir_car_detect0
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[0]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[0]  )
);
ir_car_detect	ir_car_detect1
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[1]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[1]  )
);
ir_car_detect	ir_car_detect2
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[2]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[2]  )
);
ir_car_detect	ir_car_detect3
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[3]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[3]  )
);
ir_car_detect	ir_car_detect4
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[4]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[4]  )
);
ir_car_detect	ir_car_detect5
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[5]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[5]  )
);
ir_car_detect	ir_car_detect6
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[6]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[6]  )
);
ir_car_detect	ir_car_detect7
(
	.clk          (clk            ),
	.rst_n        (rstn           ),
	.ir_sd_i      (f2n_irx_temp[7]),
	.ir_car_care_i(ir_car_care	  ),
	.ir_car_en_o  (irx_car_en[7]  )
);

wire ir_eye_out;

ir_def_lv#
(
	.BIT_NUM(3'd1)	, //本地红外输入数
	.IO_NUM	(7'd1)  //实际8位
)
ir_def_lv_inst1  
( 
	.ir_defX_in		(ir_eye		),  
	.ir_defH_out	(ir_eye_out    ),
	.rstn			(rstn			), 
	.clk_ms			(clk			)        //25M
); 






// *********************************************************************************
// 有载波就加上载波，准备最后输出
// *********************************************************************************
wire [7:0] f2n_irx_car_temp;
assign f2n_irx_car_temp[0] = irx_car_en[0] && ~f2n_irx_temp[0] ? car_clk :f2n_irx_temp[0];
assign f2n_irx_car_temp[1] = irx_car_en[1] && ~f2n_irx_temp[1] ? car_clk :f2n_irx_temp[1];
assign f2n_irx_car_temp[2] = irx_car_en[2] && ~f2n_irx_temp[2] ? car_clk :f2n_irx_temp[2];
assign f2n_irx_car_temp[3] = irx_car_en[3] && ~f2n_irx_temp[3] ? car_clk :f2n_irx_temp[3];
assign f2n_irx_car_temp[4] = irx_car_en[4] && ~f2n_irx_temp[4] ? car_clk :f2n_irx_temp[4];
assign f2n_irx_car_temp[5] = irx_car_en[5] && ~f2n_irx_temp[5] ? car_clk :f2n_irx_temp[5];
assign f2n_irx_car_temp[6] = irx_car_en[6] && ~f2n_irx_temp[6] ? car_clk :f2n_irx_temp[6];
assign f2n_irx_car_temp[7] = irx_car_en[7] && ~f2n_irx_temp[7] ? car_clk :f2n_irx_temp[7];

// *********************************************************************************
// 本地红外切换
// *********************************************************************************
`ifdef IR_N2N
	reg [7:0] sw0_n2n;//本地的红外切换开关
	reg [7:0] sw1_n2n;//本地的红外切换开关
	reg [7:0] sw2_n2n;//本地的红外切换开关
	reg [7:0] sw3_n2n;//本地的红外切换开关
	reg [7:0] sw4_n2n;//本地的红外切换开关
	reg [7:0] sw5_n2n;//本地的红外切换开关
	reg [7:0] sw6_n2n;//本地的红外切换开关
	reg [7:0] sw7_n2n;//本地的红外切换开关
	always@(posedge clk) //本地红外切换标志位
	begin	
		//sw0_n2n<=8'b11111111;
		if(!getting_com)begin //接收指令完成才处理
		case (ir_com_o[3:0])
			4'd0:sw0_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd1:sw1_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd2:sw2_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd3:sw3_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd4:sw4_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd5:sw5_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd6:sw6_n2n<=8'b1<<(ir_com_i[3:0]);
			4'd7:sw7_n2n<=8'b1<<(ir_com_i[3:0]);
			default :begin
				sw0_n2n<=sw0_n2n;
				sw1_n2n<=sw1_n2n;
				sw2_n2n<=sw2_n2n;
				sw3_n2n<=sw3_n2n;
				sw4_n2n<=sw4_n2n;
				sw5_n2n<=sw5_n2n;
				sw6_n2n<=sw6_n2n;
				sw7_n2n<=sw7_n2n;		
			end
		endcase
		end
	end
	wire [7:0] n2n_irx_temp; //本地到本地输出最后缓存
	assign n2n_irx_temp[0] = (sw0_n2n[0]?irx_i[0]:1) & (sw1_n2n[0]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[0]?irx_i[2]:1) & (sw3_n2n[0]?irx_i[3]:1)
	                       & (sw4_n2n[0]?irx_i[4]:1) & (sw5_n2n[0]?irx_i[5]:1)
	                       & (sw6_n2n[0]?irx_i[6]:1) & (sw7_n2n[0]?irx_i[7]:1);
	assign n2n_irx_temp[1] = (sw0_n2n[1]?irx_i[0]:1) & (sw1_n2n[1]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[1]?irx_i[2]:1) & (sw3_n2n[1]?irx_i[3]:1)
	                       & (sw4_n2n[1]?irx_i[4]:1) & (sw5_n2n[1]?irx_i[5]:1)
	                       & (sw6_n2n[1]?irx_i[6]:1) & (sw7_n2n[1]?irx_i[7]:1);
	assign n2n_irx_temp[2] = (sw0_n2n[2]?irx_i[0]:1) & (sw1_n2n[2]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[2]?irx_i[2]:1) & (sw3_n2n[2]?irx_i[3]:1)
	                       & (sw4_n2n[2]?irx_i[4]:1) & (sw5_n2n[2]?irx_i[5]:1)
	                       & (sw6_n2n[2]?irx_i[6]:1) & (sw7_n2n[2]?irx_i[7]:1);
	assign n2n_irx_temp[3] = (sw0_n2n[3]?irx_i[0]:1) & (sw1_n2n[3]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[3]?irx_i[2]:1) & (sw3_n2n[3]?irx_i[3]:1)
	                       & (sw4_n2n[3]?irx_i[4]:1) & (sw5_n2n[3]?irx_i[5]:1)
	                       & (sw6_n2n[3]?irx_i[6]:1) & (sw7_n2n[3]?irx_i[7]:1);
	assign n2n_irx_temp[4] = (sw0_n2n[4]?irx_i[0]:1) & (sw1_n2n[4]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[4]?irx_i[2]:1) & (sw3_n2n[4]?irx_i[3]:1)
	                       & (sw4_n2n[4]?irx_i[4]:1) & (sw5_n2n[4]?irx_i[5]:1)
	                       & (sw6_n2n[4]?irx_i[6]:1) & (sw7_n2n[4]?irx_i[7]:1);
	assign n2n_irx_temp[5] = (sw0_n2n[5]?irx_i[0]:1) & (sw1_n2n[5]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[5]?irx_i[2]:1) & (sw3_n2n[5]?irx_i[3]:1)
	                       & (sw4_n2n[5]?irx_i[4]:1) & (sw5_n2n[5]?irx_i[5]:1)
	                       & (sw6_n2n[5]?irx_i[6]:1) & (sw7_n2n[5]?irx_i[7]:1);
	assign n2n_irx_temp[6] = (sw0_n2n[6]?irx_i[0]:1) & (sw1_n2n[6]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[6]?irx_i[2]:1) & (sw3_n2n[6]?irx_i[3]:1)
	                       & (sw4_n2n[6]?irx_i[4]:1) & (sw5_n2n[6]?irx_i[5]:1)
	                       & (sw6_n2n[6]?irx_i[6]:1) & (sw7_n2n[6]?irx_i[7]:1);
	assign n2n_irx_temp[7] = (sw0_n2n[7]?irx_i[0]:1) & (sw1_n2n[7]?irx_i[1]:1) //本地到本地矩阵切换 
	                       & (sw2_n2n[7]?irx_i[2]:1) & (sw3_n2n[7]?irx_i[3]:1)
	                       & (sw4_n2n[7]?irx_i[4]:1) & (sw5_n2n[7]?irx_i[5]:1)
	                       & (sw6_n2n[7]?irx_i[6]:1) & (sw7_n2n[7]?irx_i[7]:1); 
`endif                   
// ***************************************************************
// 远端IRin到本地IRout
// ***************************************************************	
`ifdef IR_N2N 
	assign irx_o = n2n_irx_temp & f2n_irx_car_temp;//两路都从本地IROUT输出
`else
	assign irx_o = f2n_irx_car_temp;//两路都从本地IROUT输出
`endif
assign ir_all_out = &irx_o; //位缩
// ***************************************************************
// 本地IRin到远端IRout
// ***************************************************************					  
assign ir_402x_o=irx_i;//远端输出与本地输入一一对应	

// ***************************************************************
// ir到MCU
// ***************************************************************	
/*
wire [7:0] 	ir_402_to_mcu;
ir_rx ir_rx_isnt0
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[0]),
	.ir_sd_o(ir_402_to_mcu[0])
);
ir_rx ir_rx_isnt1
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[1]),
	.ir_sd_o(ir_402_to_mcu[1])
);
ir_rx ir_rx_isnt2
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[2]),
	.ir_sd_o(ir_402_to_mcu[2])
);
ir_rx ir_rx_isnt3
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[3]),
	.ir_sd_o(ir_402_to_mcu[3])
);
ir_rx ir_rx_isnt4
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[4]),
	.ir_sd_o(ir_402_to_mcu[4])
);
ir_rx ir_rx_isnt5
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[5]),
	.ir_sd_o(ir_402_to_mcu[5])
);
ir_rx ir_rx_isnt6
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[6]),
	.ir_sd_o(ir_402_to_mcu[6])
);
ir_rx ir_rx_isnt7
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(f2n_irx_level[7]),
	.ir_sd_o(ir_402_to_mcu[7])
);
wire ir_eye_to_mcu; //ir_eye
ir_rx ir_rx_isnt8
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(ir_eye          ),
	.ir_sd_o(ir_eye_to_mcu   )
);

wire ir_front_to_mcu;
ir_rx ir_rx_isnt9
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(ir_front          ),
	.ir_sd_o(ir_front_to_mcu   )
);
assign  ir2mcu = (&ir_402_to_mcu)&ir_eye_to_mcu&ir_front_to_mcu; //远端可ireye都可以控mcu
*/
 
wire ir2mcu_bf_flit=(&f2n_irx_level)&&ir_front&&ir_eye_out;
ir_rx ir_rx_isntx
(
	.clk    (clk             ),//25M	
	.rst_n  (rstn            ),
	.ir_sd_i(ir2mcu_bf_flit  ),
	.ir_sd_o(ir2mcu   )
);




// ***************************************************************		  
// 串口	
// ***************************************************************
/////////////////////////////////////////
///////////////////////////////////////////////
reg rs_402_all_send;
reg [7:0] send_to_402;
always @(posedge clk)begin
	if(chnum>0&&chnum<9) begin
		send_to_402 <= (~(8'b1<<(chnum-1)))|{8{rs232tx}};//cpld出来的串口选择哪个402出去
	end
	else if(chnum==9)begin
			send_to_402 <= {8{rs232tx}};//cpld出来的串口从7路出去
	end
	else begin
		send_to_402<=8'b11111111;
	end
end

//reg [2:0] rxbaud_fifo_tx_db9;//准备发送到DB9的串口缓存模块RX端的波特率
//reg [2:0] rxbaud_fifo_tx_mcu;//准备发送到mcu的串口缓存模块RX端的波特率
//reg db9_mcu_en;
//wire rs232_402_all=(&rs232_402x_tx);
//always @(posedge clk)begin
//	if(!rs232_mcu_tx)begin//单片机在发送
//		rxbaud_fifo_tx_db9<=3'd0;//解析单片机的数据必须9600
//		db9_mcu_en<=1'd0;//表面此时是MCU发
//	end
//	else if(!rs232_402_all)begin//402在发送
//		rxbaud_fifo_tx_db9<=bps_sel;
//		db9_mcu_en<=1'd1;
//	end
//end

//wire rs232_mcu_tx_changebaud;
//mcu2pc_uart_top mcu2pc_uart_top1
//(
//	.clk(clk),
//	.rstn(rstn),
//	.rx(rs232_mcu_tx&rs232_402_all),
//	.rx_baud(rxbaud_fifo_tx_db9),//接收MCU固定9600 402则是传的波特率
//	.tx_baud(to_db9_baud),	
//	.tx(rs232_mcu_tx_changebaud)
//);

//wire rs232_to_mcu=db9_mcu_en?rs232_mcu_tx_changebaud:1'd1;//是402发的 绕一圈后再给MCU ，MCU发的不需要再给mcu
//wire rs232_db9_tx_changebaud;
//pc2mcu_uart_top pc2mcu_uart_top1
//(
//	.clk(clk),
//	.rstn(rstn),
//	.rx(rs232_to_mcu&rs232_db9_tx),	
//	.rx_baud(to_db9_baud),
//	.tx_baud(3'd0),//发送到MCU固定9600
//	.tx(rs232_db9_tx_changebaud)
//);
//assign rs232_db9_rx  =  rs232_mcu_tx_changebaud;//db9wait402ack ? rs232_402_tx && mcu_rs232_tx:1'b1;
//assign rs232_tcp_rx  =  rs232_mcu_tx;

//assign rs232_mcu_rx  = rs232_db9_tx_changebaud & rs232_tcp_tx;
//assign rs232_402x_rx = send_to_402;//现在只有DB9-MCU-CPLD-402,以后加本地zone

///////////////////////////////////////////////////////////////////////////////////////
reg clock_count15_reg0;
reg clock_count15_reg1;
wire psg_clock_count15=clock_count15_reg0&&(~clock_count15_reg1);
always @(posedge clk)begin
	clock_count15_reg0<=clock_count[15];
	clock_count15_reg1<=clock_count15_reg0;
end

reg [2:0] rxbaud_fifo_tx_db9;//准备发送到DB9的串口缓存模块RX端的波特率
reg [2:0] rxbaud_fifo_tx_mcu;//准备发送到mcu的串口缓存模块RX端的波特率
reg db9_402_en;//区分是DB9还是402发送的数据

initial begin
	rxbaud_fifo_tx_db9=3'd0;
	rxbaud_fifo_tx_mcu=3'd0;
	db9_402_en=1'd0;
	clock_count15_reg0=1'd0;
	clock_count15_reg1=1'd0;
	f_to_mcu_en=1'd0;
	bps_sel=3'd0;
	to_db9_baud=3'd0;
end
wire rs232_402_all=(&rs232_402x_tx[5:0]);
reg [2:0] rs232_402_all_back;
always @(posedge clk)begin
	rs232_402_all_back[0]<=rs232_402_all;
	rs232_402_all_back[1]<=rs232_402_all_back[0];
	rs232_402_all_back[2]<=rs232_402_all_back[1];
	if(!rs232_mcu_tx)begin//单片机在发送
		rxbaud_fifo_tx_db9<=3'd0;//解析单片机的数据必须9600
		db9_402_en<=1'd0;//表面此时是MCU发
	end
	else if(!(|rs232_402_all_back))begin//402在发送
//		if     (!rs232_402x_tx[0])rxbaud_fifo_tx_db9<=bps_sel0;
//		else if(!rs232_402x_tx[1])rxbaud_fifo_tx_db9<=bps_sel1;
//		else if(!rs232_402x_tx[2])rxbaud_fifo_tx_db9<=bps_sel2;
//		else if(!rs232_402x_tx[3])rxbaud_fifo_tx_db9<=bps_sel3;
//		else if(!rs232_402x_tx[4])rxbaud_fifo_tx_db9<=bps_sel4;
//		else if(!rs232_402x_tx[5])rxbaud_fifo_tx_db9<=bps_sel5;
//		else if(!rs232_402x_tx[6])rxbaud_fifo_tx_db9<=bps_sel6;
//		else if(!rs232_402x_tx[7])rxbaud_fifo_tx_db9<=bps_sel7;
		rxbaud_fifo_tx_db9<=bps_sel;
		db9_402_en<=1'd1;
	end
end

wire rs232_send_to_db9_changebaud;
mcu2pc_uart_top mcu2pc_uart_top1
(
	.clk(clk),
	.rstn(rstn),
	.rx(rs232_mcu_tx&rs232_402_all),
	.rx_baud(rxbaud_fifo_tx_db9),//接收MCU固定9600 402则是传的波特率
	.tx_baud(to_db9_baud),	
	.tx(rs232_send_to_db9_changebaud)
);

wire rs232_to_mcu=(wd_disable_402_to_mcu[10]&&f_to_mcu_en&&db9_402_en)?rs232_send_to_db9_changebaud:1'd1;//是402发的 绕一圈后再给MCU ，MCU发的不需要再给mcu
wire rs232_send_to_mcu_changebaud;
pc2mcu_uart_top pc2mcu_uart_top1
(
	.clk(clk),
	.rstn(rstn),
	.rx(rs232_to_mcu&rs232_db9_tx),	
	.rx_baud(to_db9_baud),
	.tx_baud(3'd0),//发送到MCU固定9600
	.tx(rs232_send_to_mcu_changebaud)
);
assign rs232_db9_rx  =  rs232_send_to_db9_changebaud;//db9wait402ack ? rs232_402_tx && mcu_rs232_tx:1'b1;
assign rs232_tcp_rx  =  rs232_mcu_tx;

assign rs232_mcu_rx  = rs232_send_to_mcu_changebaud & rs232_tcp_tx;
assign rs232_402x_rx = send_to_402;//现在只有DB9-MCU-CPLD-402,以后加本地zone


endmodule
