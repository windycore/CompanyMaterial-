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

input       clk      ;	// 50MHz��ʱ��
input       rst_n    ;	//�͵�ƽ��λ�ź�
input [2:0] bpssel   ;   //������ѡ��
input       bps_start;   //���յ����ݺ󣬲�����ʱ�������ź���λ
output      clk_bps  ;	// clk_bps�ĸߵ�ƽΪ���ջ��߷�������λ���м������ 


parameter 		bps2400     = 10415,
				bps4800     = 5207,
				bps9600 	= 2603,	//������Ϊ9600bps
			 	bps19200 	= 1301,	//������Ϊ19200bps
				bps38400 	= 650,	//������Ϊ38400bps
				bps57600 	= 433,	//������Ϊ57600bps
				bps115200	= 216;	//������Ϊ115200bps
/*
parameter 		bps4800_2   = 2603,
				bps9600_2 	= 1301,
				bps19200_2	= 650 ,
				bps38400_2	= 325 ,
				bps57600_2	= 216 ,
				bps115200_2 = 108 ;  
*/				

//���²����ʷ�Ƶ����ֵ�ɲ�������Ĳ������и���
//`define		BPS_PARA		5207	//������Ϊ9600ʱ�ķ�Ƶ����ֵ
//`define 	BPS_PARA_2		2603	//������Ϊ9600ʱ�ķ�Ƶ����ֵ��һ�룬�������ݲ���

reg[13:0] cnt      ;			    //��Ƶ����
reg       clk_bps_r;			    //������ʱ�ӼĴ���

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
		cnt       <= 14'd0;	  //�����ʼ�������
	else
		cnt       <= cnt+1'b1;//������ʱ�Ӽ�������

always @ (posedge clk or negedge rst_n)
	if(!rst_n) clk_bps_r <= 1'b0;
	else if(cnt >= bpcnt)
		clk_bps_r <= 1'b1;
	else 	                  
		clk_bps_r <= 1'b0;

assign clk_bps = clk_bps_r;

endmodule



