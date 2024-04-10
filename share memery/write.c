#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main(int argv, char **arg){
	
	int shmid = shmget(100, 4095, IPC_CREAT|0664);
	int cnt = 200;
	char a = 'a';
	char b = 'b';
	char c = 'c';
	char d = 'd';
	
	if(shmid == -1){
		perror("memory create fail");
	}
	
	printf("shmid = %d\r\n",shmid);

	char *ptr = (char*)shmat(shmid, NULL, 0);

	while(cnt){
		
		memset(ptr,a,10);
		sleep(2);
		memset(ptr,b,10);
		sleep(2);
		memset(ptr,c,10);
		sleep(2);
		memset(ptr,d,10);
		sleep(2);
		cnt--;
	}

	shmdt(ptr);

	return 0;
}
