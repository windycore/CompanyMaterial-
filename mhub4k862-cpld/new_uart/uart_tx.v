//module uart_tx
//(
//	input  clk,
//	//input  rstn,
//	//tx
//	input  [7:0] tx_dat,
//	input  tx_en,
//	input  [2:0] tx_baud,
//	output tx,
//	output tx_ing,//正在发送
//	output tx_ok	
//);

//reg tx_en_reg0;
//reg tx_en_reg1;
//always @ (posedge clk)begin
//	tx_en_reg0<=tx_en;
//	tx_en_reg1<=tx_en_reg0;
//end
//wire tx_en_psg=tx_en_reg0&&(~tx_en_reg1);

//reg [13:0] tx_baud_count;
//always @ (posedge clk)begin
//	case(tx_baud)
//		3'd0:begin tx_baud_count<=14'd2603; end //9600 default
//		3'd1:begin tx_baud_count<=14'd10415;end //2400
//		3'd2:begin tx_baud_count<=14'd5207; end //4800
//		3'd3:begin tx_baud_count<=14'd2603; end //9600
//		3'd4:begin tx_baud_count<=14'd1301; end //19200
//		3'd5:begin tx_baud_count<=14'd650;  end //38400
//		3'd6:begin tx_baud_count<=14'd433;  end //57600
//		3'd7:begin tx_baud_count<=14'd216;  end //115200
//	endcase 
//end


//reg tx_ing_buf;
//reg [13:0] tx_clk_count;
//reg tx_samp_clk;
//always @ (posedge clk)begin
//	//tx
//	if(tx_clk_count<tx_baud_count)begin
//		if(!tx_ing_buf)begin//空闲时
//			tx_clk_count<=14'd0;
//		end
//		else begin
//			tx_clk_count<=tx_clk_count+14'd1;
//		end
//	end
//	else begin
//		tx_clk_count<=14'd0;
//	end

//	if(tx_clk_count==tx_baud_count-1)tx_samp_clk <=1'd1;
//	else tx_samp_clk <=1'd0;
//end

//reg [3:0] tx_step;
//reg tx_ok_buf;
//reg tx_buf;
//always @ (posedge clk)begin		
//	case (tx_step)	
//		4'd0:begin		
//			tx_ok_buf<=1'd0;
//		end
//		4'd1,4'd2,4'd3,4'd4,4'd5,4'd6,4'd7,4'd8:begin
//			if(tx_samp_clk)begin
//				tx_buf <=tx_dat[tx_step-1];
//				tx_step<=tx_step+4'd1;
//			end	
//		end
//		4'd9:begin
//			if(tx_samp_clk)begin
//				tx_buf <=1'd1;				
//				tx_step<=4'd10;
//			end	
//		end
//		4'd10:begin
//			if(tx_samp_clk)begin
//				tx_ok_buf<=1'd1;
//				tx_ing_buf <=1'd0;			
//				tx_step<=4'd0;
//			end	
//		end
//		default:begin 
//			tx_step  <=4'd0;
//			tx_ok_buf<=1'd0;
//			tx_buf   <=1'd1;
//			tx_ing_buf <=1'd0;
//		end	
//	endcase 

//	if(tx_en_psg)begin //不论在何种状态下，收到开始发，重头开始
//		tx_ing_buf<=1'd1;
//		tx_buf<=1'd0;
//		tx_step<=4'd1;	
//	end
//end
//assign tx_ok = tx_ok_buf;
//assign tx    = tx_buf;
//assign tx_ing=tx_ing_buf;

//initial begin
//	tx_baud_count =14'd2604;
//	tx_clk_count  =14'd0;
//	tx_samp_clk   =1'd0;
//	tx_step       =4'd0;
//	tx_buf        =1'd1;
//	tx_ing_buf      =1'd0;
//	tx_ok_buf     =1'd0;
//	tx_en_reg0=1'd0;
//	tx_en_reg1=1'd0;
//end

//endmodule
// ********************************************************************************

module uart_tx
(
	input clk           ,// 25MHz主时钟
	input rstn          ,//低电平复位信号
	input [7:0] tx_dat  ,//待发送数据
	input [2:0] tx_baud ,
	input tx_int        ,
	output tx           ,
	output rdy
 );
 				
reg [13:0] rx_baud_count;
always@(posedge clk)
begin
	case(tx_baud)
		3'd0:rx_baud_count <= 14'd2603 ;//9600		
		3'd1:rx_baud_count <= 14'd10415;//2400	
		3'd2:rx_baud_count <= 14'd5207 ;//4800	
		3'd3:rx_baud_count <= 14'd2603 ;//9600	
		3'd4:rx_baud_count <= 14'd1301 ;//19200	
		3'd5:rx_baud_count <= 14'd650  ;//38400	
		3'd6:rx_baud_count <= 14'd433  ;//57600	
		3'd7:rx_baud_count <= 14'd216  ;//115200	
	endcase
end

reg [13:0] rx_clk_count;			    //分频计数
always @ (posedge clk)begin
	if(rx_clk_count >= rx_baud_count)rx_clk_count<=14'd0;	  //波特率计数清零
	else                             rx_clk_count<=rx_clk_count+1'b1;//波特率时钟计数启动
end
		
reg       tx_samp_clk;			    //波特率时钟寄存器
always @ (posedge clk)begin
	if(rx_clk_count>=rx_baud_count)tx_samp_clk <= 1'b1;
	else                           tx_samp_clk <= 1'b0;
end

reg      tx_en      ;  //发送数据使能信号，高有效
reg[7:0] tx_data    ;	//待发送数据的寄存器
always @ (posedge clk) begin
	if(tx_int) begin	//接收数据完毕，准备把接收到的数据发回去
		tx_data <= tx_dat;	    //把接收到的数据存入发送数据寄存器
		tx_en <= 1'b1;		    //进入发送数据状态中
	end
	else if(tx_step==4'd11) begin	//数据发送完成，复位
		tx_en <= 1'b0;
	end
end

reg tx_buf;
reg [3:0] tx_step        ;
always @ (posedge clk) begin
	if(tx_en) begin
		if(tx_samp_clk)	begin
			tx_step <= tx_step+1'b1;
			case (tx_step)
				4'd0: tx_buf <= 1'b0; 	    //发送起始位
				4'd1: tx_buf <= tx_data[0];	//发送bit0
				4'd2: tx_buf <= tx_data[1];	//发送bit1
				4'd3: tx_buf <= tx_data[2];	//发送bit2
				4'd4: tx_buf <= tx_data[3];	//发送bit3
				4'd5: tx_buf <= tx_data[4];	//发送bit4
				4'd6: tx_buf <= tx_data[5];	//发送bit5
				4'd7: tx_buf <= tx_data[6];	//发送bit6
				4'd8: tx_buf <= tx_data[7];	//发送bit7
				4'd9: tx_buf <= 1'b1;	    //发送结束位
				default: tx_buf <= 1'b1;
			endcase 
		end
		else if(tx_step==4'd11) tx_step <= 4'd0;	   //复位
	end
end
assign tx = tx_buf;
assign rdy = tx_en;

initial begin
	rx_baud_count=14'd2603;
	rx_clk_count =14'd0;   //分频计数
	tx_samp_clk  =1'd0 ;   //波特率时钟寄存器
	tx_en        =1'd0 ;   //发送数据使能信号，高有效
	tx_data      =8'd0 ;   //待发送数据的寄存器
	tx_buf       =1'd1 ;
	tx_step      =4'd0 ;
end

endmodule
