module sub_module (
input wire [3:0] input_a,
input wire [3:0] input_b,
input wire control1,
input wire control2,
output wire [3:0] output_y
);

assign output_y=(control1 & control2)?(input_a+ input_b): 
(control1 & ~control2) ? (input_a -input_b) : 
(~control1 & control2) ? (input_a |input_b) : 
(input_a & input_b);

endmodule






