module mcu2pc_uart_top
(
	input  clk,
	input  rstn,
	//rx
	input  rx,
	input  [2:0] rx_baud,
	//tx
	input  [2:0] tx_baud,
	output tx
);

////////////////////////////////////////////////////////
wire [7:0] rx_dat;
wire rx_ok;
uart_rx mcu2pc_rx
(
	.clk    (clk    ),
	//.rstn   (rstn   ),
	.rx     (rx     ),
	.rx_baud(rx_baud),//9600¹Ì¶¨
	.rx_dat (rx_dat ),
	.rx_ok  (rx_ok  )		
);

reg        txen   ;
wire [7:0] tx_dat ;
wire       tx;
wire       rdy    ;
wire       empty  ;
wire       full   ;
wire       rden   ;

//wire wren_w = wren&&(~full);
fifo64x8 mcu2pc_fifo(
	.Data       (rx_dat   ), 
	.WrClock    (clk   ),  
	.RdClock    (clk   ), 
	.WrEn       (rx_ok), 
	.RdEn       (rden  ), 
	.Reset      (~rstn ), 
	.RPReset    (~rstn ), 
	.Q          (tx_dat), 
	.Empty      (empty ), 
	.Full       (full  ),
	.AlmostEmpty(      ), 
	.AlmostFull (      )
);
uart_tx  mcu2pc_tx(
	.clk     (clk     ),
	.rstn   (rstn    ),
	.tx_baud  (tx_baud),
	.tx_int  (tx_int  ),
	.tx_dat (tx_dat  ),
	.tx(tx ),
	.rdy     (rdy     )
);	
					
always@(posedge clk,negedge rstn)begin
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

endmodule
