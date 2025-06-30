module datapath_unit(
    input wire  [3:0] data1,
    input wire  [3:0] data2,
    input wire  [3:0] factor,
    input wire        select,
    output wire [5:0] final_mult,
    output wire [3:0] logic_mix,
    output wire       flag
);
    assign final_mult = (select ? (factor * data1 * 2) : (factor * data2));
    assign logic_mix = data1 ^ data2 ^ factor;
    assign flag = (data2 >= 3);
endmodule

module control_logic_unit(
    input wire        cond_a,
    input wire        cond_b,
    input wire        cond_c,
    input wire  [3:0] val1,
    input wire  [3:0] val2,
    input wire  [3:0] val3,
    input wire  [3:0] val4,
    output wire [3:0] res1,
    output wire [3:0] res2,
    output wire [3:0] res3
);
    assign res1 = (cond_a && ~cond_b) ? (val1 + val2) : (val3 + val4);
    assign res2 = (~cond_c || (cond_b && cond_a)) ? (val2 + val3) : (val1 + val4);
    assign res3 = (cond_a ^ cond_c) ? (val1 + val3) : (val2 ^ val4);
endmodule


