#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in addr1, addr2;
    char *str_ptr;
    char str_arr[20];

    addr1.sin_addr.s_addr=htonl(0x1020304);
    addr2.sin_addr.s_addr=htonl(0x1010101);

    // 第一次调用`inet_ntoa`函数，转换整型地址到点分十进制类型
    str_ptr=inet_ntoa(addr1.sin_addr);
    // 拷贝转换后的地址，以便长期保存
    strcpy(str_arr, str_ptr);
    printf("Dotted-Decimal notation1: %s \n", str_ptr);

    // 再次调用`inet_ntoa`函数，再做一次地址转换
    inet_ntoa(addr2.sin_addr);
    // 验证同一地址上保存的地址已经自动更新
    printf("Dotted-Decimal notation2: %s \n", str_ptr);
    // 验证原来保存过的地址还在
    printf("Dotted-Decimal notation3: %s \n", str_arr);
    return 0;
}