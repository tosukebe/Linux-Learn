#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

int main(int argv, char **arc){
	
	int shmid = shmget(100, 0, IPC_CREAT|0664);

	int cnt = 210;
	
	if(shmid == -1){
		perror("read memory error");
	}

	//printf("read shmid = %d\r\n",shmid);

	char *ptr = shmat(shmid, NULL, 0);

	while(cnt){

		printf("%s\r\n",ptr);

		cnt--;
		sleep(1);
	}

	shmdt(ptr);
	shmctl(shmid, IPC_RMID, NULL);
        

	return 0;
}
