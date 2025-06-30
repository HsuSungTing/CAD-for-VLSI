module new_sub_module1(
    input wire        new_in1,
    input wire  [3:0] new_in2,
    input wire  [3:0] new_in3,
    output wire [4:0] new_out1,
    output wire [4:0] new_out2
);
    assign new_out1 = 2*new_in2 +new_in3 +new_in1;
    assign new_out2 = new_in3+(new_in2 && ~new_in1);

endmodule

module new_sub_module2(
    input wire  [3:0] new_in1,
    input wire  [3:0] new_in2,
    input wire  [3:0] new_in3,
    output wire [3:0] new_out1,
    output wire [3:0] new_out2
);

    assign new_out2 = new_in2 + 3*new_in3*new_in1;
    assign new_out1 = (new_in1 > 2) ? (new_in3+new_in1): (new_in3 ^ new_in2);

endmodule

module new_sub_module3(
    input wire        new_in3,
    input wire        new_in5,
    input wire        new_in6,
    input wire  [3:0] new_in1,
    input wire  [3:0] new_in2,
    input wire  [3:0] new_in4,
    output wire [3:0] new_out1,
    output wire [3:0] new_out2
);

    assign new_out2 = (new_in5 || (~new_in6 && new_in3)) ? (new_in4 + new_in2) : (new_in2 + new_in1);
    assign new_out1 = (new_in6 && ~new_in3 || (new_in5 && ~new_in6 && new_in3)) ? (new_in2 + new_in1) : (new_in4 + new_in2);

endmodule
