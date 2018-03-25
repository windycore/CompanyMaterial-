module ir_rx
(
	clk,//25M	
	rst_n,
	ir_sd_i,
	ir_sd_o
);
input			clk;
input			rst_n;
input 			ir_sd_i;
output			ir_sd_o;
/*
///////////////////////////////////////
//低电平没载波，高电平有载波
reg [9:0] 	low_cnt;
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		low_cnt <=0;
	else if(ir_sd_i==1'b1)//采到载波的高电平
		low_cnt <=0;
	else if(low_cnt <1020)
		low_cnt <=low_cnt +1;
	else 
		low_cnt <=1020;//连续1020个时钟采到低电平，认为有低电平。
end
wire ir_sd_o=(low_cnt ==1020) ? 0: 1;
*/
///////////////////////////////////////
///////////////////////////////////////
//高电平没载波，低电平有载波
reg [11:0] 	hig_cnt;
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		hig_cnt <=4090;
	else if(ir_sd_i==1'b0)//采到载波的低电平
		hig_cnt <=0;
	else if(hig_cnt <4090)
		hig_cnt <=hig_cnt +1;
	else 
		hig_cnt <=4090;//连续1020个时钟采到高电平，认为有高电平。
end
wire ir_sd_o=(hig_cnt ==4090) ? 1: 0;
///////////////////////////////////////
endmodule
