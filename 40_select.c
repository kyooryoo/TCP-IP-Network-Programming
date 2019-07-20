#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char msg[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO(&reads);
    FD_SET(0, &reads);

    puts("Input something within 5 seconds.");
    puts("Press Ctrl + C to quit the program.");
    while(1)
    {
        // 每次循环都初始化一个新的文件描述符数组
        temps=reads;
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        // 监视文件描述符数组，只一个读取对象
        result=select(1, &temps, 0, 0, &timeout);
        if(result==-1)
        {
            puts("select() error!");
            break;
        }
        else if(result==0)
            puts("Time out!");
        else
        {
            // 如果在文件描述符数组第一位有更新则触发
            if(FD_ISSET(0, &temps))
            {
                // 读取标准输入，存到msg变量，并返回msg长度
                str_len=read(0, msg, BUF_SIZE);
                // 在msg变量结尾添加0
                msg[str_len]=0;
                printf("message from console: %s", msg);
            }
        }
    }
    return 0;
}