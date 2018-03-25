module mcu2pc_uart_top
(
	input  clk,
	input  rstn,
	//rx
	input  rx,
	input  [2:0] rx_baud,
	//tx
	input  [2:0] tx_baud,
	output rx_ok,
	output tx
);

wire [7:0] rx_dat;
//wire rx_ok;
uart_rx mcu2pc_rx
(
	.clk    (clk    ),
	//.rstn   (rstn   ),
	.rx     (rx     ),
	.rx_baud(3'd0),
	.rx_dat (rx_dat ),
	.rx_ok  (rx_ok  )		
);

wire [7:0] tx_dat;
wire tx_ing;
uart_tx mcu2pc_tx
(
	.clk    (clk    ),
//	.rstn   (rstn   ),
	.tx_dat (rx_dat),//	.tx_dat (tx_dat ),
	.tx_en  (rx_ok   ),//��fifo ׼����ʼ��//	.tx_en  (rden   ),//��fifo ׼����ʼ��
	.tx_baud(3'd0),
	.tx     (tx     ),
	.tx_ing (tx_ing ),
	.tx_ok  (       )	
);

wire empty;
wire rden=(!empty)&&(!tx_ing);//�ǿ��Ҳ��ڷ��ͣ���������ͬʱ�������ڷ���
fifo64x8 mcu2pc_fifo
(
	.Data       (rx_dat   ), 
	.WrClock    (clk   ),  
	.RdClock    (clk   ), 
	.WrEn       (rx_ok ), 
	.RdEn       (rden  ), 
	.Reset      (~rstn ), 
	.RPReset    (~rstn ), 
	.Q          (tx_dat), 
	.Empty      (empty ), 
	.Full       (      ),
	.AlmostEmpty(      ), 
	.AlmostFull (      )
);

endmodule
