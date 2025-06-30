module top_module (
    input wire  [3:0] in1_sub_module1,
    input wire  [3:0] in2_sub_module1,
    input wire        cin_sub_module1,
    output wire [4:0] sum_sub_module1,
    input wire  [3:0] in1_sub_module2,
    input wire  [3:0] in2_sub_module2,
    input wire  [3:0] in3_sub_module2,
    output wire [3:0] out1_sub_module2,
    output wire [3:0] out2_sub_module2,
    input wire  [3:0] in1_sub_module3,
    input wire  [3:0] in2_sub_module3,
    input wire  [3:0] in3_sub_module3,
    input wire        x1_sub_module3,
    input wire        x2_sub_module3,
    input wire        x3_sub_module3,
    output wire [3:0] out1_sub_module3,
    output wire [3:0] out2_sub_module3
);
    sub_module1 U1 (
        .in1(in1_sub_module1),
        .in2(in2_sub_module1),
        .cin(cin_sub_module1),
        .sum(sum_sub_module1)
    );

    sub_module2 U2 (
        .in1(in1_sub_module2),
        .in2(in2_sub_module2),
        .in3(in3_sub_module2),
        .out1(out1_sub_module2),
        .out2(out2_sub_module2)
    );

    sub_module3 U3 (
        .in1(in1_sub_module3),
        .in2(in2_sub_module3),
        .in3(in3_sub_module3),
        .x1(x1_sub_module3),
        .x2(x2_sub_module3),
        .x3(x3_sub_module3),
        .out1(out1_sub_module3),
        .out2(out2_sub_module3)
    );

endmodule
