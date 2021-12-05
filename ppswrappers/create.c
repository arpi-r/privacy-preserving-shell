#include<unistd.h>
#include<stdio.h>
#include<sys/syscall.h>
#include<linux/types.h>
#include<stdlib.h>
#include<string.h>
#include<termios.h>

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

int read_string(char **lineptr, char* verb, size_t n)
{
	printf("Enter %s for the service: ", verb);
	int nread = getline (lineptr, &n, stdin);
	if(nread <= 1) //ignoring "\n"
		return 0;
	(*lineptr)[nread - 1] = 0; // discarding \n
	return 1;
}

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


int main() {

	struct ppshell_create_params ppcprms;
	
	ppcprms.name = ppcprms.description = ppcprms.command = ppcprms.auth_pwd = NULL;
	ppcprms.auth_uid_list = NULL;
	ppcprms.environ = NULL;
	ppcprms.size = sizeof(struct ppshell_create_params);
	
	while(!read_string(&ppcprms.name, "name", 100));
	while(!read_string(&ppcprms.description, "description", 1000));
	while(!read_string(&ppcprms.command, "command", 800));
	
	printf("Enter a password for the service (type ENTER to skip): ");
	size_t pwd_len = 0;
	ssize_t pwd_read = my_getpass(&ppcprms.auth_pwd, &pwd_len, stdin);
	
	if(pwd_read <= 1) //ignoring "\n"
		ppcprms.auth_pwd = NULL;
	else
		ppcprms.auth_pwd[pwd_read - 1] = 0; // discarding \n
	
	printf("\nEnter number of authorized uids for the service: ");
	scanf("%u", &ppcprms.auth_uid_len);
	if(ppcprms.auth_uid_len > 100)
		ppcprms.auth_uid_len = 100;
	
	if(ppcprms.auth_uid_len) {
		ppcprms.auth_uid_list = malloc(ppcprms.auth_uid_len * sizeof(uid_t));
		for(int i = 0; i < ppcprms.auth_uid_len; i++)
		{
			printf("Enter authorized uid #%d: ", i);
			scanf("%u", &ppcprms.auth_uid_list[i]);
		}
	}

	printf("Enter number of environment variables for the service: ");
	scanf("%u", &ppcprms.env_len);
	getchar(); // \n discard

	if(ppcprms.env_len > 100)
		ppcprms.env_len = 100;
	
	if(ppcprms.env_len) {
		ppcprms.environ = malloc(ppcprms.env_len * sizeof(char*));
		for(int i = 0; i < ppcprms.env_len; i++)
		{
			ppcprms.environ[i] = NULL;
			char verb[40];
			sprintf(verb, "environment variable #%d", i);

			while(!read_string(&ppcprms.environ[i], verb, 1000));
		}
	}

	int x = makesyscall(&ppcprms);
	printf("%d\n", x);
	
	return 0;
}

