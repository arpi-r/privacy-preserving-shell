#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <stdlib.h>

int makesyscall()
{
	int err;	
	__asm(
		"MOV x8, #553;" //x8 holds syscall no
		"SVC #0;"      // supervisor call
		"MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err; 
}

int main()
{
	int ret = makesyscall();
	printf("ppslist syscall: %d\n", ret);
	
	return 0;
}

