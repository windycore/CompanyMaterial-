module uart_top
(
	input clk,
//	input rstn,
	//rx
	input rx,
	input [2:0] rx_baud,
	output [7:0] rx_dat,
	output rx_ok,
	//tx
	input tx_dat,
	input tx_en,
	input [2:0] tx_baud,
	output tx,
	output tx_ing,
	output tx_ok	
);

reg [13:0] rx_baud_count;
reg [13:0] tx_baud_count;
always @ (posedge clk)begin
	case(rx_baud)
		3'd0:begin rx_baud_count<=14'd2604; end //9600 default
		3'd1:begin rx_baud_count<=14'd10416;end //2400
		3'd2:begin rx_baud_count<=14'd5208; end //4800
		3'd3:begin rx_baud_count<=14'd2604; end //9600
		3'd4:begin rx_baud_count<=14'd1302; end //19200
		3'd5:begin rx_baud_count<=14'd651;  end //38400
		3'd6:begin rx_baud_count<=14'd434;  end //57600
		3'd7:begin rx_baud_count<=14'd217;  end //115200
	endcase 

	case(tx_baud)
		3'd0:begin tx_baud_count<=14'd2604; end //9600 default
		3'd1:begin tx_baud_count<=14'd10416;end //2400
		3'd2:begin tx_baud_count<=14'd5208; end //4800
		3'd3:begin tx_baud_count<=14'd2604; end //9600
		3'd4:begin tx_baud_count<=14'd1302; end //19200
		3'd5:begin tx_baud_count<=14'd651;  end //38400
		3'd6:begin tx_baud_count<=14'd434;  end //57600
		3'd7:begin tx_baud_count<=14'd217;  end //115200
	endcase 
end


reg [13:0] rx_clk_count;
reg rx_samp_clk;

reg [13:0] tx_clk_count;
reg tx_samp_clk;
reg tx_ing_buf;
always @ (posedge clk)begin
	//rx
	if(rx_clk_count<rx_baud_count)begin
		rx_samp_clk <=1'd0;
		if((!rx)&&(!rx_step))begin //ÆğÊ¼Î»
			rx_clk_count<=rx_baud_count>>1;
		end
		else begin
			rx_clk_count<=rx_clk_count+14'd1;	
		end
	end
	else begin
		rx_clk_count<=14'd0;
		rx_samp_clk <=1'd1;
	end

	//tx
	if(tx_clk_count<tx_baud_count)begin
		tx_samp_clk <=1'd0;
		if(!tx_ing_buf)begin
			tx_clk_count<=14'd0;
		end
		else begin
			tx_clk_count<=tx_clk_count+14'd1;
		end
	end
	else begin
		tx_clk_count<=14'd0;
		tx_samp_clk <=1'd1;
	end
end

reg [3:0] rx_step;
reg [7:0] rx_dat_buf;
reg rx_ok_buf;
always @ (posedge clk)begin
	case (rx_step)	
		4'd0:begin
			rx_ok_buf<=1'd0;
			if(!rx)rx_step<=4'd1;		
		end
		4'd1:begin
			if(rx_samp_clk&&(!rx))rx_step<=4'd2;
		end
		4'd2,4'd3,4'd4,4'd5,4'd6,4'd7,4'd8,4'd9:begin
			if(rx_samp_clk)begin
				rx_dat_buf[rx_step-2]<=rx;
				rx_step<=rx_step+4'd1;
			end			
		end
		3'd10:begin
			if(rx_samp_clk&&rx)begin
				rx_ok_buf<=1'd1;
				rx_step<=4'd0;
			end	
		end
		default:begin 
			rx_step<=4'd0;
			rx_ok_buf<=1'd0;
			rx_dat_buf<=8'd0;
		end	
	endcase 
end
assign rx_ok  = rx_ok_buf;
assign rx_dat = rx_ok_buf?rx_dat_buf:rx_dat;

reg [3:0] tx_step;
reg tx_ok_buf;
reg tx_buf;
always @ (posedge clk)begin
	case (tx_step)	
		4'd0:begin
			tx_ok_buf<=1'd0;
			tx_ing_buf<=1'd0;
			if(tx_en)begin
				tx_ing_buf<=1'd1;
				tx_buf<=1'd0;
				tx_step<=4'd1;	
			end
		end
		4'd1,4'd2,4'd3,4'd4,4'd5,4'd6,4'd7,4'd8:begin
			if(tx_samp_clk)begin
				tx_buf <=tx_dat[tx_step-1];
				tx_step<=tx_step+4'd1;
			end			
		end
		3'd10:begin
			if(tx_samp_clk)begin
				tx_buf <=1'd1;
				tx_step<=4'd0;
			end	
		end
		default:begin 
			tx_step  <=4'd0;
			tx_ok_buf<=1'd0;
			tx_buf   <=1'd1;
			tx_ing_buf <=1'd0;
		end	
	endcase 
end
assign tx_ok = tx_ok_buf;
assign tx    = tx_buf;
assign tx_ing=tx_ing_buf;

initial begin
	//rx
	rx_baud_count =14'd2604;
	rx_clk_count  =14'd0;
	rx_samp_clk   =1'd0;
	rx_step       =4'd0;
	rx_dat_buf    =8'd0;
	rx_ok_buf     =1'd0;
	//tx
	tx_baud_count =14'd2604;
	tx_clk_count  =14'd0;
	tx_samp_clk   =1'd0;
	tx_step       =4'd0;
	tx_buf        =1'd1;
	tx_ing_buf      =1'd0;
	tx_ok_buf     =1'd0;
end

endmodule




























