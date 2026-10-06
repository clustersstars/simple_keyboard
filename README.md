# simple-keyboard
DIY 双MCU机械键盘固件

## 项目简介
双STM32分离架构：
- **nkro_keyboard**：主控MCU，负责矩阵按键扫描、USB NKRO HID键盘上报
- **nkro_ws2812b**：灯光从机MCU，专门驱动WS2812 RGB灯带；通过UART串口接收主控下发的灯光指令

灯光逻辑和键盘核心逻辑物理隔离，修改灯效不会影响USB键盘功能，方便调试与二次开发。

## 开发环境
- IDE：Keil MDK5
- 芯片：STM32F1系列
- 库：STM32 HAL库

## 编译 & 烧录
1. 主控固件：`nkro_keyboard/MDK-ARM/nkro_keyboard.uvprojx`
   编译后烧录至主控STM32
2. 灯光固件：`nkro_ws2812b/MDK-ARM/nkro_ws2812b.uvprojx`
   编译后烧录至灯光从机STM32

## 通讯协议
主控 ↔ 灯光从机：UART串口通信
主控下发RGB灯效指令给从机，从机只负责解析指令、驱动WS2812。

## 硬件信息
硬件相关资料存放于 `hardware/` 目录：
- PCB工程源文件（嘉立创EDA可直接打开修改）
- 原理图PDF
- BOM物料清单（元件型号、封装、数量）
- gerber.zip 光绘文件包，可直接提交PCB工厂打板
- PCB预览图片

硬件架构：双STM32独立工作，UART交叉连接（主控TX → 从机RX，主控RX → 从机TX），共用供电。

## License
MIT
