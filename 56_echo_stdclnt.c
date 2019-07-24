#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char *msg);

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char rmsg[BUF_SIZE], wmsg[BUF_SIZE];

    FILE *readfp;
    FILE *writefp;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    else
        puts("Connected......");

    readfp=fdopen(sock, "r");
    writefp=fdopen(sock, "w");

    while(1)
    {
        fputs("Input message(Q to quit): ", stdout);
        fgets(wmsg, BUF_SIZE, stdin);

        if(!strcmp(wmsg, "q\n") || !strcmp(wmsg, "Q\n"))
            break;

        fputs(wmsg, writefp);
        fflush(writefp);
        fgets(rmsg, BUF_SIZE, readfp);
        
        printf("Message from server : %s", rmsg);
    }
    close(sock);
    return 0;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
