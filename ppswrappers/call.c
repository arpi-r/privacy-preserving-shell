#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<linux/types.h>
#include<stdlib.h>

struct ppshell_call_params {
    __u32 size; // for fwd/bkw compatability
    char *name; // name for the service to be called
    uid_t owner_euid; // effective user id of the owner
    char *auth_pwd;// password based auth (NULL = not checked)
};

int makesyscall(struct ppshell_call_params *pif)
{
	int err;	
	__asm(
	    "MOV x8, #552;" //x8 holds syscall no
	    " SVC #0;"      // supervisor call
	    "MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err; 
}

int main() {

	struct ppshell_call_params ppcprms;
	ppcprms.size = sizeof(struct ppshell_call_params);
	ppcprms.name = "listdir";
	ppcprms.owner_euid=1000;
	ppcprms.auth_pwd = "mypwd";
	
	int x = makesyscall(&ppcprms);
	printf("%d\n", x);
	
	return 0;
}

