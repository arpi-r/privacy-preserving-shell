#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>


#include<sys/syscall.h>
#include<linux/types.h>

struct thread_exec_params {
	long unsigned int *thread_id;
	int thread_num;
	int num_syscalls;
};

long time_taken[1000]; // max no of threads
int n; // number of threads
char* seed;// used in pps_create

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

int makesyscall_ppscreate(struct ppshell_create_params *pif)
{
	int err;	
	__asm(
	    "MOV x8, #551;" //x8 holds syscall no
	    " SVC #0;"      // supervisor call
	    "MOV %[result], x0" : [result] "=r"(err) // copy return code to err variable
	);
	return err; 
}


// The function to be executed by all threads for create
void *call_create(void *vargp)
{
	// Store the value argument passed to this thread
	struct thread_exec_params *p = (struct thread_exec_params *) vargp;
	long unsigned int *tid;
	tid = p->thread_id;
	
	struct timespec start, finish;
	
	int m = p->num_syscalls;
	int tnum = p->thread_num;
	
	struct ppshell_create_params ppcprms[m+2];
	
	int i;
	for (i = 0; i < m; i++) {
		ppcprms[i].size = sizeof(struct ppshell_create_params);
		ppcprms[i].name = (char*) malloc(100);
		ppcprms[i].description = (char*) malloc(100);
		ppcprms[i].command = (char*) malloc(100);
		
		sprintf(ppcprms[i].name, "perftest-%s-%d", seed, tnum*m + i);
		sprintf(ppcprms[i].description, "perftest");
		sprintf(ppcprms[i].command, "echo %d && whoami", tnum*m + i);
		
		ppcprms[i].auth_pwd = "mypwd";
		ppcprms[i].auth_uid_list = NULL;
		ppcprms[i].environ = NULL;
		ppcprms[i].auth_uid_len = ppcprms[i].env_len = 0;
	}
	
	clock_gettime(CLOCK_MONOTONIC, &start);
  	for (i = 0; i < m; i++) {
		makesyscall_ppscreate(&ppcprms[i]);
		//printf("SYSCALL %d %d : %d\n", tnum, i, ret);
	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	
	long tnsec = 0;
	if ((finish.tv_nsec-start.tv_nsec)<0)
	{
	    tnsec = (finish.tv_sec-start.tv_sec-1)*1000000000;
	    tnsec += 1000000000+finish.tv_nsec-start.tv_nsec;
    	}	
    	else 
    	{
	    tnsec = (finish.tv_sec-start.tv_sec)*100000000;
	    tnsec += finish.tv_nsec-start.tv_nsec;
    	}
	
	time_taken[tnum] = tnsec;
	
	printf("Thread ID: %ld and tnum = %d and m = %d and time = %ld\n", *tid, tnum, m, time_taken[tnum]);
}



int main(int argc, char *argv[])
{

	if (argc != 5) {
		printf("Usage: ./measure_perf <pps_<syscall_to_test>> <no_of_threads> <no_of_calls_per_thread> <seed>\n(<pps_<syscall_to_test>> can be pps_create, pps_call and pps_list)\n");
		exit(1);
	}
	
	char *syscall_to_test = argv[1];
	seed = argv[4];
	
	n = atoi(argv[2]);
	int m = atoi(argv[3]);
	
	printf("syscall = %s\nseed = %s\nn = %d\nm = %d\n", syscall_to_test, seed, n, m);
	
	int i;
	pthread_t tid[n];

	if (strncmp(syscall_to_test, "pps_create", 10) == 0) {
		for (i = 0; i < n; i++) {
			struct thread_exec_params *p = (struct thread_exec_params *) malloc(sizeof(struct thread_exec_params));
			p->thread_id = &tid[i];
			p->num_syscalls = m;
			p->thread_num = i;
			pthread_create(&tid[i], NULL, call_create, (void *)p);
		}
	}
	
	else {
		printf("syscall does not exist!\n");
		exit(1);
	}
	
	
	long tot_time = 0;
	for (i = 0; i < n; i++) {
		pthread_join(tid[i], NULL);
		printf("Thread %d (%ld) : time %ld\n", i, tid[i], time_taken[i]);
		tot_time += time_taken[i];
	}
	long avg_time = tot_time/(n*m);
	printf("Tot time(nsec): %ld , average time(nsec): %ld\n", tot_time, avg_time);
	
	return 0;
}
