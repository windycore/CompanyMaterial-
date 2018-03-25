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

input      clk      ;	  // 50MHz��ʱ��
input      rst_n    ;	  //�͵�ƽ��λ�ź�
input [2:0] bpssel   ;     //������ѡ��
input [7:0] tx_data  ;	  //�������ݼĴ���������ֱ����һ����������
input      tx_int   ;		    

output     rs232_tx ;     //RS232���������ź�
output     rdy      ;

//wire       bps_start;     //���յ����ݺ󣬲�����ʱ�������ź���λ
wire       clk_bps  ;     // clk_bps_r�ߵ�ƽΪ��������λ���м������,ͬʱҲ��Ϊ�������ݵ����ݸı�� 


//----------------------------------------------------				
speed_select   speed_tx(	
						  .clk      (clk      ),	//������ѡ��ģ��
						  .rst_n    (rst_n    ),
						  .bps_start(rdy      ),
						  .bpssel   (bpssel   ),
						  .clk_bps  (clk_bps  )
						);

my_uart_tx	 my_uart_tx(		
						 .clk      (clk       ),	//��������ģ��
						 .rst_n    (rst_n     ),
						 .rx_data  (tx_data   ),
						 .rx_int   (tx_int    ),
						 .rs232_tx (rs232_tx  ),
						 .clk_bps  (clk_bps   ),
						// .bps_start(bps_start ),
						 .rdy      (rdy       )       
						);

endmodule
