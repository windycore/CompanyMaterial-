module hc74595 (
									srclrn ,
									rclk   ,
									srclk  ,
									ser    ,
									qa     ,
									qb     , 
									qc     ,
									qd     ,
									qe     ,
									qf     ,
									qg     ,
									qh     ,
									qhn
								);

input  	   srclrn ;
input	     rclk   ;
input	     srclk  ;
input	     ser    ;
output reg qa     ;
output reg qb     ;
output reg qc     ;
output reg qd     ;
output reg qe     ;
output reg qf     ;
output reg qg     ;
output reg qh     ;
output     qhn    ;

reg qa0  ;
reg qb0  ;
reg qc0  ;
reg qd0  ;
reg qe0  ;
reg qf0  ;
reg qg0  ;
reg qh0  ;
reg qhn0 ;

always@(posedge srclk,negedge srclrn)
		if(!srclrn)begin
			qa0  <= 1'b0;
			qb0  <= 1'b0;
			qc0  <= 1'b0;
			qd0  <= 1'b0;
			qe0  <= 1'b0;
			qf0  <= 1'b0;
			qg0  <= 1'b0;
			qh0  <= 1'b0;
			qhn0 <= 1'b0;
		end
		else begin
			qa0  <= ser ;
			qb0  <= qa0 ;
			qc0  <= qb0 ;
			qd0  <= qc0 ;
			qe0  <= qd0 ;
			qf0  <= qe0 ;
			qg0  <= qf0 ;
			qh0  <= qg0 ;
			qhn0 <= qh0 ;
		end

	always@(posedge rclk)
	begin
		qa  <= qa0 ;
		qb  <= qb0 ;
		qc  <= qc0 ;
		qd  <= qd0 ;
		qe  <= qe0 ;
		qf  <= qf0 ;
		qg  <= qg0 ;
		qh  <= qh0 ;
	end

	assign qhn = qhn0 ;

endmodule
