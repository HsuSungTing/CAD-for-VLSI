module datapath_v5(
    input wire  [3:0] in_data2,
    input wire  [3:0] in_data1,
    input wire  [3:0] in_coeff,
    input wire        ctrl_trigger,
    output wire [5:0] out_shifted_sum,
    output wire [3:0] out_bitwise_and,
    output wire [3:0] out_bitwise_or,
    output wire       out_valid,
    output wire [3:0] out_coeff_shift2
);
    assign out_shifted_sum  = ctrl_trigger ? (in_coeff >> 1)+(in_data1 << 2) : (in_data2 << 1);
    assign out_bitwise_and  = in_data1 & in_coeff;
    assign out_bitwise_or   = in_coeff | in_data2;
    assign out_valid        = ((in_data1 != 4'b0000)&&ctrl_trigger );
    assign out_coeff_shift2 = in_coeff << 2;
endmodule

module logic_unit_v5(
    input wire        ctrl_b,
    input wire        ctrl_a,
    input wire        ctrl_c,
    input wire  [3:0] in1,
    input wire  [3:0] in2,
    input wire  [3:0] in3,
    input wire  [3:0] in4,
    output wire [3:0] out1,
    output wire [3:0] out3,
    output wire [3:0] out2,
    output wire [3:0] out5,
    output wire [3:0] out4
);
    assign out1 = (ctrl_b && ctrl_a) ? (in1 + in2) : (in4 + in3);
    assign out2 = ctrl_c ? (in2 << 1) : (in3 >> 1);
    assign out3 = (ctrl_b && ~ctrl_c) ? (in1 + in4) : (in2 + in3);
    assign out4 = in1 ^ in2 ^ in3;
    assign out5 = in4 << 2;
endmodule

