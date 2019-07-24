#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fp;

    if(argc!=2) {
        printf("Usage : %s <FILE>\n", argv[0]);
        exit(1);
    }

    // 使用open函数创建或刷新文件，并返回文件描述符
    int fd=open(argv[1], O_WRONLY|O_CREAT|O_TRUNC);
    if(fd==-1) {
        printf("file open error! \n");
        return -1;
    }

    // 打印文件描述符
    printf("original file descriptor: %d \n", fd);
    // 将文件描述符转换为FILE指针，并设置为写入模式
    fp=fdopen(fd, "w");
    // 使用标准输出函数将文本写入FILE指针定义的目标文件
    fputs("Network C programming \n", fp);
    // 将FILE指针转换回文件描述符并打印
    printf("converted file descriptor: %d \n", fd);
    // 使用FILE指针关闭文件，文件描述符也不再有意义
    fclose(fp);
    return 0;
}