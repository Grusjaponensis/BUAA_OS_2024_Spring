- fork 往年答案1
```c
int make_shared(void *va) {
    u_int perm = PTE_D | PTE_V;
    if (!(vpd[va >> 22] & PTE_V) || !(vpt[va >> 12] & PTE_V)) { 
        //当前进程的页表中不存在该虚拟页
        if (syscall_mem_alloc(0, ROUNDDOWN(va, BY2PG), perm) != 0) { 
            //将envid设为0，表示默认curenv
            return -1;
    	}
    } 
    perm = vpt[VPN(va)] & 0xfff; //获得va的perm
    if (va >= (void *)UTOP || 
        ((vpd[va >> 22] & PTE_V) && (vpt[va >> 12] & PTE_V) && !(perm & PTE_D))) {
        return -1;
    }
    perm = perm | PTE_LIBRARY;
    u_int addr = VPN(va) * BY2PG; 
    if (syscall_mem_map(0, (void *)addr, 0, (void *)addr, perm) != 0) {
			return -1;
	} 
    return ROUNDDOWN(vpt[VPN(va)] & (~0xfff), BY2PG);
}
```

- fork yanna_100:
```c
//经过和其他同学的讨论，以及代码对拍，下面这个代码可能可以得100分
u_int sys_barrier_wait(u_int* p_barrier_num, u_int* p_barrier_useful) {
	static u_int env_not[100];
	static u_int N = 0;
	static u_int num = 0;
	static u_int useful = 0;
	if ((*p_barrier_num) > N) {
		N = (*p_barrier_num);
		num = N;
		useful = (*p_barrier_useful);
	}
	if (num == 0) { //third version
			useful = 0;
			return ENV_RUNNABLE;
	}	
	if (useful == 1) {
		for (u_int i = 0; i < N - num; i++) {
			if (env_not[i] == curenv->env_id) {
					return ENV_NOT_RUNNABLE;
			}
		}
		env_not[N - num] = curenv->env_id;
		num--;
		return ENV_NOT_RUNNABLE;
	}
	return ENV_RUNNABLE;
}
```

- fork yanna_70:
```c
//测试数据点6不过，得分70分
u_int sys_barrier_wait(u_int* p_barrier_num, u_int* p_barrier_useful) {
	static u_int env_not[100];
	static u_int N = 0;
	static u_int num = 0;
	static u_int useful = 0;
	if ((*p_barrier_num) > N) {
		N = (*p_barrier_num);
		num = N;
		useful = (*p_barrier_useful);
	}
	if (useful == 1) {
		if (num == 0) { //second version
				useful = 0;
				return ENV_RUNNABLE;
		}	
		for (u_int i = 0; i < N - num; i++) {
			if (env_not[i] == curenv->env_id) {
					return ENV_NOT_RUNNABLE;
			}
		}
		env_not[N - num] = curenv->env_id;
		num--;
		return ENV_NOT_RUNNABLE;
	}
	return ENV_RUNNABLE;
}
```

```c
int fork(void) (user/lib/fork.c)

【注意】: env = envs + ENVX(syscall_getenvid());

syscall_getenvid()：获得当前进程的envid

envs + ENVX(…) : 由envid获得env

【注意】: vpd是页目录首地址，以vpd为基地址，加上页目录项偏移数即可指向va对应页目录项，即(*vpd) + (va >> 22) 或 vpd[va >> 22] ；

二级页表的物理地址：vpd[va >> 22] & (~0xfff)

提前判断有效位： (vpd[va >> 22] & PTE_V) 或 (vpd[VPN(va) >> 10] & PTE_V)

vpt是页表首地址，以vpt为基地址，加上页表项偏移数即可指向va对应的页表项，即(*vpt) + (va >> 12) 或 vpt[va >> 12] 即 vpt[VPN(va)]；

物理页面地址：vpt[va >> 12] & (~0xfff)

提前判断有效位： (vpt[va >> 12] & PTE_V) 或 (vpt[VPN(va)] & PTE_V)

vpn = VPN(va) = va >> 12（ 虚拟页号）

static void ... cow_entry(...) (user/lib/fork.c)
```