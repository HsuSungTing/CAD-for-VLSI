module new_sub_module (
input wire [3:0] data_x,
input wire [3:0] data_y,
input wire control1,
input wire flag_x,
output wire [3:0] result
);


assign result=(control1 & flag_x)?(data_x + data_y):
(control1 & ~flag_x)?(data_x - data_y):
(~control1 & flag_x)?(data_x | data_y):
(data_x & data_y);

endmodule

