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

int makesyscall(char *service_name, int *service_name_size, char *service_info, int *service_size)
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
	// TODO: get service name as input
	// TODO: handle special cases: syscall fails, service not found
	char *service_name = "hello_world_service";
	int service_name_len = strlen(service_name);
	char *service_info = (char *) malloc(500);
	int *service_size = (int *) malloc(sizeof(int) * 207);
	int cur = 0;

	int ret = makesyscall(service_name, &service_name_len, service_info, service_size);

	printf("pps_show syscall: %d\n", ret);

	printf("Service Details:\n");

	printf("Name: %s\n", service_name);

	printf("Owner euid: %s\n", substr(service_info, cur, service_size[0]));
	cur += service_size[0];

	printf("Description: %s\n", substr(service_info, cur, service_size[1]));
	cur += service_size[1];

	printf("Command: %s\n", substr(service_info, cur, service_size[2]));
	cur += service_size[2];

	if (service_size[3] == -1)
	{
		printf("No Password\n");
	}
	else
	{
		printf("Password: %s\n", substr(service_info, cur, service_size[3]));
		cur += service_size[3];
	}

	if (service_size[4] == 0)
	{
		printf("No Auth List\n");
	}
	else
	{
		printf("Auth List: ");
		for (int i = 5; i < 5 + service_size[4]; i++)
		{
			printf("%s; ", substr(service_info, cur, service_size[i]));
			cur += service_size[i];
		}
		printf("\n");
	}

	int env_list_start = 5 + service_size[4];
	if ( service_size[env_list_start] == 0)
	{
		printf("No Special Environment\n");
	}
	else
	{
		printf("Environment: ");
		for (int i = env_list_start + 1; i < env_list_start + 1 + service_size[env_list_start]; i++)
		{
			printf("%s; ", substr(service_info, cur, service_size[i]));
			cur += service_size[i];
		}
		printf("\n");
	}
	return 0;
}
