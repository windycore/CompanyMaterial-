`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:
// Design Name:    
// Module Name:    my_uart_top
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
module my_uart_top(
					clk     ,
					rst_n   ,
					bpssel  ,
					tx_int  ,
					tx_data ,
					rs232_tx,
					rdy     
				);

input      clk      ;	  // 50MHz主时钟
input      rst_n    ;	  //低电平复位信号
input [2:0] bpssel   ;     //波特率选择
input [7:0] tx_data  ;	  //接收数据寄存器，保存直至下一个数据来到
input      tx_int   ;		    

output     rs232_tx ;     //RS232发送数据信号
output     rdy      ;

//wire       bps_start;     //接收到数据后，波特率时钟启动信号置位
wire       clk_bps  ;     // clk_bps_r高电平为接收数据位的中间采样点,同时也作为发送数据的数据改变点 


//----------------------------------------------------				
speed_select   speed_tx(	
						  .clk      (clk      ),	//波特率选择模块
						  .rst_n    (rst_n    ),
						  .bps_start(rdy      ),
						  .bpssel   (bpssel   ),
						  .clk_bps  (clk_bps  )
						);

my_uart_tx	 my_uart_tx(		
						 .clk      (clk       ),	//发送数据模块
						 .rst_n    (rst_n     ),
						 .rx_data  (tx_data   ),
						 .rx_int   (tx_int    ),
						 .rs232_tx (rs232_tx  ),
						 .clk_bps  (clk_bps   ),
						// .bps_start(bps_start ),
						 .rdy      (rdy       )       
						);

endmodule
