///////////////////////////////////////////////////////////////////////////////
///
// Company:        PTN 
// Engineer:       jerry lee
// 
// Create Date:    13:38:12 11/05/2013 
// Design Name: 
// Module Name:    check_IR_defv 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description:   check the default voltage IR signal from 402 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
///////////////////////////////////////////////////////////////////////////////
module check_IR_defv
(
	input            clk     ,   //25M
	input            rstn    ,
	input            ir_in   ,
	output reg       defv     
);

reg [4:0] H_count;
reg [4:0] L_count;
always@(posedge clk,negedge rstn)
begin
	if(!rstn)begin
		H_count<=5'b0;
		L_count<=5'b0;
	end
    else begin
		if     (ir_in)   begin L_count<=0;H_count<=H_count+5'd1;end
		else             begin H_count<=0;L_count<=L_count+5'd1;end
		
		if     (H_count==5'd25)begin H_count<=4'd0;defv <= 1'b0;end
		else if(L_count==5'd25)begin L_count<=4'd0;defv <= 1'b1;end
		else                         defv <= defv;
    end
end

										
		
//	parameter T1ms = 24999;  //110_0001_1010_1000
//	reg [14:0] cnt1ms  ;
//	reg       pluse1ms;
//	always@(posedge clk,negedge rstn)
//		if(!rstn)begin
//			cnt1ms   <= 15'd0;
//			pluse1ms <= 1'b0 ;
//		end 
//		else begin
//			if(cnt1ms >= T1ms)begin
//				cnt1ms   <= 15'd0;
//				pluse1ms <= 1'b1 ;
//			end
//			else begin
//				cnt1ms   <= cnt1ms + 1'b1;
//				pluse1ms <= 1'b0         ;
//			end
//		end
//	
//	reg [4:0] Hcnt;
//	reg [4:0] Lcnt;
//	always@(posedge clk,negedge rstn)
//		if(!rstn)begin
//			Hcnt <= 5'd0;
//			Lcnt <= 5'd0;
//		end
//	else if(pluse1ms)begin
//		if(ir_in)begin
//			Hcnt <= Hcnt + 1'b1;
//			Lcnt <= 5'd0       ;
//		end
//		else begin
//			Lcnt <= Lcnt + 1'b1;
//			Hcnt <= 5'd0       ;
//		end				
//	end
//	else begin
//		Hcnt <= Hcnt;
//		Lcnt <= Lcnt;	
//	end


//	always@(posedge clk,negedge rstn)
//		if(!rstn)
//			defv <= 1'b0;
//		else begin
//			if(Hcnt>=5'd25)
//				defv <= 1'b0;
//			else if(Lcnt>=5'd25)
//				defv <= 1'b1;	
//			else
//				defv <= defv;		
//		end


endmodule 
