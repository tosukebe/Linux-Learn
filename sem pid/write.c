#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SEM_NAME1  	"/mysemname1"

int main(int argv, char **arg){
	
	int shmid = shmget(100, 4095, IPC_CREAT|0664);
	int cnt = 200;
	sem_t* msem = NULL;            
	
	if(shmid == -1){
		perror("memory create fail");
	}
	
	printf("shmid = %d\r\n",shmid);

	char *ptr = (char*)shmat(shmid, NULL, 0);

	msem = sem_open(SEM_NAME1, O_CREAT, 0664, 0);

	while(cnt){
		
		memset(ptr,'a',10);
		sleep(5);
		sem_post(msem);
		memset(ptr,'b',10);
		sleep(5);
		sem_post(msem);
		memset(ptr,'c',10);
		sleep(5);
		sem_post(msem);
		cnt--;
	}

	shmdt(ptr);

	return 0;
}
