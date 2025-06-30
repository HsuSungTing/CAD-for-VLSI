module datapath_v5(
    input wire  [3:0] data_a,
    input wire  [3:0] data_b,
    input wire  [3:0] coeff,
    input wire        trigger,
    output wire [5:0] shift_result,
    output wire [3:0] and_result,
    output wire [3:0] or_result,
    output wire       valid_flag,
    output wire [3:0] shifted_twice
);
    assign shift_result   = trigger ? (data_a << 2) + (coeff >> 1) : (data_b << 1);
    assign and_result     = data_a & coeff;
    assign or_result      = data_b | coeff;
    assign valid_flag     = (trigger && (data_a != 4'b0000));
    assign shifted_twice  = coeff << 2;
endmodule

module logic_unit_v5(
    input wire        flag_a,
    input wire        flag_b,
    input wire        flag_c,
    input wire  [3:0] input_1,
    input wire  [3:0] input_2,
    input wire  [3:0] input_3,
    input wire  [3:0] input_4,
    output wire [3:0] output_1,
    output wire [3:0] output_2,
    output wire [3:0] output_3,
    output wire [3:0] output_4,
    output wire [3:0] output_5
);
    assign output_1 = (flag_a && flag_b) ? (input_1 + input_2) : (input_3 + input_4);
    assign output_2 = flag_c ? (input_2 << 1) : (input_3 >> 1);
    assign output_3 = (flag_b && ~flag_c) ? (input_4 + input_1) : (input_2 + input_3);
    assign output_4 = input_1 ^ input_2 ^ input_3;
    assign output_5 = input_4 << 2;
endmodule


