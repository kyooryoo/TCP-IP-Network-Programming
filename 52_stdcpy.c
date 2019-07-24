#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE 3

int main(int argc, char * argv[])
{
    FILE * fd1, * fd2; // 保存FILE结构体指针
    char msg[BUF_SIZE];

    if(argc!=3) {
        printf("Usage : %s <FROM_FILE> <TO_FILE>\n", argv[0]);
        // exit命令需要stdlib.h
        exit(1);
    }

    fd1=fopen(argv[1], "r");
    fd2=fopen(argv[2], "w");

    while(fgets(msg, BUF_SIZE, fd1)!=NULL)
        fputs(msg, fd2);
    
    fclose(fd1);
    fclose(fd2);
    return 0;
}