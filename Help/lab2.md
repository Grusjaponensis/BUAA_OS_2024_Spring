```C
//已知Pde *pgdir
for (u_long i = 0; i < 1024; i++) { //遍历页目录的1024项
	Pde *pde = pgdir + i; //第i个页目录项对应的虚拟地址
	if ((*pde) & PTE_V) { //第i个页表有效
		for (u_long j = 0; j < 1024; j++) { //遍历第i个页表的1024项
			Pte *pte = (Pte*)KADDR(PTE_ADDR(*pde)) + j; //第j个页表项对应的虚拟地址
			if ((*pte) & PTE_V) { //第j个页有效
			/*例如：
			1.将(*pte)页表项映射到“某物理地址”，且保留后面的所有12位offset
			(*pte) = ((某物理地址 / BY2PG) << 12) | ((*pte) & 0xfff);
			上式作用等价于(*pte) = PTE_ADDR(pa) | ((*pte) & 0xfff);
			
			2.定向设置(*pte)页表项的权限位
			(*pte) = ((*pte) | PTE_SWP)  & (~PTE_V);
			
			3.改变TLB中的(*pte)页表项（实际上是使它无效）
			tlb_invalidate(asid, (i << 22) | (j << 12) );
			tlb_invalidate(asid, va);
			*/
			}
		}
	}
}
//已知Pde *pgdir
Pde *pde = pgdir + i;
   /* pde:                   第i个页目录项对应的虚拟地址
      (*pde):                第i个页目录项内容
      PTE_ADDR(*pde):        第i个页目录项内容中的物理地址 = 第i个页表的虚拟基地址
      KADDR(PTE_ADDR(*pde)): 第i个页表的物理基地址 */
Pte *pte = (Pte*)KADDR(PTE_ADDR(*pde)) + j; 
   /* pte:            第j个页表项对应的虚拟地址
      (*pde):         第j个页表项内容 */
u_long va =  (i << 22) | (j << 12) | offset ; // va 虚拟地址
u_long i  = PDX(va);
u_long j  = PTX(va);
```

```bash
ctags -R * # 使用ctag进行跳转
grep -r some_function ./ # 当前目录下搜索某个函数定义
```

在Vim中查找

1. 按/
2. 输入搜索样式
3. 按Enter进行搜索
4. 按n搜索下一个匹配结果，或者N查找前面一个匹配结果

**DEBUGK大法**：
1. 在include/下新建如下文件
```C
#ifndef _DBGK_H_
#define _DBGK_H_
#include <printk.h>
#define DEBUGK // 可注释

#ifdef DEBUGK
#define DEBUGK(fmt, ...) do { printk("debugk::" fmt, ##__VA_ARGS___);} while (0)
#else
#define DEBUGK(...)
#endif
#endif // !_DBGK_H_
```

2. 要想debug哪个 .c 文件就在文件前面加上 `#include <debugk.h>`在 assert 处想要输出的地方加上：
```C
DEBUGK("ckpt%d-待填入语句\n", 数字表示是加的第几个DEBUGK);
```
运行对应的 `make && make run`， 查看运行结果，如果卡在了哪里就能由 `debugk::ckpy` 看出来

3. 如果不行debug的内容输出影响对于整体程序的输出，可以注释掉 `include/debugk.h` 的下面这一行：
```C
/* #define DEBUGK // 可注释 */
```


```Makefile
make test lab=2_1 && make run         
# 先笼统地看一下运行结果

make test lab=2_1  # 编译成相应的测试数据
make objdump       # 得到 target/mos.objdump 的反汇编文件
make dbg           # 进入调试模式
r, 0               # 查看CP0寄存器的值
在target/mos.objdump中查看CP0寄存器中epc地址处的语句，向上找一下可以看到它是那个函数里面
在注释的位置帮助下，可以确定到底是哪条语句出了问题

breakpoint add [page_insert 或 0x80014560]    # 添加断点
c      # 运行到下一个断点，如果没有到断点，则说明根本不会执行到断点函数处
s [n]  # 向后执行 n 条汇编指令
unassemble  # 导出某一个地址后续（或附近）的汇编指令序列
dump [curenv 或 0x804320e8]       # 导出某一个地址后续（或附近）的内存信息
reg         # 导出所有寄存器的值
r, 0        # 导出CP0的寄存器值
tlbdump     # 导出TLB内容
trace       # 
```

**lab2_exam**
对于给定的页目录 pgdir，统计其包含的所有二级页表中满足以下条件的页表项：

1. 页表项有效；
2. 页表项映射的物理地址为给定的 Page *pp 对应的物理地址；
3. 页表项的权限包含给定的权限 perm_mask。
```C
u_int page_perm_stat(Pde *pgdir, struct Page *pp, u_int perm_mask) {
	int count = 0;//统计满足条件的页表项的数量
	Pde *pde;
	Pte *pte;
	for (int i = 0; i < 1024; i++) {
		pde = pgdir + i;
		if(!(*pde & PTE_V)) { //当前页目录是否有效
			continue;
		}
	
		for (int j = 0; j< 1024;j++ ){
			pte = (Pte*)KADDR(PTE_ADDR(*pde)) + j;
			if (!(*pte & PTE_V)) { ////当前页表是否有效
				continue;
			}
			if (((perm_mask | (*pte))== (*pte)) 
                && (((u_long)(page2pa(pp)))>>12) == (((u_long)(PTE_ADDR(*pte)))>>12))
				count++;
            /*该层if判断条件等价于
            (perm_mask & (*pte))== perm_mask
            (page2pa(pp) == PTE_ADDR(*pte))
            */
		}
    }
	return count;
}	
```

**lab2_extra**
实现一个swap
```C
// Interface for 'Passive Swap Out'
struct Page *swap_alloc(Pde *pgdir, u_int asid) {
	// Step 1: Ensure free page
	if (LIST_EMPTY(&page_free_swapable_list)) {
		/* Your Code Here (1/3) */
		u_char  *disk_swap = disk_alloc();
		u_long da = (u_long)disk_swap;
		struct Page *p = pa2page(SWAP_PAGE_BASE);//这里策略是只换0x3900000处的页
		for (u_long i = 0; i < 1024; i++) { //改变所有页表中映射到0x3900000的页表项
			Pde *pde = pgdir + i;
			if ((*pde) & PTE_V) {
				for (u_long j = 0; j < 1024; j++) {
					Pte *pte = (Pte*)KADDR(PTE_ADDR(*pde)) + j;
					if (((*pte) & PTE_V) && (PTE_ADDR(*pte) == SWAP_PAGE_BASE) ) {
						(*pte) = ((da / BY2PG) << 12) | ((*pte) & 0xfff);
						//上式作用等价于(*pte) = PTE_ADDR(da) | ((*pte) & 0xfff);保留后面的所有12位offset
						(*pte) = ((*pte) | PTE_SWP)  & (~PTE_V);
						tlb_invalidate(asid, (i << 22) | (j << 12) ); //tlb_invalidate(asid, va);
					}
				}
			}
		}
		memcpy((void *)da, (void *)page2kva(p), BY2PG); 
		//这里没有再删掉页控制块p对应的内容，是因为跳出该if之后，会执行剩下的语句，其中倒数第2句会帮助情空
		LIST_INSERT_HEAD(&page_free_swapable_list, p, pp_link);
	}

	// Step 2: Get a free page and clear it
	struct Page *pp = LIST_FIRST(&page_free_swapable_list);
	LIST_REMOVE(pp, pp_link);
	memset((void *)page2kva(pp), 0, BY2PG);

	return pp;
}

// Interfaces for 'Active Swap In'
static int is_swapped(Pde *pgdir, u_long va) {
	/* Your Code Here (2/3) */
	Pde *pde = pgdir + PDX(va);
	if (*pde & PTE_V) {
		Pte *pte = (Pte*)KADDR(PTE_ADDR(*pde)) + PTX(va);
		if ((*pte & PTE_SWP) && !(*pte & PTE_V)) {
			return 1;
		}
	}
	return 0;
}

static void swap(Pde *pgdir, u_int asid, u_long va) {
	/* Your Code Here (3/3) */
	struct Page *pp = swap_alloc(pgdir, asid); //可用于交换的内存块的页控制块
	u_long da = PTE_ADDR(*((Pte*)KADDR(PTE_ADDR(*(pgdir + PDX(va)))) + PTX(va))); //外存地址
	memcpy((void *)page2kva(pp), (void *)da, BY2PG);

	for (u_long i = 0; i < 1024; i++) { //所有页表项记录的映射在外存的页表项改到新申请下来的swap的内存地址
		Pde *pde = pgdir + i;
		if (*pde & PTE_V) {
			for (u_long j = 0; j < 1024; j++) {
				Pte *pte = (Pte*)KADDR(PTE_ADDR(*pde)) + j;
				if (!(*pte & PTE_V) && (*pte & PTE_SWP) && (PTE_ADDR(*pte) == da)) {
					//以下三句话含义均可类比于 swap_alloc
					(*pte) = ((page2pa(pp) / BY2PG) << 12) | ((*pte) & 0xfff);
					(*pte) = ((*pte) & ~PTE_SWP) | PTE_V;
					tlb_invalidate(asid, (i << 22) | (j << 12) );
				}
			}
		}
	}
	disk_free((u_char *)da);
	return;
}
```