#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argv, char **arg){
	
	int shmid = shmget(100, 4095, IPC_CREAT|0664);
	
	if(shmid == -1){
		perror("memory create fail");
	}
	
	printf("shmid = %d\r\n",shmid);

	char *ptr = (char*)shmat(shmid, NULL, 0);
	
	memset(ptr,'a',15);

	shmdt(ptr);

	return 0;
}
