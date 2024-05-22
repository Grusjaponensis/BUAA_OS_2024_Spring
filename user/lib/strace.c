#include <lib.h>

void strace_barrier(u_int env_id) {
	int straced_bak = straced;
	straced = 0;
	while (envs[ENVX(env_id)].env_status == ENV_RUNNABLE) {
		syscall_yield();
	}
	straced = straced_bak;
}

void strace_send(int sysno) {
	//debugf("111!!!\n\n");
	if (!((SYS_putchar <= sysno && sysno <= SYS_set_tlb_mod_entry) ||
	      (SYS_exofork <= sysno && sysno <= SYS_panic)) ||
	    sysno == SYS_set_trapframe) {
		return;
	}

	// Your code here. (1/2)
	int str_bac = straced;
	//debugf("11, %s, %d\n", __FILE__, __LINE__);
	if (straced != 0) {
		straced = 0;
		ipc_send(env->env_parent_id, sysno, 0, 0); // should perm be 0 or 1?
		syscall_set_env_status(0, ENV_NOT_RUNNABLE);
	}
	straced = str_bac;
	//debugf("22, %s, %d\n", __FILE__, __LINE__);
}

void strace_recv() {
	// Your code here. (2/2)
	int result;
	while ((result = ipc_recv(NULL, 0, 0)) != SYS_env_destroy) {
		//debugf("33, %s, %d\n", __FILE__, __LINE__);
		strace_barrier(env->env_ipc_from);
		recv_sysno(env->env_ipc_from, result);
		syscall_set_env_status(env->env_ipc_from, ENV_RUNNABLE);
	}
	strace_barrier(env->env_ipc_from);
	recv_sysno(env->env_ipc_from, result);
	syscall_set_env_status(env->env_ipc_from, ENV_RUNNABLE);
}
