#include <stdio.h>
#include <stddef.h>

#include "verilated_vcd_c.h" //可选，如果要导出vcd则需要加上
#include "VTop.h"

vluint64_t Sim_Time_GV = 0;
VerilatedVcdC tfp;
/**
 * Must implement, used by verilator 
 * Return current simulation time
 */
double sc_time_stamp() {
    return Sim_Time_GV;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    // --- 用于导出vcd波形
    Verilated::traceEverOn(true);
    // VerilatedVcdC *tfp = new VerilatedVcdC;
    // ---
    // 构造顶模块，此处的“top”将作为更上一层的模块的名称
    VTop *top = new VTop("top"); // 调用VTop.h里面的IO struct
    // --- 开始跟踪波形
    top->trace(&tfp, 0);
    tfp.open("wave.vcd"); // 打开vcd
    // ---
    // 设置仿真时间段
    uint8_t bit_num = 0;
    while (Sim_Time_GV < 8 && !Verilated::gotFinish()) {
        // --- Write testbench here
        top->IO_Sel = !!(bit_num & 0x04);
        top->IO_Signal_A = !!(bit_num & 0x02);
        top->IO_Signal_B = !!(bit_num & 0x01);
        
        bit_num ++;
        // ---
        // 计算模型，当输入改变时应调用
        top->eval(); 
        // --- Post-eval operations
        // 输出仿真结果
        printf("[TB SIM RESULT] Sel: %d, SigA: %d, SigB: %d, Result: %d\n", 
               top->IO_Sel, 
               top->IO_Signal_A, 
               top->IO_Signal_B,  
               top->IO_Result); 
        // 存储波形文件
        tfp.dump(Sim_Time_GV);
        Sim_Time_GV++;
    }

    top->final();
    tfp.flush();
    tfp.close();
    delete top;
    return 0;
}