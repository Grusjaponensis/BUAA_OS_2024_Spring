```C
void do_ov(struct Trapframe *tf) {
    curenv->env_ov_cnt++;
    
    u_long va = tf->cp0_epc; //va是异常指令的虚拟地址（用户虚拟地址）
    Pte *pte;
    page_lookup(curenv->env_pgdir, va, &pte);//通过查询curenv页表，获得页表项
    u_long pa = PTE_ADDR(*pte) | (va & 0xfff);//由页表项获得物理地址
    u_long kva = KADDR(pa);
    //将物理地址转化至"kseg0 区间中对应的虚拟地址"（内核虚拟地址）
    
    int *instr = (int *)kva;//内核虚拟地址可以直接访存获得指令
    int code = (*instr) >> 26; // get instruction op_code [31:26]
    int subcode = (*instr) & (0xf); // get instruction func_code [5:0]
    
    if (code == 0) {
        if (subcode == 0) {
            printk("add ov handled\n");
        } else if (subcode == 2) {
            printk("sub ov handled\n");
        }
        (*instr) = (*instr)|(0x1); //把指令换成addu或subu
    } else {
        tf->cp0_epc += 4;
        printk("addi ov handled\n");
        int reg_s = ((*instr) >> 21) & (0x1f);
        int reg_t = ((*instr) >> 16) & (0x1f);
        u_int imm = (*instr) & (0xffff); // get imm16 from instruction [15:0]
        tf->regs[reg_t] = tf->regs[reg_s] / (u_int)2 + imm / (u_int)2;
    }
    return;
}
```

```verilog
`define Op 31:26
`define Rs 25:21
`define Rt 20:16
`define Rd 15:11
`define func 5:0
`define Imm16 15:0
`define Addr26 25:0
```

- Extra
```C
void do_adel(char* sp) {// handler_adel把sp寄存器的值传给do_adel（BUILD_HANDLER中 move a0,sp j do_adel
    struct Trapframe *tf;
    tf = (struct Trapframe*)sp; // 找到保存所有寄存器的栈空间
    u_int instr;    // 对应指令
    u_int* instr_ptr;   //  对应指令的地址（PC）
    instr_ptr = tf->cp0_epc;    // 获取了epc
    instr = *instr_ptr; // 获取对应指令
    u_int opcode = instr & 0xFC000000; 
    opcode = opcode >> 26;  // 获取opcode
    instr = instr & 0x03FFFFFF; // 把instr的opcode先清0
    /* 开始处理opcode */
    if (opcode == 0b100011) { // lw 变成 lh
        opcode = 0b100001;
    }
    else { // lh 变成 lb
        opcode = 0b100000;
    }
    opcode = opcode << 26;
    instr = instr | opcode;
    *instr_ptr = instr; // 修改指令机器码
    return;
}
```

- Exam
```C
void sched_yield(void)
{
    static int count = 0; // remaining time slices of current env
    static int point = 0; // current env_sched_list index
    static struct Env* e = NULL;
    static struct Env* nexte = NULL;
    static int oddFlag = 0;
    static int weights[3] = {1, 2, 4}; 
    if (count == 0 || e == NULL || e->env_status != ENV_RUNNABLE) {
        while (1) {
            if (LIST_EMPTY(&env_sched_list[point])) point = (point + 1) % 3;    // point轮转
            nexte = LIST_FIRST(&env_sched_list[point]);
            if (nexte->env_pri % 2 == 0) {
                oddFlag = 0;    // 判断奇数
            } else {
                oddFlag = 1;
            }
            LIST_REMOVE(nexte, env_sched_link);
            // 根据奇偶插入相应队列队尾
            if (oddFlag) {
                LIST_INSERT_TAIL(&env_sched_list[(point + 1) % 3], nexte, env_sched_link);
            } else {
                LIST_INSERT_TAIL(&env_sched_list[(point + 2) % 3], nexte, env_sched_link);
            }
            
            if (nexte != NULL && nexte->env_status == ENV_RUNNABLE) {
                break;
            }
        }
        // find next env -> renew count
        e = nexte;
        count = e->env_pri * weights[point];
    }
    count--;
    env_run(e);
}
```