#include <iostream>
#include <string>
#include <csignal>
#include "nvboard.h"
#include "Vtop.h"

// --- Macro definitions
// 为避免编辑器或IDE报错"TOP_NAME未定义"
// Makefile中的-D参数会覆盖掉此处对TOP_NAME的定义
#ifndef TOP_NAME
#define TOP_NAME Vtop
#endif

// --- Global declarations/definitions
// 对于本项目，TOP_NAME宏通过gcc的-D参数设置为Vtop(详见Makefile)
static TOP_NAME dut;
using std::cin;
using std::cout;
using std::endl;
using std::string;

// --- Function declarations
// 此函数的实现由auto_pin_bind.py生成
void nvboard_bind_all_pins(Vtop* top);

// --- Function definitions

#ifdef CLK_DEBUG_MODE
static uint8_t continue_flag;
static uint8_t operate_flag;
void sigroutine(int dunno) { 
    cout << " Detected Ctrl+C, pause" << endl;
    continue_flag = 0;
    operate_flag = 0;
    return;
}
#endif

static void single_cycle() {
#ifdef CLK_DEBUG_MODE  
    static uint64_t current_cycle;
    static uint64_t dst_cycle;

    if((!continue_flag) && (!operate_flag) && (current_cycle >= dst_cycle)) {
        cout << "> ";
        string cmd;
        string param;
        cin >> cmd;
        if(cmd == "c" || cmd == "continue") {
            cin >> param;
            if(std::stoll(param) == -1) {
                continue_flag = true; 
                cout << "Press Ctrl+C to pause again." << endl;
                return;
            }
            dst_cycle = current_cycle + std::stoll(param);
        }
        else if(cmd == "s" || cmd == "step") {
            dst_cycle = current_cycle + 1;
        }
        else if(cmd == "p" || cmd == "posedge") {

        }
        else if(cmd == "n" || cmd == "negedge") {

        }
        else if(cmd == "st" || cmd == "status") {
            cout << "** Current cycle: " << current_cycle << endl;
        }
        else if(cmd == "o" || cmd == "operate") {
            operate_flag = true;
            cout << "** GUI is able to operate now." << endl;
            cout << "Press Ctrl+Z to pause again." << endl;
        }
        else {
            cout << "Invalid command. Try again." << endl;
        }
    }
    if(!operate_flag) {
        dut.clk = 0; dut.eval();
        dut.clk = 1; dut.eval();
        current_cycle ++;
    }
#else
    dut.clk = 0; dut.eval();
    dut.clk = 1; dut.eval();
#endif  
}

static void reset(int n) {
    dut.rst = 1;
    while ((n--) > 0) single_cycle();
    dut.rst = 0;
}

int main() {
#ifdef CLK_DEBUG_MODE
    cout << "** Simulation running in CLK_DEBUG_MODE." << endl;
    cout << "** CLK signal paused, and will be generated according your following commands." << endl;
    cout << "** Usage: [CMD][PARAM]" << endl;
    cout << "** [  CMD  ]        [  PARAM  ]" << endl;
    cout << "** c  continue      cycle num, integer, -1 to continue" << endl;
    cout << "** p  posedge       no param" << endl;
    cout << "** n  negedge       no param" << endl;
    cout << "** s  step          no param" << endl;
    cout << "** st status        no param" << endl;
    cout << "** o  oeprate       no param" << endl;
    // 注册Ctrl+C停止信号
    signal(SIGINT, sigroutine);
#endif

    nvboard_bind_all_pins(&dut);
    nvboard_init();
    reset(10);

    while(1) {
        nvboard_update();
        single_cycle();
    }
}
