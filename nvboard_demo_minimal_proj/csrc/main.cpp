#include <iostream>
#include <string>
#include <csignal>
#include <verilated_vcd_c.h> 
#include "nvboard.h"
#include "Vtop.h"

// --- Macro definitions
// 为避免编辑器或IDE报错"TOP_NAME未定义"
// Makefile中的-D参数会覆盖掉此处对TOP_NAME的定义
#ifndef TOP_NAME
#define TOP_NAME Vtop
#endif

// --- Global declarations/definitions
uint64_t CurrentCycle_GV;
// 对于本项目，TOP_NAME宏通过gcc的-D参数设置为Vtop(详见Makefile)
static TOP_NAME dut;
static VerilatedVcdC tfp;
static uint64_t EvalCounter_GV;
static pthread_mutex_t PauseFlagLock = PTHREAD_MUTEX_INITIALIZER;

using std::cin;
using std::cout;
using std::endl;
using std::string;


// --- Function declarations
// 此函数的实现由auto_pin_bind.py生成
void nvboard_bind_all_pins(Vtop* top);

// --- Function definitions

#ifdef CLK_DEBUG_MODE
// 标识是否一直运行
// true  - 忽略dst_cycle，一直运行直到Ctrl+C
// false - 运行到dst_cycle就停止
static uint8_t continue_flag;
// 标识用户是否需要操作
// true  - 用户需要操作，此时不进入命令行，时钟停止
// false - 用户不需要操作
static uint8_t operate_flag;
void sigroutine(int dunno) { 
    cout << " Detected Ctrl+C, pause" << endl;
    pthread_mutex_lock(&PauseFlagLock);
    continue_flag = 0;
    operate_flag = 0;
    pthread_mutex_unlock(&PauseFlagLock);
    return;
}
#endif

static uint64_t dst_cycle;
static void single_cycle() {
#ifdef CLK_DEBUG_MODE
    // 提示：未增加dst_cycle的命令执行完成后应return，否则仍会执行一个周期，与预期行为不符
    // 在不持续运行，且用户不需要操作界面的时候，检查是否执行到目标时钟数
    pthread_mutex_lock(&PauseFlagLock);
    uint8_t pause_flag = (!continue_flag) && (!operate_flag);
    pthread_mutex_unlock(&PauseFlagLock);
    if(pause_flag && (CurrentCycle_GV >= dst_cycle)) {
        cout << "> ";
        string cmd;
        string param;
        cin >> cmd;
        if(cmd == "c" || cmd == "continue") {
            cin >> param;
            if(std::stoll(param) == -1) {
                pthread_mutex_lock(&PauseFlagLock);
                continue_flag = true;
                pthread_mutex_unlock(&PauseFlagLock);
#ifdef DUMP_WAVE
                cout << "Caution! May generate VERY LARGE wave file." << endl;
#endif
                cout << "Press Ctrl+C to pause again." << endl;
                return;
            }
            dst_cycle = CurrentCycle_GV + std::stoll(param);
        }
        else if(cmd == "s" || cmd == "step") {
            dst_cycle = CurrentCycle_GV + 1;
        }
        else if(cmd == "p" || cmd == "posedge") {

        }
        else if(cmd == "n" || cmd == "negedge") {

        }
        else if(cmd == "st" || cmd == "status") {
            cout << "** Current cycle: " << CurrentCycle_GV << endl;
            return;
        }
        else if(cmd == "o" || cmd == "operate") {
            pthread_mutex_lock(&PauseFlagLock);
            operate_flag = true;
            pthread_mutex_unlock(&PauseFlagLock);
            cout << "** GUI is able to operate now." << endl;
            cout << "Press Ctrl+C to pause again." << endl;
            return;
        }
        else if(cmd == "dump") {
#ifdef DUMP_WAVE
            tfp.flush();
            cout << "success" << endl;
#else
            cout << "DUMP_MODE disabled, define DUMP_MODE macro to dump wave" << endl;
#endif
            return;
        }
        else {
            cout << "Invalid command. Try again." << endl;
        }
    }
    if(!operate_flag) {
        dut.clk = 0; dut.eval();
#ifdef DUMP_WAVE
        tfp.dump(EvalCounter_GV); EvalCounter_GV++;
#endif
        dut.clk = 1; dut.eval();
#ifdef DUMP_WAVE
        tfp.dump(EvalCounter_GV); EvalCounter_GV++;
#endif
        CurrentCycle_GV ++;
    }
#else
    dut.clk = 0; dut.eval();
#ifdef DUMP_WAVE
    tfp.dump(EvalCounter_GV); EvalCounter_GV++;
#endif
    dut.clk = 1; dut.eval();
#ifdef DUMP_WAVE
    tfp.dump(EvalCounter_GV); EvalCounter_GV++;
#endif
    CurrentCycle_GV ++;
#endif // CLK_DEBUG_MODE
}

static void reset(int n) {
    dut.rst = 1;
    while ((n--) > 0) {
        dut.clk = 0; dut.eval();
#ifdef DUMP_WAVE
        tfp.dump(EvalCounter_GV); EvalCounter_GV++;
#endif
        dut.clk = 1; dut.eval();
#ifdef DUMP_WAVE
        tfp.dump(EvalCounter_GV); EvalCounter_GV++;
#endif
        CurrentCycle_GV ++;
    }
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
    cout << "** dump             no param, dump wave" << endl;
    // 注册Ctrl+C停止信号
    signal(SIGINT, sigroutine);
#endif

#ifdef DUMP_WAVE
    // --- 启用导出vcd波形
    Verilated::traceEverOn(true);
    // --- 开始跟踪波形
    // 注意这里必须先调用trace()，然后再调用open()，否则崩溃
    dut.trace(&tfp, 0);
    tfp.open("wave.vcd");
    cout << "** Wave tracking enabled, disable wave tracking to get better performance" << endl;
#endif

    nvboard_bind_all_pins(&dut);
    nvboard_init();
    reset(10);

#ifdef CLK_DEBUG_MODE
    // 首次暂停时的周期序数
    dst_cycle = 11;
#endif

    while(1) {
        nvboard_update();
        single_cycle();
    }
}
