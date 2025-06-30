module sub_module1(
    input wire  [3:0] in1,
    input wire  [3:0] in2,
    input wire        cin,
    output wire [4:0] sum,
    output wire [4:0] sum_2
);
    assign sum = (in2&&~in1)+cin;
    assign sum_2 = 2*in1 + cin + in2;

endmodule

module sub_module2(
    input wire  [3:0] in1,
    input wire  [3:0] in2,
    input wire  [3:0] in3,
    output wire [3:0] out1,
    output wire [3:0] out2
);

    assign out1 = 3*in3*in1 +in2;
    assign out2 = (in2 > 2) ? (in3 + in1) : (in1 ^ in3);

endmodule

module sub_module3(
    input wire  [3:0] in1,
    input wire  [3:0] in2,
    input wire  [3:0] in3,
    input wire        x1,
    input wire        x2,
    input wire        x3,
    output wire [3:0] out1,
    output wire [3:0] out2
);

    assign out1 = ((x3&&~x2 )||x1) ? (in2 + in1) : (in3 + in2);
    assign out2 = (~x3 && x2 || (~x2 && x1 && x3)) ? (in2 + in3) : (in2 + in1);

endmodule
