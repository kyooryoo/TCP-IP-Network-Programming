#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int cfd1, cfd2;
    char msg1[]="From the 1st copied fildes! \n";
    char msg2[]="From the 2nd copied fildes! \n";
    char msg3[]="From the original fildes! \n";
    char msg4[]="From the original fildes, again! \n";

    // 分别使用两种dup函数复制文件描述符
    cfd1=dup(1);
    cfd2=dup2(cfd1, 7);

    // 使用复制的文件描述符输出，验证复制有效
    printf("fd1=%d, fd2=%d \n", cfd1, cfd2);
    write(cfd1, msg1, sizeof(msg1));
    write(cfd2, msg2, sizeof(msg2));

    // 关闭复制的两个文件描述符
    close(cfd1);
    close(cfd2);
    // 使用原始文件描述符输出，验证其依然有效
    write(1, msg3, sizeof(msg3));
    close(1);
    // 关闭原始的文件描述符后再次尝试从其输出，应该不会成功
    write(1, msg4, sizeof(msg4));
    return 0;
}