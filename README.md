# STM32F405RGT6 机器人双路 CAN 电机控制及多任务管理框架

本项目是一个基于 **STM32F405RGT6** 微控制器、结合 **HAL 库** 与 **FreeRTOS** 实时操作系统搭建的机器人底层核心控制系统。项目实现了对**大疆 RoboMaster 系列智能电机（M3508 / M2006 / GM6020）**与**达妙科技（Damiao）动力电机（DM4310）**的双路 CAN 总线兼容驱动。系统集成了完整的串级 PID 控制算法、硬件级 CAN 接收滤波器以及基于 DMA 传输的高速串口通信接口。

## ⚙️ 开发环境与底层硬件架构

* **核心芯片**：STM32F405RGT6 (Cortex-M4 核心，主频 168MHz)
* **固件库版本**：STM32Cube HAL 库
* **实时操作系统**：FreeRTOS (CMSIS-RTOS V1 API 接口)
* **编译工具链**：Keil MDK v5
* **通信波特率**：
    * **CAN1 / CAN2**：1 Mbps (Prescaler=3, BS1=10TQ, BS2=3TQ, 总计 14TQ，在 APB1=42MHz 下精准达成 1MHz)
    * **USART3**：100000 bps (常用于高速外设或定制遥控协议，配置 8N1 模式)

---

## 🔌 硬件引脚分配表

| 硬件外设 | 引脚功能 | MCU 引脚 | 物理层配置说明 |
| :--- | :--- | :--- | :--- |
| **CAN1** | CAN1_RX / CAN1_TX | `PA11` / `PA12` | 复用为 `GPIO_AF9_CAN1`，接 CAN 收发器 |
| **CAN2** | CAN2_RX / CAN2_TX | `PB12` / `PB13` | 复用为 `GPIO_AF9_CAN2`，接 CAN 收发器 |
| **USART3** | USART3_TX / USART3_RX | `PB10` / `PB11` | 复用为 `GPIO_AF7_USART3`，接逻辑电平转换或接口板 |
| **DMA1** | Stream1 / Stream3 | — | 分别绑定 `USART3_RX` 与 `USART3_TX` 的 DMA 通道 4 传输 |
| **TIM2** | 硬件定时器 | 内置中断 | 配置为 FreeRTOS 运行时的底层高优先级系统 Tick 维持时钟 |

---

## 📁 关键代码模块及其职责

```text
├── main.c              # 系统核心入口，包含串级 PID 算法实现、CAN 异步中断接收回调及全驱数据解析
├── freertos.c          # RTOS 多任务调度核心，控制执行逻辑入口（如 StartDefaultTask 业务展示）
├── can.c / can.h       # 双路 CAN 控制器基础参数、时序及波特率配置
├── Filter.c / Filter.h # 统一的 CAN 硬件标识符掩码滤波器配置，开启双路中断通知并关联 FIFO0
├── test_DM.c / .h      # 达妙电机 MIT 模式核心驱动库（数据浮点/整型互转、报文封包、全套控制命令）
├── usart.c / usart.h   # USART3 串口及双向 DMA 传输控制初始化
├── dma.c / dma.h       # 统一管理和使能系统的 DMA1 控制器时钟
└── stm32f4xx_it.c      # 集中挂载硬件中断服务函数（CAN1/2 TX & RX, DMA, USART3 核心中断）
