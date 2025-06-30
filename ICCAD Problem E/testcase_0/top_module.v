module top_module (
input wire [3:0] a,
input wire [3:0] b,
input wire sel1,
input wire sel2,
output wire [3:0] y
);

sub_module U1 (
.input_a(a),
.input_b(b),
.control1(sel1),
.control2(sel2),
.output_y(y)
);
//內部訊號宣告(不須修改)
wire [3:0]input_a; 
wire [3:0]input_b;
wire sel1; 
wire sel2;
wire [3:0]output_y;

//其他combinational logic(不須修改)
assign y=output_y;
endmodule
