module datapath_mix(
    input  wire        enable,
    input  wire  [3:0] src_b,
    input  wire  [2:0] sel_ctrl,
    input  wire  [3:0] src_a,
    input  wire  [3:0] gain,
    output wire        flag_ok,
    output wire [3:0]  xor_out,
    output wire [5:0]  combo_shift,
    output wire [3:0]  logic_and,
    output wire [3:0]  logic_or,
    output wire [3:0]  shift_2x,
    output wire [3:0]  final_mix,
    output wire        range_flag
);
    assign combo_shift = enable ? (src_a << sel_ctrl) + (gain >> 1) : (src_b << sel_ctrl);
    assign logic_and   = src_a & gain;
    assign logic_or    = src_b | gain;
    assign flag_ok     = (enable && (src_a != 4'b0000));
    assign shift_2x    = gain << 2;
    assign xor_out     = src_a ^ src_b ^ gain;
    assign final_mix   = (sel_ctrl ? src_a : src_b) + (sel_ctrl ? gain : 4'b0011);
    assign range_flag  = ((src_a + gain) > 4'b1111);
endmodule

module logic_unit_mix(
    input  wire        cond_3,
    input  wire        cond_2,
    input  wire        cond_4,
    input  wire        cond_1,
    input  wire  [3:0] sig_a,
    input  wire  [3:0] sig_d,
    input  wire  [3:0] sig_c,
    input  wire  [3:0] sig_e,
    input  wire  [3:0] sig_b,
    output wire [3:0]  out_x,
    output wire [3:0]  out_y,
    output wire [3:0]  out_m,
    output wire [3:0]  out_p,
    output wire [3:0]  out_q,
    output wire [3:0]  out_z,
    output wire [3:0]  out_r,
    output wire        eq_flag
);
    assign out_x   = (cond_1 && cond_2) ? (sig_a + sig_b) : (sig_c + sig_d);
    assign out_y   = cond_3 ? (sig_b << 1) : (sig_c >> 1);
    assign out_m   = (cond_2 && ~cond_3) ? (sig_d + sig_a) : (sig_b + sig_c);
    assign out_p   = sig_b ^ sig_c ^ sig_a;
    assign out_q   = sig_d << 2;
    assign out_z   = (cond_4) ? (sig_e + sig_a) : (sig_b - sig_e);
    assign out_r   = (sig_e & sig_c) | (sig_d ^ sig_a ^ sig_e ^ sig_c);
    assign eq_flag = (sig_a == sig_e) && (sig_c != sig_b);
endmodule
