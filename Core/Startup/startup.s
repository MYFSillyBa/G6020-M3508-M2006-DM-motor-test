/* 最小STM32启动文件 */
.syntax unified
.cpu cortex-m4
.thumb

.global Reset_Handler
.global Default_Handler

/* 中断向量表 */
.section .isr_vector
.align 2
.word 0x20001000    /* 初始堆栈指针（根据你的RAM修改） */
.word Reset_Handler /* 复位向量 */

/* 复位处理程序 */
.section .text
Reset_Handler:
    /* 调用SystemInit（如果有） */
    /* bl SystemInit */

    /* 调用main函数 */
    bl main

    /* 如果main返回，进入死循环 */
Infinite_Loop:
    b Infinite_Loop

/* 默认中断处理程序 */
Default_Handler:
    b Default_Handler

.end