module Top(
    input  wire IO_Sel,
    input  wire IO_Signal_A,
    input  wire IO_Signal_B,
    output wire IO_Result
);

assign IO_Result = (~IO_Sel & IO_Signal_A) | (IO_Sel & IO_Signal_B);

endmodule