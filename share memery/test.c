#include <stdio.h>
#include <unistd.h>

int main(int argv, char **arg){

    char a = '1';
    int add = 0;
    int cnt = 100;

    while(cnt){
        cnt--;

        if(add>9){
            add = 0;
            a ='1';
        }
        a = a+add;
        add++;
        printf("%c\r\n",a);

        sleep(1);
    }


    return 0;
}