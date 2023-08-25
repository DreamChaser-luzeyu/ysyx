module top (
    input  wire[3:0] I_NUM,
    output wire[1:0] O_BIN
);

Encode42 encode42(
    .I_NUM(I_NUM),
    .O_BIN(O_BIN)
);

endmodule

