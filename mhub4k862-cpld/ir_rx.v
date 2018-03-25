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
//�͵�ƽû�ز����ߵ�ƽ���ز�
reg [9:0] 	low_cnt;
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		low_cnt <=0;
	else if(ir_sd_i==1'b1)//�ɵ��ز��ĸߵ�ƽ
		low_cnt <=0;
	else if(low_cnt <1020)
		low_cnt <=low_cnt +1;
	else 
		low_cnt <=1020;//����1020��ʱ�Ӳɵ��͵�ƽ����Ϊ�е͵�ƽ��
end
wire ir_sd_o=(low_cnt ==1020) ? 0: 1;
*/
///////////////////////////////////////
///////////////////////////////////////
//�ߵ�ƽû�ز����͵�ƽ���ز�
reg [11:0] 	hig_cnt;
always@(posedge clk or negedge rst_n)
begin
	if(!rst_n)
		hig_cnt <=4090;
	else if(ir_sd_i==1'b0)//�ɵ��ز��ĵ͵�ƽ
		hig_cnt <=0;
	else if(hig_cnt <4090)
		hig_cnt <=hig_cnt +1;
	else 
		hig_cnt <=4090;//����1020��ʱ�Ӳɵ��ߵ�ƽ����Ϊ�иߵ�ƽ��
end
wire ir_sd_o=(hig_cnt ==4090) ? 1: 0;
///////////////////////////////////////
endmodule
