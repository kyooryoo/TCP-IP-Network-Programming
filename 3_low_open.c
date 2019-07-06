// 创建一个测试程序用来演示创建、写入和关闭文件
// 这里文件名用low开头表示low level access file
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
void error_handling(char* message);

int main(void)
{
    int fd;
    char buf[]="Let's go!\n";

    // 这里使用的文件打开模式为创建、只写和清空（原有内容）
    fd=open("data.txt", O_CREAT|O_WRONLY|O_TRUNC);
    if(fd==-1)
        error_handling("open() error!");
    printf("file descriptor: %d \n", fd);

    // 向fd中保存的文件描述所指定的文件写入buf中保存的数据
    if(write(fd, buf, sizeof(buf))==-1)
        error_handling("write() error!");
    close(fd);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}