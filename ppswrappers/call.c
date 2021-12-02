#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<linux/types.h>
#include<stdlib.h>
#include<string.h>
#include<termios.h>

// https://www.gnu.org/software/libc/manual/html_node/getpass.html
ssize_t my_getpass (char **lineptr, size_t *n, FILE *stream)
{
  struct termios old, new;
  int nread;

  /* Turn echoing off and fail if we can’t. */
  if (tcgetattr (fileno (stream), &old) != 0)
    return -1;
  new = old;
  new.c_lflag &= ~ECHO;
  if (tcsetattr (fileno (stream), TCSAFLUSH, &new) != 0)
    return -1;

  /* Read the passphrase */
  nread = getline (lineptr, n, stream);

  /* Restore terminal. */
  (void) tcsetattr (fileno (stream), TCSAFLUSH, &old);

  return nread;
}

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

void usage()
{
	printf("ppshell call: call a registered service\n");
	printf("usage: pps_call <name> <owner_uid> -p\n");
	printf("<name>: name of the service, non empty string, REQUIRED\n");
	printf("<owner_uid>: uid of the owner of service, non negative valid user id, REQUIRED\n");
	printf("-p: password for authenticating, enter non-empty password when prompted, OPTIONAL\n");
	exit(0);
}


uid_t getuidfromstr(char* str)
{
	int len = strlen(str);
	for(int i = 0; i < strlen(str); i++)
	{
		if(str[i] < '0' || str[i] > '9')
			usage();
	}
	return (uid_t) atoi(str);
}

int main(int argc, char** argv) {

	if(argc < 3)
		usage();
	
	char *name;
	uid_t owner_euid;
	char* pwd = NULL;

	if(argv[1] == NULL || strlen(argv[1]) == 0) // non-empty name
		usage();
	else
		name = argv[1];
	
	if(argv[2] == NULL || strlen(argv[2]) == 0)
		usage();
	else {
		owner_euid = getuidfromstr(argv[2]); //not using atoi directly as it doesnt distinguish error and 0
	}

	if(argc > 3) {
		if(argv[3] == NULL || strcmp(argv[3], "-p"))
			usage();
		
		printf("Enter password: ");

		size_t pwd_len = 0;
		ssize_t pwd_read = my_getpass(&pwd, &pwd_len, stdin);
		if(pwd_read <= 1) //ignoring "\n"
			usage();
		pwd[pwd_read - 1] = 0; // discarding \n
	}

	int pid=fork();

	if(pid<0) /* Why is this here? */
	{
		fprintf(stderr, "Fork failed: try again");
		exit(-1);
	}
	else if (pid == 0)
	{
		printf("Making syscall to execute service\n");
		
		struct ppshell_call_params ppcprms;
		ppcprms.size = sizeof(struct ppshell_call_params);
		ppcprms.name = name;
		ppcprms.owner_euid = owner_euid;
		ppcprms.auth_pwd = pwd;
		
		int x = makesyscall(&ppcprms);
		printf("%d\n", x);
	}
	else
	{
		printf("Waiting for child to finish\n");
		wait(pid);
	}
	
	return 0;
}

