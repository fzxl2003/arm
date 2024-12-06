    .arch armv7-a
    .eabi_attribute 28,1
    .fpu vfpv3-d16
    .eabi_attribute 20,1
    .eabi_attribute 21,1
    .eabi_attribute 23,3
    .eabi_attribute 24,1
    .eabi_attribute 25,1
    .eabi_attribute 26,2
    .eabi_attribute 30,6
    .eabi_attribute 34,1
    .eabi_attribute 18,4
    .file "sort.c"
    .text
    .align 2
    .global sort
    .syntax unified
    .thumb
    .thumb_func
    .type sort, %function

sort:
    ; 保存 r7 寄存器，并为局部变量分配栈空间
    push    {r7}                   
    sub     sp, sp, #28           
    add     r7, sp, #0            

    ; 保存传入的参数 buffer 和 bufferlen
    str     r0, [r7, #4]           ; 保存 buffer 地址
    str     r1, [r7]               ; 保存 bufferlen 长度

    ; 初始化 i = 0
    movs    r3, #0
    str     r3, [r7, #20]          ; 保存 i 的初值

    b       .L2                    ; 跳到外层循环开始

.L7:
    ; 外层循环：设置 min = i，初始化内层循环 j = i + 1
    ldr     r3, [r7, #20]          ; 加载 i
    str     r3, [r7, #12]          ; 设置 min = i
    ldr     r3, [r7, #20]          ; 加载 i
    adds    r3, r3, #1             ; 设置 j = i + 1
    str     r3, [r7, #16]          ; 保存 j

    b       .L3                    ; 跳到内层循环

.L5:
    ; 内层循环：比较 buffer[j] 和 buffer[min]，如果 buffer[j] < buffer[min] 更新 min
    ldr     r3, [r7, #16]        
    lsls    r3, r3, #2             
    ldr     r2, [r7, #4]          
    add     r3, r3, r2            
    ldr     r2, [r3]               ; 加载 buffer[j]
    
    ldr     r3, [r7, #12]         
    lsls    r3, r3, #2            
    ldr     r1, [r7, #4]         
    add     r3, r3, r1             
    ldr     r3, [r3]               ; 加载 buffer[min]
    
    cmp     r2, r3                 ; 比较 buffer[j] 和 buffer[min]
    bge     .L4                    ; 如果 buffer[j] >= buffer[min] 跳过更新

    ; 更新 min = j
    ldr     r3, [r7, #16]          ; 加载 j
    str     r3, [r7, #12]          ; 更新 min

.L4:
    ; 更新 j = j + 1，检查是否继续内层循环
    ldr     r3, [r7, #16]          ; 加载 j
    adds    r3, r3, #1             ; j = j + 1
    str     r3, [r7, #16]          ; 保存 j

.L3:
    ; 检查内层循环是否结束，j < bufferlen
    ldr     r2, [r7, #16]          ; 加载 j
    ldr     r3, [r7]               ; 加载 bufferlen
    cmp     r2, r3                 ; 比较 j 和 bufferlen
    blt     .L5                    ; 如果 j < bufferlen，继续内层循环

    ; 检查是否需要交换，如果 min != i，则交换 buffer[i] 和 buffer[min]
    ldr     r2, [r7, #12]      
    ldr     r3, [r7, #20]         
    cmp     r2, r3                
    beq     .L6                    

    ; 执行交换操作：tmp = buffer[i], buffer[i] = buffer[min], buffer[min] = tmp
    ldr     r3, [r7, #20]          
    lsls    r3, r3, #2             
    ldr     r2, [r7, #4]           
    add     r3, r3, r2             
    ldr     r3, [r3]               
    str     r3, [r7, #8]           

    ldr     r3, [r7, #12]          ; 加载 min
    lsls    r3, r3, #2             
    ldr     r2, [r7, #4]           
    add     r3, r3, r2             
    ldr     r3, [r3]               
    str     r3, [r7, #20]          

    ldr     r3, [r7, #8]           
    lsls    r3, r3, #2             
    ldr     r2, [r7, #4]           
    add     r3, r3, r2             
    ldr     r2, [r7, #8]           
    str     r2, [r3]               

.L6:
    ; 外层循环 i = i + 1，检查是否继续
    ldr     r3, [r7, #20]          
    adds    r3, r3, #1             
    str     r3, [r7, #20]          

.L2:
    ; 检查外层循环是否结束，i < bufferlen
    ldr     r2, [r7, #20]          
    ldr     r3, [r7]               
    cmp     r2, r3                 
    blt     .L7                    

    ; 函数退出，恢复栈帧并返回
    nop                           
    nop                           
    adds    r7, r7, #28           
    mov     sp, r7                 
    ldr     r7, [sp], #4           
    bx      lr                     
    .size sort, .-sort            
    .ident "GCC:(LinaroGCC5.4-2017.05) 5.4.1 20170404"
    .section .note.GNU-stack,"",%progbits  
