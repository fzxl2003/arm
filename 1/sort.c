void sort(int *buffer, int bufferlen) {
    int i, j, tmp;
    
    // 假设栈上分配空间的位置
    // r7: 保存栈基址，栈帧的基地址
    // [r7, #4] 存储 buffer 地址
    // [r7] 存储 bufferlen 长度
    // [r7, #20] 存储 i
    // [r7, #16] 存储 j
    // [r7, #12] 存储 tmp

    // 外层循环，控制排序的轮数
    for (i = 0; i < bufferlen; i++) {   // i == [r7, #20]
        // 内层循环，进行相邻元素的比较和交换
        for (j = 0; j < bufferlen - i - 1; j++) {  // j == [r7, #16]
            // 如果 buffer[j] > buffer[j+1]，交换它们
            if (buffer[j] < buffer[j + 1]) {
                tmp = buffer[j+1];    // tmp == [r7, #12]
                buffer[j+1] = buffer[j];
                buffer[j] = tmp;
            }
        }
    }
}

//选择排序
void select_sort(int *buffer, int bufferlen) {

    
    int i, j, min, tmp;
    
    // 假设栈上分配空间的位置
    // r7: 保存栈基址，栈帧的基地址
    // [r7, #4] 存储 buffer 地址
    // [r7] 存储 bufferlen 长度
    // [r7, #20] 存储 i
    // [r7, #16] 存储 j
    // [r7, #12] 存储 min
    // [r7, #8] 存储 tmp

    // 外层循环，控制排序的轮数
    for (i = 0; i < bufferlen; i++) {   // i == [r7, #20]
        min = i;
        // 内层循环，进行相邻元素的比较和交换
        for (j = i + 1; j < bufferlen; j++) {  // j == [r7, #16]
            // 如果 buffer[j] > buffer[j+1]，交换它们
            if (buffer[j] < buffer[min]) {
                min = j;
            }
        }
        if (min != i) {
            tmp = buffer[i];    // tmp == [r7, #8]
            buffer[i] = buffer[min];
            buffer[min] = tmp;
        }
    }
}


栈帧分配：

在函数开始时，栈空间被分配给 i, j, min, tmp 等变量（通过栈操作）。这些变量的地址通过偏移量存储在栈帧中。
r7 寄存器指向栈帧的基址，栈帧中的变量通过 r7 寄存器的偏移来访问：
[r7, #4] 存储 buffer 地址（即数组首地址）。
[r7] 存储 bufferlen 长度。
[r7, #20] 存储 i（外层循环计数器）。
[r7, #16] 存储 j（内层循环计数器）。
[r7, #12] 存储 min（当前最小值的位置）。
[r7, #8] 存储 tmp（临时变量，用于交换元素）。
外层循环：

C 代码中的 for (i = 0; i < bufferlen; i++) 对应的汇编代码在 .L2 和 .L7 中实现。
汇编中使用 r7, #20 来保存 i，并在每次循环时更新。
汇编代码通过 movs r3, #0 清零并存储到栈中初始化计数器。
内层循环：

C 代码中的 for (j = i + 1; j < bufferlen; j++) 对应的汇编代码在 .L5 和 .L3 中实现。
内层循环通过比较数组元素 buffer[j] 和 buffer[min] 来找出最小值。汇编代码使用 ldr 加载数组元素，使用 cmp 进行比较，bge 跳过交换操作。
交换元素：

当找到新的最小值时，C 代码中的交换操作 tmp = buffer[i]; buffer[i] = buffer[min]; buffer[min] = tmp; 对应的汇编代码在 .L4 部分。
汇编代码通过 str 指令将 buffer[i] 和 buffer[min] 的值交换。交换使用临时变量 tmp 来存储 buffer[i] 的值，然后将最小值放到 buffer[i]，最后将 tmp 的值放到 buffer[min]。
循环控制和退出：

外层循环通过比较 i 和 bufferlen 来控制循环的退出。当 i >= bufferlen 时退出。
汇编代码中的 .L2 和 .L7 实现了循环条件的检查与跳转。
函数退出：

函数的最后通过 bx lr 指令返回到调用者。退出时恢复了栈帧的状态（add r7, r7, #28 和 mov sp, r7）。