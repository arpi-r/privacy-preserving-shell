#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<linux/types.h>
#include<stdlib.h>

struct ppshell_create_params {
    __u32 size; // for fwd/bkw compatability
    char *name; // name for the service, unique per user
    char *description; // verbose description of the service, shown to other users
    char *command; // command to be executed
    char *auth_pwd;// password based auth (NULL = disabled)
    uid_t *auth_uid_list;
    __u32 auth_uid_len;
    char** environ;
    __u32 env_len;
};

int makesyscall(struct ppshell_create_params *pif)
{
	int err;	
	__asm(
	    "MOV x8, #551;" //x8 holds syscall no
	    " SVC #0;"      // supervisor call
	    "MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err; 
}

int main() {

	struct ppshell_create_params ppcprms;
	ppcprms.size = sizeof(struct ppshell_create_params);
	ppcprms.name = "dssd7";
	ppcprms.description = "dssd";
	ppcprms.command = "dssd";
	ppcprms.auth_pwd = "yy";
	ppcprms.auth_uid_list = malloc(3*sizeof(uid_t));
	ppcprms.auth_uid_list[0] = 2;
	ppcprms.auth_uid_list[1] = 1;
	ppcprms.auth_uid_list[2] = 100;
	ppcprms.auth_uid_len = 3;
	
	
	ppcprms.environ = malloc(3*sizeof(char*));
	ppcprms.environ[0] = "HOdsME=home";
	ppcprms.environ[1] = "random";
	ppcprms.environ[2] = "dsds";
	ppcprms.env_len = 3;
	
	int x = makesyscall(&ppcprms);
	printf("%d\n", x);
	
	return 0;
}

