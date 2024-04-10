#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#define __USE_GNU
#include <sched.h>
#include <pthread.h>

char *ptr = NULL;
sem_t msem;
int cpu_num;

/*
void* mwrite2(){

    while(1){
    memset(ptr, 'b', 12);
    printf("write2\r\n");
    sleep(1);
    }
}
*/

void* mwrite1(){

    cpu_set_t mask;
    cpu_set_t get;

    CPU_ZERO(&mask); 
    CPU_SET(0,&mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1){
        perror("write cpu affinity set fail");
    }

    printf("write1 pid = %d\r\n",getpid());

    while(1){

    CPU_ZERO(&get);
    if(sched_getaffinity(0, sizeof(get), &get) == -1){
        perror("write cpu affinity get fail");
    }

    for(int i=0;i<cpu_num;i++){
        if(CPU_ISSET(i,&get)){
            printf("wirte1 thread is running in cpu %d\r\n", i);
        }
    }

    memset(ptr, 'a', 12);
    printf("write1\r\n");
    sem_post(&msem);
    sleep(1);
    }
}

void* mread(){

    cpu_set_t get;
    cpu_set_t mask;

    CPU_ZERO(&mask);
    CPU_SET(0,&mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) == -1){
        perror("read cpu affinity set fail");
    }

    printf("read pid = %d\r\n",getpid());

    while(1){

    CPU_ZERO(&get);
    if(sched_getaffinity(0, sizeof(get), &get) == -1){
        perror("read cpu affinity get fail");
    }

    for(int i=0;i<cpu_num;i++){
        if(CPU_ISSET(i,&get)){
            printf("read thread is running in cpu %d\r\n", i);
        }
    }

    sem_wait(&msem);
    //printf("%s\r\n",ptr);
    printf("read1\r\n");
    sleep(1);
    }
}

void set_thread_policy_priority(pthread_attr_t *thr_shed_attr, int priority){
    
    int policy;
    struct sched_param thr_sched_param;
    int status;

    status = pthread_attr_init(thr_shed_attr);
    if(status != 0){
        perror("pthread_attr_init error");
    }

    status = pthread_attr_setscope(thr_shed_attr, PTHREAD_SCOPE_SYSTEM);
    if(status != 0){
        perror("pthread_attr_setscope error");
    }

    policy = SCHED_FIFO; // FIFO=1, RR=2
    status = pthread_attr_setschedpolicy(thr_shed_attr, policy);  
    if(status != 0){
        perror("pthread_attr_setschedpolicy error");
    }

    thr_sched_param.sched_priority = priority;
    status = pthread_attr_setschedparam(thr_shed_attr, &thr_sched_param);
    if(status != 0){
        perror("pthread_attr_setschedparam error");
    }

    /*
    继承调度属性 inheritsched：当我手动设置了调度策略或优先级时，必须显示的设置
    线程调度策略的inheritsched属性，因为pthread没有为inheritsched设置默认值。
    所以在改变了调度策略或优先级时必须总是设置该属性。
    pthread_attr_setinheritsched()的参数如下:
    PTHREAD_INHERIT_SCHED 或者 PTHREAD_EXPLICIT_SCHED。
    前者为继承创建线程的调度策略和优先级，后者指定不继承调度策略和优先级，而是使用
    自己设置的调度策略和优先级。无论何时，当你需要控制一个线程的调度策略或优先级时，
    必须将inheritsched属性设置为PTHREAD_EXPLICIT_SCHED。
    */
    status = pthread_attr_setinheritsched(thr_shed_attr, PTHREAD_EXPLICIT_SCHED);
    if(status != 0){
        perror("pthread_attr_setinheritsched error");
    }
}

int main(int argv, char **arg){

    pthread_attr_t thr_shed_attr;
    struct sched_param thr_shed_param;
    int status = 0;
    int thr_policy;

    status = pthread_attr_init(&thr_shed_attr);
    if(status != 0){
        perror("pthread_attr_init error");
    }

    status = pthread_attr_getschedpolicy(&thr_shed_attr, &thr_policy);
    if(status != 0){
        perror("pthread_attr_getschedpolicy error");
    }
    printf("policy = %d\r\n",thr_policy);

    status = pthread_attr_getschedparam(&thr_shed_attr, &thr_shed_param);
    if(status != 0){
        perror("pthread_attr_getschedparam error");
    }
    printf("sched_priority = %d\r\n",thr_shed_param.sched_priority);

    // after change thr_shed_attr
    set_thread_policy_priority(&thr_shed_attr, 1);

    status = pthread_attr_getschedpolicy(&thr_shed_attr, &thr_policy);  
    if(status != 0){
        perror("after pthread_attr_getschedpolicy error");
    }
    printf("after policy = %d\r\n",thr_policy);

    status = pthread_attr_getschedparam(&thr_shed_attr, &thr_shed_param);
    if(status != 0){
        perror("after pthread_attr_getschedparam error");
    }
    printf("after sched_priority = %d\r\n",thr_shed_param.sched_priority);

    // get number of cpu
    cpu_num = sysconf(_SC_NPROCESSORS_ONLN);
    printf("cpu num = %d\r\n", cpu_num);

    // get pid
    printf("main pid = %d\r\n",getpid());


    //pthread_t thr_w2 = 0;
    ptr = (char *)malloc(20*sizeof(char));
    int ret = 0;
    pthread_t thr_w1 = 0;
    pthread_t thr_r = 0;

    // init sem
    sem_init(&msem, 0, 0);

/*
    ret = pthread_create(&thr_w2, NULL, mwrite2, NULL);
    if (ret != 0) {
        printf("pthread_read failed \n");
        return -1;
    }
*/

    ret = pthread_create(&thr_w1, NULL, mwrite1, NULL);
    if (ret != 0) {
        printf("pthread_write1 failed \n");
        return -1;
    }

    ret = pthread_create(&thr_r, NULL, mread, NULL);
    if (ret != 0) {
        printf("pthread_read failed \n");
        return -1;
    }

    printf("write thread id = %lu, read thread id = %lu\r\n", thr_w1, thr_r);

    while(1){

    }

    return 0;
}