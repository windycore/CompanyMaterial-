`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:  
// Design Name:    
// Module Name:    speed_select
// Project Name:   
// Target Device:  
// Tool versions:  
// Description:
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////
module speed_select(
						clk      ,
						rst_n    ,
						bps_start,
						bpssel   ,
						clk_bps
					);

input       clk      ;	// 50MHz主时钟
input       rst_n    ;	//低电平复位信号
input [2:0] bpssel   ;   //波特率选择
input       bps_start;   //接收到数据后，波特率时钟启动信号置位
output      clk_bps  ;	// clk_bps的高电平为接收或者发送数据位的中间采样点 


parameter 		bps2400     = 10415,
				bps4800     = 5207,
				bps9600 	= 2603,	//波特率为9600bps
			 	bps19200 	= 1301,	//波特率为19200bps
				bps38400 	= 650,	//波特率为38400bps
				bps57600 	= 433,	//波特率为57600bps
				bps115200	= 216;	//波特率为115200bps
/*
parameter 		bps4800_2   = 2603,
				bps9600_2 	= 1301,
				bps19200_2	= 650 ,
				bps38400_2	= 325 ,
				bps57600_2	= 216 ,
				bps115200_2 = 108 ;  
*/				

//以下波特率分频计数值可参照上面的参数进行更改
//`define		BPS_PARA		5207	//波特率为9600时的分频计数值
//`define 	BPS_PARA_2		2603	//波特率为9600时的分频计数值的一半，用于数据采样

reg[13:0] cnt      ;			    //分频计数
reg       clk_bps_r;			    //波特率时钟寄存器

//----------------------------------------------------------
	reg [13:0]bpcnt;
	always@(*)
	begin
		case(bpssel)
//			3'd0:bpcnt <= bps2400   ;			
//			3'd1:bpcnt <= bps4800   ;
//			3'd2:bpcnt <= bps9600   ;
//			3'd3:bpcnt <= bps19200  ;
//			3'd4:bpcnt <= bps38400  ;
//			3'd5:bpcnt <= bps57600  ;
//			3'd6:bpcnt <= bps115200 ;
//			default:bpcnt <= bps4800;
			3'd0:bpcnt <= bps9600;
			3'd1:bpcnt <= bps2400   ;			
			3'd2:bpcnt <= bps4800   ;
			3'd3:bpcnt <= bps9600   ;
			3'd4:bpcnt <= bps19200  ;
			3'd5:bpcnt <= bps38400  ;
			3'd6:bpcnt <= bps57600  ;
			3'd7:bpcnt <= bps115200 ;		
		endcase
	end
	
//----------------------------------------------------------

always @ (posedge clk or negedge rst_n)
	if(!rst_n) cnt <= 14'd0;
	else if((cnt >= bpcnt) || !bps_start)
		cnt       <= 14'd0;	  //波特率计数清零
	else
		cnt       <= cnt+1'b1;//波特率时钟计数启动

always @ (posedge clk or negedge rst_n)
	if(!rst_n) clk_bps_r <= 1'b0;
	else if(cnt >= bpcnt)
		clk_bps_r <= 1'b1;
	else 	                  
		clk_bps_r <= 1'b0;

assign clk_bps = clk_bps_r;

endmodule



