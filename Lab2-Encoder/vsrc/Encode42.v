module Encode42(
    input  wire[3:0] I_NUM,
    output wire[1:0] O_BIN
);

assign O_BIN[1] = I_NUM[2] | I_NUM[3];
assign O_BIN[0] = I_NUM[3] | (I_NUM[1] & (~I_NUM[2]));

endmodule