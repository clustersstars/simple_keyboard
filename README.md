# simple-keyboard
DIY 双MCU机械键盘固件

## 项目简介
双STM32分离架构：
- **nkro_keyboard**：主控MCU，负责矩阵按键扫描、USB NKRO HID键盘上报
- **nkro_ws2812b**：灯光从机MCU，专门驱动WS2812 RGB灯带；通过UART串口接收主控下发的灯光指令

灯光逻辑和键盘核心逻辑物理隔离，修改灯效不会影响USB键盘功能，方便调试与二次开发。

## 开发环境
- 构建系统：CMake + Ninja
- 交叉编译器：gcc-arm-none-eabi
- 芯片：STM32F1系列
- 库：STM32 HAL库

## 编译 & 烧录
> 仓库提供 `CMakePresets.json`，一键切换 Debug / Release 配置
```bash
# 拉取代码
git clone https://github.com/clustersstars/simple_keyboard.git
cd simple-keyboard

# 编译 Debug（调试版本）
cmake --preset Debug
cmake --build --preset Debug

# 编译 Release（量产版本）
cmake --preset Release
cmake --build --preset Release
