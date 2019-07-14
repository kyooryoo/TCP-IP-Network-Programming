#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <unistd.h>
// 缺少如下引用会有错误 use of undeclared identifier 'IPPROTO_TCP'
#include <arpa/inet.h>
// 缺少如下引用会有错误 use of undeclared identifier 'TCP_NODELAY'
#include <netinet/tcp.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char* argv[])
{
    int tcp_sock, udp_sock;
    int sock_type, sock_sndbuf, sock_rcvbuf, sock_reuseaddr,
        sock_keepalive, sock_broadcast, sock_dontroute,
        sock_oobinline, sock_error;
    int ip_tos, ip_ttl, ip_multicast_ttl, ip_multicast_loop,
        ip_multicast_if, tcp_keepalive, tcp_nodelay, tcp_maxseg;
    socklen_t opt_len;
    int state, opt_val;
    int set_sndbuf=1024*3, set_rcvbuf=1024*3;

    opt_len=sizeof(opt_val);
    // 分别生成TCP和UDP套接字
    tcp_sock=socket(PF_INET, SOCK_STREAM, 0);
    udp_sock=socket(PF_INET, SOCK_DGRAM, 0);
    // 输出创建以上套接字时指定的套接字类型值
    printf("The setting value used in configuration:\n");
    printf("SOCK_STREAM: %d \n", SOCK_STREAM);
    printf("SOCK_DGRAM: %d \n", SOCK_DGRAM);
    printf("The real default value in new Sockets:\n");

    // 获取套接字类型值并打印
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Socket type TCP: %d \n", sock_type);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Socket type UDP: %d \n", sock_type);

    // 查看SOL_SOCKET协议层下的SO_SNDBUF选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&sock_sndbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Send buffer TCP: %d \n", sock_sndbuf);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&sock_sndbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Send buffer UDP: %d \n", sock_sndbuf);

    // 重设SOL_SOCKET协议层下的SO_SNDBUF选项设置并再次查看
    state=setsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&set_sndbuf, sizeof(set_sndbuf));
    if(state)
        error_handling("setsockopt() on SO_SNDBUF of TCP error!");
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&sock_sndbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Send buffer TCP after reset: %d \n", sock_sndbuf);

    state=setsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&set_sndbuf, sizeof(set_sndbuf));
    if(state)
        error_handling("setsockopt() on SO_SNDBUF of UDP error!");
    state=getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&sock_sndbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Send buffer UDP after reset: %d \n", sock_sndbuf);

    // 查看SOL_SOCKET协议层下的SO_RCVBUF选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&sock_rcvbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Receive buffer TCP: %d \n", sock_rcvbuf);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&sock_rcvbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Receive buffer UDP: %d \n", sock_rcvbuf);

    // 重设SOL_SOCKET协议层下的SO_RCVBUF选项设置并再次查看
    state=setsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&set_rcvbuf, sizeof(set_rcvbuf));
    if(state)
        error_handling("getsockopt() error!");
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&sock_rcvbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Receive buffer TCP after reset: %d \n", sock_rcvbuf);
    
    state=setsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&set_rcvbuf, sizeof(set_rcvbuf));
    if(state)
        error_handling("getsockopt() error!");
    state=getsockopt(udp_sock, SOL_SOCKET, SO_RCVBUF, (void*)&sock_rcvbuf, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Receive buffer UDP after reset: %d \n", sock_rcvbuf);

    // 查看SOL_SOCKET协议层下的SO_REUSEADDR选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_reuseaddr, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Reuse addr TCP: %d \n", sock_reuseaddr);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_reuseaddr, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Reuse addr UDP: %d \n", sock_reuseaddr);

    // 查看SOL_SOCKET协议层下的SO_KEEPALIVE选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&sock_keepalive, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Keep alive TCP: %d \n", sock_keepalive);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&sock_keepalive, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Keep alive UDP: %d \n", sock_keepalive);

    // 查看SOL_SOCKET协议层下的SO_BROADCAST选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_BROADCAST, (void*)&sock_broadcast, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Broadcast TCP: %d \n", sock_broadcast);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, (void*)&sock_broadcast, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Broadcast UDP: %d \n", sock_broadcast);

    // 查看SOL_SOCKET协议层下的SO_DONTROUTE选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_DONTROUTE, (void*)&sock_dontroute, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Do not route TCP: %d \n", sock_dontroute);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_DONTROUTE, (void*)&sock_dontroute, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Do Not Route UDP: %d \n", sock_dontroute);

    // 查看SOL_SOCKET协议层下的SO_OOBINLINE选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_OOBINLINE, (void*)&sock_oobinline, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("OOB inline TCP: %d \n", sock_oobinline);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_OOBINLINE, (void*)&sock_oobinline, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("OOB inline UDP: %d \n", sock_oobinline);

    // 查看SOL_SOCKET协议层下的SO_ERROR选项设置
    state=getsockopt(tcp_sock, SOL_SOCKET, SO_ERROR, (void*)&sock_error, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("ERROR TCP: %d \n", sock_error);

    state=getsockopt(udp_sock, SOL_SOCKET, SO_ERROR, (void*)&sock_error, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("ERROR UDP: %d \n", sock_error);

    // 查看IPPROTO_IP协议层下的IP_TOS选项设置
    state=getsockopt(tcp_sock, IPPROTO_IP, IP_TOS, (void*)&ip_tos, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP TOS TCP: %d \n", ip_tos);

    state=getsockopt(udp_sock, IPPROTO_IP, IP_TOS, (void*)&ip_tos, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP TOS UDP: %d \n", ip_tos);

    // 查看IPPROTO_IP协议层下的IP_TTL选项设置
    state=getsockopt(tcp_sock, IPPROTO_IP, IP_TTL, (void*)&ip_ttl, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP TTL TCP: %d \n", ip_ttl);

    state=getsockopt(udp_sock, IPPROTO_IP, IP_TTL, (void*)&ip_ttl, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP TTL UDP: %d \n", ip_ttl);

    // 查看IPPROTO_IP协议层下的IP_MULTICAST_TTL选项设置
    state=getsockopt(tcp_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&ip_multicast_ttl, &opt_len);
    printf("state of getsockopt() for IP_MULTICAST_TTL in TCP is: %d \n", state);
    // if(state)
    //     error_handling("getsockopt() error!");
    printf("IP Multicast TTL TCP: %d \n", ip_multicast_ttl);

    state=getsockopt(udp_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&ip_multicast_ttl, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP Multicast TTL UDP: %d \n", ip_multicast_ttl);
 
     // 查看IPPROTO_IP协议层下的IP_MULTICAST_LOOP选项设置
    state=getsockopt(tcp_sock, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&ip_multicast_loop, &opt_len);
    printf("state of getsockopt() for IP_MULTICAST_LOOP in TCP is: %d \n", state);
    // if(state)
    //     error_handling("getsockopt() error!");
    printf("IP Multicast Loop TCP: %d \n", ip_multicast_loop);

    state=getsockopt(udp_sock, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&ip_multicast_loop, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP Multicast Loop UDP: %d \n", ip_multicast_loop);

    // 查看IPPROTO_IP协议层下的IP_MULTICAST_IF选项设置
    state=getsockopt(tcp_sock, IPPROTO_IP, IP_MULTICAST_IF, (void*)&ip_multicast_if, &opt_len);
    printf("state of getsockopt() for IP_MULTICAST_IF in TCP is: %d \n", state);
    // if(state)
    //     error_handling("getsockopt() error!");
    printf("IP Multicast If TCP: %d \n", ip_multicast_if);

    state=getsockopt(udp_sock, IPPROTO_IP, IP_MULTICAST_IF, (void*)&ip_multicast_if, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("IP Multicast If UDP: %d \n", ip_multicast_if);

    // 查看IPPROTO_TCP协议层下的TCP_KEEPALIVE选项设置
    state=getsockopt(tcp_sock, IPPROTO_TCP, TCP_KEEPALIVE, (void*)&tcp_keepalive, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Keep Alive TCP: %d \n", tcp_keepalive);

    // 查看IPPROTO_TCP协议层下的TCP_NODELAY选项设置
    // 很奇怪，反注释如下代码查看选项属性值后，就无法再修改了
    state=getsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&tcp_nodelay, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("When no-delay (Nagle) is on for TCP: %d \n", tcp_nodelay);
    
    opt_val=1;
    setsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val));
    state=getsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&tcp_nodelay, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("When no-delay (Nagle) is off for TCP: %d \n", tcp_nodelay);

    // 查看IPPROTO_TCP协议层下的TCP_MAXSEG选项设置
    state=getsockopt(tcp_sock, IPPROTO_TCP, TCP_MAXSEG, (void*)&tcp_maxseg, &opt_len);
    if(state)
        error_handling("getsockopt() error!");
    printf("Max Seg TCP: %d \n", tcp_maxseg);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
