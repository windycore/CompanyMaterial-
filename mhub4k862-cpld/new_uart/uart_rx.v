module uart_rx
(
	input clk,
	//input rstn,
	//rx
	input rx,
	input [2:0] rx_baud,
	output [7:0] rx_dat,
	output rx_ok		
);

reg [13:0] rx_baud_count;
always @ (posedge clk)begin
	case(rx_baud)
		3'd0:begin rx_baud_count<=14'd2603; end //9600 default
		3'd1:begin rx_baud_count<=14'd10415;end //2400
		3'd2:begin rx_baud_count<=14'd5207; end //4800
		3'd3:begin rx_baud_count<=14'd2603; end //9600
		3'd4:begin rx_baud_count<=14'd1301; end //19200
		3'd5:begin rx_baud_count<=14'd650;  end //38400
		3'd6:begin rx_baud_count<=14'd433;  end //57600
		3'd7:begin rx_baud_count<=14'd216;  end //115200
	endcase 
end


/////////////////////////////////////////////////////////////////////

reg [3:0] rx_step;
reg [13:0] rx_clk_count;
reg rx_samp_clk;
reg rx_en;
always @ (posedge clk)begin	

	begin		
		if ( rx_clk_count <rx_baud_count )rx_clk_count<=rx_clk_count+14'd1;		
		else rx_clk_count<=14'd0;		
		if(rx_clk_count==rx_baud_count-14'd1)rx_samp_clk<=1'd1;		
		else rx_samp_clk<=1'd0;	
		
		if((!rx_step)&&(!rx))begin //0 步，且收到起始			
			rx_clk_count<=rx_baud_count>>1;//在检测起始位的状态下 收到低电平			
			rx_samp_clk<=1'd0;		
		end	
	end	
end//串口接收模块

reg [2:0] baud_back;
reg [7:0] rx_dat_buf;
reg rx_ok_buf;
assign rx_ok=rx_ok_buf;
assign rx_dat=rx_dat_buf;
always @ (posedge clk)begin	
 begin		
		case (rx_step)			
			4'd0:begin				
				if(!rx)rx_step<=4'd1;					
				rx_ok_buf<=1'd0;							
			end			
			4'd1:begin				
				if(rx_samp_clk)begin
					if(!rx)rx_step<=4'd2;//起始位
					else rx_step<=4'd0;
				end
			end			
			4'd2,4'd3,4'd4,4'd5,4'd6,4'd7,4'd8,4'd9:begin				
				if(rx_samp_clk)begin 					
					rx_dat_buf[rx_step-2]<=rx;						
					rx_step<=rx_step+4'd1;	
					if(rx_step==4'd9)rx_ok_buf<=1'd1; 
				end	
				
			end			
			4'd10:begin	
				rx_ok_buf<=1'd0; 
				if(rx)begin														
					rx_step<=4'd0;				
				end			
			end		
		endcase 

		if(baud_back!=rx_baud)begin
			rx_step<=1'd0;
			rx_dat_buf<=8'h00;
			rx_ok_buf<=1'd0; 
			baud_back<=rx_baud;
		end
	end
end

initial begin
	rx_baud_count =14'd2604;
	rx_clk_count  =14'd0;
	rx_samp_clk   =1'd0;
	rx_step       =4'd0;
	rx_dat_buf    =8'd0;
	rx_ok_buf     =1'd0;
end

endmodule





