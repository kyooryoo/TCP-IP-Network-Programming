#include <stdio.h>
#include <sys/uio.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    struct iovec vecw[2], vecr[2];
    char msgw1[]="ABCDEFG", msgw2[]="1234567";
    char msgr1[BUF_SIZE]={0,}, msgr2[BUF_SIZE]={0,};
    int str_len;

    // 将要传输的两个字符串分别放到两个iovec类结构体
    // 并分别定义需要传输的字节数，都不是全部传输
    vecw[0].iov_base=msgw1;
    vecw[0].iov_len=3;
    vecw[1].iov_base=msgw2;
    vecw[1].iov_len=4;

    // 准备好保存输入的iovec结构体
    // 第一个成员保存10字节，第二个保存一个缓冲区大小
    vecr[0].iov_base=msgr1;
    vecr[0].iov_len=10;
    vecr[1].iov_base=msgr2;
    vecr[1].iov_len=BUF_SIZE;

    // 输出到标准输出
    // 指定iovec类型结构变量vec的前两个成员
    str_len=writev(1, vecw, 2);
    printf("\nWrite bytes: %d \n", str_len);

    // 从标准输入读取数据并保存到iovec结构体
    puts("Input something for testing:");
    str_len=readv(0, vecr, 2);
    printf("Read bytes: %d \n", str_len);
    printf("First message: %s \n", msgr1);
    printf("Second message: %s", msgr2);
    
    return 0;
}