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
sort: ;/*函数标签*/
    @args=0,pretend=0,frame= 24
    @frame_needed=1,uses_anonymous_args = 0
    @link register save eliminated.
    push {r7}
    sub sp, sp, #28
    add r7, sp, #0
    str r0, [r7, #4]   ;/* r0为第一个参数buffer，将buffer保存到[r7,#4] */
    str r1, [r7]       ;/* r1为第二个参数bufferlen，保存到[r7] */
    movs r3, #0        ;/* r3为i */
    str r3, [r7, #20]  ;/* 将i保存在[r7,#20] */
    b .L2

.L6:
    movs r3, #0        ;/* r3 = 0 */
    str r3, [r7, #16]  ;/* [r7+16] = r3 */
    b .L3

.L5:  ;/* 先判断buffer[j]和buffer[j+1]的大小，然后根据结果来进行是否交换 */
    ldr r3, [r7, #16]   ;/* r3 = [r7+16], r3 存放着第二层循环，相当于 j */
    lsls r3, r3, #2     ;/* r3 = r3 * 4 */
    ldr r2, [r7, #4]    ;/* r2 = buffer 的地址 */
    add r3, r3, r2      ;/* r3 = buffer[j] 的地址 */
    ldr r2, [r3]        ;/* r2 = buffer[j] */
    ldr r3, [r7, #16]   ;/* r3 = j */
    adds r3, r3, #1     ;/* r3 = j + 1 */
    lsls r3, r3, #2     ;/* r3 = (j + 1) * 4 */
    ldr r1, [r7, #4]    ;/* r1 = buffer 的地址 */
    add r3, r3, r1      ;/* r3 = buffer[j+1] 的地址 */
    ldr r3, [r3]        ;/* r3 = buffer[j+1] */
    cmp r2, r3          ;/* 比较 buffer[j] 和 buffer[j+1] */
    bge .L4              ;/* 如果 buffer[j] >= buffer[j+1]，跳到 L4，不交换 */

    ;/* 交换两个数 */
    ldr r3, [r7, #16]   ;/* r3 = j */
    adds r3, r3, #1     ;/* r3 = j + 1 */
    lsls r3, r3, #2     ;/* r3 = j * 4 */
    ldr r2, [r7, #4]    ;/* r2 = buffer 的地址 */
    add r3, r3, r2      ;/* r3 = buffer[j+1] 的地址 */
    ldr r3, [r3]        ;/* r3 = buffer[j+1] */
    str r3, [r7, #12]   ;/* tmp = buffer[j+1] */

    ldr r3, [r7, #16]   ;/* r3 = j */
    adds r3, r3, #1     ;/* r3 = j + 1 */
    lsls r3, r3, #2     ;/* r3 = j * 4 */
    ldr r2, [r7, #4]    ;/* r2 = buffer 的地址 */
    add r3, r3, r2      ;/* r3 = buffer[j+1] 的地址 */
    ldr r2, [r7, #16]   ;/* r2 = j */
    lsls r2, r2, #2     ;/* r2 = j * 4 */
    ldr r1, [r7, #4]    ;/* r1 = buffer 的地址 */
    add r2, r2, r1      ;/* r2 = buffer[j] 的地址 */
    ldr r2, [r2]        ;/* r2 = buffer[j] */
    str r2, [r3]        ;/* buffer[j+1] = buffer[j] */
    ldr r3, [r7, #16]   ;/* r3 = j */
    lsls r3, r3, #2     ;/* r3 = j * 4 */
    ldr r2, [r7, #4]    ;/* r2 = buffer 的地址 */
    add r3, r3, r2      ;/* r3 = buffer[j] 的地址 */
    ldr r2, [r7, #12]   ;/* r2 = tmp, [r7+12] 存放了之前的 buffer[j+1] */
    str r2, [r3]        ;/* buffer[j] = tmp */

.L4:  ;/* 相当于 j = j + 1 */
    ldr r3, [r7, #16]   ;/* r3 = j */
    adds r3, r3, #1     ;/* j++ */
    str r3, [r7, #16]   ;/* 将 j++ 写入原来的地址 */

.L3:
    ldr r2, [r7]        ;/* r2 = bufferlen */
    ldr r3, [r7, #20]   ;/* r3 = i */
    subs r3, r2, r3     ;/* r3 = bufferlen - i */
    subs r2, r3, #1     ;/* r2 = bufferlen - i - 1 */
    ldr r3, [r7, #16]   ;/* r3 = j */
    cmp r2, r3          ;/* 判断 j < bufferlen - i - 1 */
    bgt .L5              ;/* 如果小于，则进入 L5 */

    ;/* 内层循环结束，相当于 i = i + 1 */
    ldr r3, [r7, #20]
    adds r3, r3, #1
    str r3, [r7, #20]

.L2:  ;/* 第一层循环 */
    ldr r2, [r7, #20]   ;/* r2 = i */
    ldr r3, [r7]        ;/* r3 = bufferlen */
    cmp r2, r3          ;/* 判断第一层循环是否结束 , i < bufferlen */
    blt .L6              ;/* 如果 i < bufferlen，则进入 L6 */
    nop
    adds r7, r7, #28
    mov sp, r7
    @ sp needed
    ldr r7, [sp], #4
    bx lr

    .size sort, .-sort
    .ident "GCC:(LinaroGCC5.4-2017.05) 5.4.1 20170404"
    .section .note.GNU-stack,"",%progbits
