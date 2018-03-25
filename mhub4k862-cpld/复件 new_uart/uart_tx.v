module uart_tx
(
	input  clk,
//	input  rstn,
	//tx
	input  [7:0] tx_dat,
	input  tx_en,
	input  [2:0] tx_baud,
	output tx,
	output tx_ing,//ÕıÔÚ·¢ËÍ
	output tx_ok	
);

reg [13:0] tx_baud_count;
always @ (posedge clk)begin
	case(tx_baud)
		3'd0:begin tx_baud_count<=14'd2603; end //9600 default
		3'd1:begin tx_baud_count<=14'd10415;end //2400
		3'd2:begin tx_baud_count<=14'd5207; end //4800
		3'd3:begin tx_baud_count<=14'd2603; end //9600
		3'd4:begin tx_baud_count<=14'd1301; end //19200
		3'd5:begin tx_baud_count<=14'd650;  end //38400
		3'd6:begin tx_baud_count<=14'd433;  end //57600
		3'd7:begin tx_baud_count<=14'd216;  end //115200
	endcase 
end


reg tx_ing_buf;
reg [13:0] tx_clk_count;
reg tx_samp_clk;
always @ (posedge clk)begin
	//tx
	if(tx_clk_count<tx_baud_count)begin
		//tx_samp_clk <=1'd0;
		if(!tx_ing_buf)begin
			tx_clk_count<=14'd0;
		end
		else begin
			tx_clk_count<=tx_clk_count+14'd1;
		end
	end
	else begin
		tx_clk_count<=14'd0;
		//tx_samp_clk <=1'd1;
	end

	if(tx_clk_count==tx_baud_count-1)tx_samp_clk <=1'd1;
	else tx_samp_clk <=1'd0;
end

reg [3:0] tx_step;
reg tx_ok_buf;
reg tx_buf;
always @ (posedge clk)begin
	case (tx_step)	
		4'd0:begin		
			tx_ok_buf<=1'd0;
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
		4'd9:begin
			if(tx_samp_clk)begin
				tx_ok_buf<=1'd1;
				tx_ing_buf <=1'd0;
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
	tx_baud_count =14'd2604;
	tx_clk_count  =14'd0;
	tx_samp_clk   =1'd0;
	tx_step       =4'd0;
	tx_buf        =1'd1;
	tx_ing_buf      =1'd0;
	tx_ok_buf     =1'd0;
end

endmodule