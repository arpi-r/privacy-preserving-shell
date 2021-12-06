#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <stdlib.h>
#include <string.h>

char* substr(const char *src, int m, int len)
{
	char *dest = (char*)malloc(sizeof(char) * (len + 1));

	strncpy(dest, (src + m), len);

	return dest;
}

int makesyscall_get_num_services(int *num_services)
{
	int err;
	__asm(
		"MOV x8, #554;" //x8 holds syscall no
		"SVC #0;"      // supervisor call
		"MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err;
}

int makesyscall(char *list_info, int *list_sizes)
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
	int cur = 0;

	int *num_services = (int *) malloc(sizeof(int));
	int ret_getnum = makesyscall_get_num_services(num_services);

	if (ret_getnum != 0)
	{
		printf("pps_get_num_services syscall failed\n");
		return ret_getnum;
	}


	if (*num_services == 0)
	{
		printf("No services exist\n");
		return 0;
	}

	int *list_sizes;
	list_sizes = (int *) malloc(*num_services * 3 * sizeof(int));

	char *list_vals;
	list_vals = (char *) malloc(*num_services * 1107);

	int ret = makesyscall(list_vals, list_sizes);

	// printf("ppslist syscall: %d\n", ret);

	if (ret != 0)
	{
		printf("pps_list syscall failed\n");
		return ret;
	}


	printf("List of Services Available:\n");
	for (int i = 0; i < (*num_services * 3); i+=3)
	{
		char *euid = substr(list_vals, cur, list_sizes[i]);
		cur += list_sizes[i];
		char *name = substr(list_vals, cur, list_sizes[i+1]);
		cur += list_sizes[i+1];
		char *desc = substr(list_vals, cur, list_sizes[i+2]);
		cur += list_sizes[i+2];
		printf("%s: %s\t- %s\n", euid, name, desc);
	}

	return 0;
}

