void mips_init(u_int argc, char **argv, char **penv, u_int ram_low_size) {
	printk("init.c:\tmips_init() is called\n");

	mips_detect_memory(ram_low_size);
	mips_vm_init();
	page_init();
	printk("Hello, world!\n");
	env_init();
	printk("Hello, world1\n");
	struct Env *ppb = ENV_CREATE_PRIORITY(test_ppb, 5);
	printk("Hello, world3\n");
	struct Env *ppc = ENV_CREATE_PRIORITY(test_ppc, 5);
	printk("Hello,world4\n");
	ppc->env_parent_id = ppb->env_id;

	schedule(0);
	panic("init.c:\tend of mips_init() reached!");
}
