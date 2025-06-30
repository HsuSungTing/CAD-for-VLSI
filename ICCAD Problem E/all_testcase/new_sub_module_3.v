module datapath_unit_v2(
    input wire  [3:0] scale_factor,
    input wire  [3:0] input_b,
    input wire        mode_select,
    input wire  [3:0] input_a,
    output wire [5:0] scaled_result,
    output wire [3:0] xor_result,
    output wire       threshold_flag
);
    assign scaled_result = mode_select ? (2 * scale_factor*input_a ) : (input_b * scale_factor);
    assign xor_result = input_b ^ scale_factor ^ input_a;
    assign threshold_flag = (input_b >= 3);
endmodule

module control_logic_unit_v2(
    input wire  [3:0] op4,
    input wire  [3:0] op2,
    input wire  [3:0] op1,
    input wire  [3:0] op3,
    input wire        ctrl_c,
    input wire        ctrl_a,
    input wire        ctrl_b,
    output wire [3:0] out2,
    output wire [3:0] out3,
    output wire [3:0] out1
);
    assign out1 = (~ctrl_b && ctrl_a) ? (op2 + op1) : (op4 + op3);
    assign out2 = ((ctrl_a && ctrl_b) || ~ctrl_c) ? (op2 + op3) : (op4 + op1);  
    assign out3 = (ctrl_c ^ ctrl_a) ? (op1 + op3) : (op2 ^ op4);
endmodule

