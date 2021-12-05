#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <stdlib.h>

int makesyscall(char *service_name, char *service_info, int *service_size)
{
	int err;	
	__asm(
		"MOV x8, #555;" //x8 holds syscall no
		"SVC #0;"      // supervisor call
		"MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err; 
}

int main()
{
	char *service_name = "hello_world_service";
	char *service_info = (char *) malloc(500);
	int *service_size = (int *) malloc(sizeof(int));

	int ret = makesyscall(service_name, service_info, service_size);

	printf("pps_show syscall: %d\n", ret);

	return 0;
}

