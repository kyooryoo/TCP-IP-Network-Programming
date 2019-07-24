#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define BUF_SIZE 3

int main(int argc, char * argv[])
{
    int fd1, fd2; // 保存文件描述符
    int len;
    char msg[BUF_SIZE];

    if(argc!=3) {
        printf("Usage : %s <FROM_FILE> <TO_FILE>\n", argv[0]);
        // exit命令需要stdlib.h
        exit(1);
    }

    // 以下的O_XXX对象需要fcntl.h
    fd1=open(argv[1], O_RDONLY);
    fd2=open(argv[2], O_WRONLY|O_CREAT|O_TRUNC);

    // read，write，和close函数需要unistd.h
    while((len=read(fd1, msg, sizeof(msg)))>0)
        write(fd2, msg, len);
    
    close(fd1);
    close(fd2);
    return 0;
}