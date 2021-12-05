#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <stdlib.h>

int makesyscall(int *num_services)
{
	int err;	
	__asm(
		"MOV x8, #554;" //x8 holds syscall no
		"SVC #0;"      // supervisor call
		"MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err; 
}

int main()
{
	int *num_services = (int *) malloc(sizeof(int));

	int ret = makesyscall(num_services);

	printf("pps_get_num_services syscall: %d\n", ret);

	return 0;
}

