# 示例工程

## Makefile Target说明
- make / make all
  - 使用Verilator生成仿真源代码，并编译出二进制可执行文件，但不执行
  - 需要在修改顶层模块的Verilog代码后执行
- make run
  - 使用Verilator生成仿真源代码，并编译出二进制可执行文件，并执行
- make run_debug
  - 使用Verilator生成仿真源代码，并编译出二进制可执行文件，并执行
  - 会启用`CLK_DEBUG_MODE`宏包围的代码，启用后支持单周期调试
- make run_debug_external_terminal
  - 使用Verilator生成仿真源代码，并编译出二进制可执行文件，并在外部终端中执行
  - 外部终端默认使用Konsole,如果系统终端不是Konsole，需要手动修改Makefile，以修改所使用的终端
  - 会启用`CLK_DEBUG_MODE`宏包围的代码，启用后支持单周期调试
- make clean
  - 删除所有生成的代码与二进制文件
  
## 示例功能
1. 左边8个LED为流水灯效果
2. 拨动右边的8个拨码开关, 可控制对应LED的亮灭
3. 8个数码管流水显示数字0-7
4. RST按钮绑定到rst信号，其他按钮暂无展示效果
