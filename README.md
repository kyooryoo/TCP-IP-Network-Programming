# TCP-IP-Network-Programming

笔记《TCP/IP 网路编程》-（韩）尹圣雨

学习网络编程所需的基础知识：操作系统，系统编程，TCP/IP

控制台输入输出和文件输入输出非常相似，也与网路编程相似。网络编程就是编写程序以便两台连网的计算机相互交换数据。操作系统提供套接字（socket）组件以便计算机通过网络传输数据。Linux系统对待文件与Socket没有区别，用于处理文件I/O的函数可以直接用来处理Socket，而Windows系统中需要使用特殊的数据传输相关函数处理Socket。

## 套接字类型与协议设置

文件描述符号
C语言或Linux系统使用相同的文件描述符号。`0`表示标准输入，`1`表示标准输出，`2`表示标准错误。之后创建的文件会使用其他数字，按递增顺序排序。

底层方式访问文件（low level access file）
指不依赖第三方的标准或代码库定义的函数，使用操作系统本身提供的功能访问文件。

创建套接字
```
int socket(int domain, int type, int protocol);
```
* domain：套接字中使用过的协议族（Protocol Family）信息
* type：套接字数据传输类型
* protocol：协议信息

协议族（Protocol Family）
指协议的分类信息，包括以下几种：
* PF_INET	IPv4互联网协议族
* PF_INET6	IPv6互联网协议族
* PF_LOCAL	本地通信的UNIX协议族
* PF_PACKET	底层套接字的协议族
* PF_IPX		IPX Novell协议族

套接字类型（Type）
指套接字的数据传输方式，包括如下几种：
* SOCK_STREAM	面向连接的套接字
* SOCK_DGRAM	面向消息的套接字
面向连接的套接字可以保证数据在传输过程中不丢失，按顺序发送和接受，由于数据包不存在严格边界发送和接受操作不需要严格对应，但由于连接本身的强制要求发送方和接收方必须一一对应。有些类似于TCP协议，但不完全相同。
面向消息的套接字强调快速传输，传输顺序不固定且数据包有可能丢失或损毁，由于数据包存在边界所以每次传输的数据大小存在限制且发送和接受操作严格的一一对应，切不存在连接的概念。有些类似于UDP协议，但也不完全相同。

最终协议
如果前两个参数即可决定采用的协议，第三个参数可以用0。但如果同一协议族中存在多个传输方式相同的协议，则需要通过第三个参数指定协议信息。例如如下两组创建套接字的函数是相同的，因为分别只有一种对应的协议：
```
* int tcp_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int tcp_socket = socket(PF_INET, SOCK_STREAM, 0);
* int udp_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  int udp_socket = socket(PF_INET, SOCK_DGRAM, 0);
```

文件打开模式
打开文件或套接字的open函数支持以下文件打开模式：
O_CREAT		需要时创建文件
O_TRUNC		删除现有数据
O_APPEND	添加文件内容
O_RDONLY	只读
O_WRONLY	只写
O_RDWR		读写

编写一个测试用的服务器端socket程序，用`Hello!`响应客户端请求。
```
$ gcc 1_hello_server.c -o hserver
$ ./hserver 9190
```
服务器端程序运行后进入待机状态，等待来自客户端的请求以便响应。

编写一个测试用的客户端socket程序，访问服务器并验证得到响应。
```
$ gcc 2_hello_client.c -o hclient
$ ./hclient 127.0.0.1 9190
Message from server : Hello!
```
以上客户端请求完成后，服务器端和客户端程序都会终止。且服务器端暂时不可再次运行程序。

编写一个测试用的创建、写入和关闭文件的小程序，并验证文件内容。
```
$ gcc 3_low_open.c -o lopen
$ ./lopen
file descriptor: 3 
$ cat data.txt 
Let's go!
```
相应编写一个测试用读取文件的小程序，并验证可用。
```
$ gcc 4_low_read.c -o lread
$ ./lread 
file descriptor: 3 
file data: Let's go!
```

编写一个测试文件描述符命名规则的小程序。
```
$ gcc 5_fd_seri.c -o fds
$ ./fds
file descriptor 1: 3
file descriptor 2: 4
file descriptor 3: 5
```

更新之前的客户端程序，编写验证TCP套接字无边界的新客户端程序，并验证。
```
// 在一个终端中运行服务器端Socket程序
$ ./hserver 9190
// 在另一个终端中运行新的TCP套接字客户端程序
$ gcc 6_tcp_client.c -o tclient
$ ./tclient 127.0.0.1 9190
Message from server : Hello! 
Function read call count: 7 
```

## 地址族与数据序列

补充一些基本网络知识。IPv4使用32位二进制数表示IP地址，即计算机在网络上的地址。32位二进制数分成四组，每组8位，即一个字节，可取值从0到255。常用的有A、B、C、D四类地址，分别使用第一字节，前两字节，前三字节做网络地址，余下的字节做主机地址，D类地址为多播IP地址。网络地址是标识一个网络的地址部分，发送到主机的数据会先按照网络地址路由并发送到所属的网络，再有所属网络的路由器或交换机根据主机地址传送数据到主机。根据业界规定，A类地址首字节取值在0到127之间，B类地址在128到191之间，C类在192到223之间，或者说A类地址的二进制形式首位为0，B类为10，C类为110，因此根据ID地址首字节或首位数值可以快速判断IP地址类型，进而识别网络地址和主机地址。

其次是端口号，一台主机上可能有多个程序和服务在运行，要区别同一主机上不同应用的套接字需要使用端口号。端口号由16位二进制数构成，可分配范围为0到65535，其中0到1023是系统保留的端口，分配给非系统级应用使用其他端口。TCP和UDP协议套接字可以重复使用一个端口，相同协议下的套接字不可以使用同一个端口。

`sockaddr_in`
在使用`socket`函数创建套接字后，需要使用`bind`函数分配地址族、IP地址、端口等，使用如下方式读入参数：
```
bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)
```
以上参数中，最重要的是第二个`sockaddr`参数，其中包含了地址族、IP地址和端口号信息，具体内容如下：
```
struct sockaddr 
{
    sa_family_t		sin_family;	//地址族
    char		sa_data[14];	//地址信息
}
```
以上`sockaddr`结构型数据的属性`sa_data`需要包含IP地址和端口号，不足14位部分填充0。直接创建该属性值有些麻烦，故先创建一个新的结构型数据`sockaddr_in`，再将其转换为`sockaddr`的形式传递给`bind`函数。

`sockaddr_in`结构类型数据的定义如下：
struct sockaddr_in
{
    sa_family_in	sin_family;	//地址族（Address Family）
    uint16_t		sin_port;	//16位TCP/IP端口号
    struct in_addr	sin_addr;	//32位IP地址
    char		sin_zero[8];	//用于数据形式合规	 
}
以上结构类型数据中包含另一个结构类型的`in_addr`数据，用来存放32位IP地址：
struct in_addr
{
    in_addr_t		s_addr;		//32位IPv4地址
}

以上的数据类型中有一些是POSIX（Portable Operating System Interface，可移植操作系统接口）定义的，其他的类似由POSIX位UNIX系统设立的数据类型还有：
```
数据类型		说明				声明的头文件
int8(16/32)_t	带符号的8（16或32）位整型		sys/types.h
uint8(16/32)_t	不带符号的8位（16或32）位整形		sys/types.h
sa_family_t	地址族				sys/socket.h
socketlen_t	结构型数据的长度			sys/socket.h
in_addr_t	IP地址，实际数据类型为uint32_t	netinet/in.h
in_port_t	端口号，实际数据类型为uint16_t	netinet/in.h
```
使用以上预定义的数据类型，可以保证在任何情况下相应数据的结构符合规范，比如IPv4地址总是占32位，不会因为头部有0而减少位数。

结构型数据`sockaddr_in`中定义的各属性值说明如下：
* sin_family - 地址族
根据协议族的不同，适用的地址族也有不同。可取值为`AF_INET`、`AP_INET6`或`AF_LOCAL`，分别表示IPv4网络协议使用的地址族、IPv6网络协议使用的地址族或本地通信中采用的UNIX协议地址族。
* sin_port - 16为端口号
重点是以网络字节序保存（后面解释）
* sin_addr - 32位IPv4地址信息
也是以网络字节序保存，在结构型数据`in_addr`中定义为`in_addr_t`即实际为`unit32_t`类型，可看作32位整数。
* sin_zero - 用于数据形式合规
为保证结构型数据`sockaddr_in`与`sockaddr`保持一致，将不足的位数补充为0。

网络字节序
不同架构的CPU向内存保存数据的方式有大端序（Big Endian）和小端序（Little Endian）两种，分别将数据的高位字节放到低位地址或高位地址，即数据的逻辑形式和在不同类型CPU的电脑内存中的保存形式（Host Byte Order 主机字节序）可能不同。为避免发送和接受方电脑由于主机字节序不同发生的通讯故障，规定网络传输中使用的字节序（Network Byte Order）统一为大端序。

在将IP地址和端口号参数放入`sockaddr_in`结构型数据前需要先转换为网络字节序，为此可以使用如下函数：
* 函数名称	说明		数据类型
* htons		主机到网络		无符号短整型
* ntohs		网络到主机		无符号短整型
* htonl		主机到网络		无符号长整型
* ntohl		网络到主机		无符号长整型
以上函数名称中，`h`代表`host`，`to`表示转换方向，`n`代表`network`，`s`和`l`分别代表短整型（占用2个字节，用于表示端口号）或长整型（占用4个字节，用于表示IP地址）数据类型。

这里编写一个测试字节序转换的小程序，验证如下：
```
$ gcc 7_endian_conv.c -o conv
$ ./conv
Host ordered port: 0x1234 
Network ordered port: 0x3412 
Host ordered address: 0x12345678 
Network ordered address: 0x78563412 
```
以上结果是在小端序CPU上运行的结果。由于大部分Intel和AMD的CPU都使用小端序标准，因此大部分人都会得到同样结果。除了向`sockaddr_in`结构型数据填充变量外，其他场合的转换过程是系统自动完成的，不需要考虑字节序问题。

网络地址转换
`bind`函数接受的`sockaddr_in`结构型数据中保存的IP地址为32为整型，不是大部分用户日常所熟悉的点分十进制（Dotted Decimal Notation）类型。可以使用如下函数转换网络地址并校验其合法：
```
#include <apra/inet.h>
in_addr_t inet_addr(const char * string);
```
以上函数在成功时返回32位大端序整型数值，失败时返回INADDR_NONE。

这里编写一个小程序使用入上函数验证地址转换和校验功能：
```
$ gcc 8_inet_addr.c -o addr
$ ./addr
Network ordered integer addr: 0x101a8c0 
Error occured! 
```

在实际实施中使用的`inet_aton`函数与上述的`inet_addr`函数功能相同，只是在转换IP地址外还自动保存了转换后的地址到指定的`sockaddr_in`结构型数据中的`in_addr`结构型数据中，省去了手工操作的麻烦。使用方法如下：
```
#include <arpa/inet.h>
int inet_aton(const char * string, struct in_addr * addr); 
```
以上函数在成功时返回1，失败时返回0。参数中，`string`是需要转换的字符串类型的点分十进制IP地址，`addr`是转换结果将要保存到的`in_addr`结构型数据的地址。

这里编写一个小程序，调用`inet_aton`函数并验证结果：
```
$ gcc 9_inet_aton.c -o aton
$ ./aton
Network ordered integer addr: 0x4f7ce87f 
```

与`inet_aton`相反，`inet_ntoa`函数可以将网络字节序的整型IP地址转换成点分十进制的易读形式。
```
#include <arpa/inet.h>
char * inet_ntoa(struct in_addr adr);
```
以上函数在成功时返回转换结果的字符串地址值，失败时返回-1。主要注意，该函数的返回类型为char指针，字符串地址保存的内存空间由系统自动分配，如果不保存到其他位置则有可能丢失，例如再次调用该函数时新的返回值会覆盖原有的返回值。

这里同样编写一个小程序验证函数`inet_ntoa`对地址的转换。
```
$ gcc 10_inet_ntoa.c -o ntoa
$ ./ntoa
Dotted-Decimal notation1: 1.2.3.4 
Dotted-Decimal notation2: 1.1.1.1 
Dotted-Decimal notation3: 1.2.3.4 
```

网络地址初始化
为创建服务器端的套接字，需要使用如下方法对网络地址做初始化设置：
```
struct sockaddr_in addr;
char * serv_ip = "211.217.168.13"	//声明IP地址
char * serv_port = "9190"		//声明端口号
memset(&addr, 0, sizeof(addr));		//将结构类型变量addr的所有属性值初始化为0
addr.sin_family = AF_INET;		//指定地址族
addr.sin_addr.s_addr = inet_addr(serv_ip);	//IP地址初始化
addr.sin_port = htons(atoi(serv_port))l;	//端口号初始化
```

对照之前服务器端的套接字程序，实际初始化IP地址使用的语句如下：
```
serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
```
这里，使用的`INADDR_ANY`为一个常数，系统自动获得服务器端的IP地址并保存在这里，如果同一服务器上分配了多个IP地址，只要套接字使用的端口号一致，就可以使用该参数从不同IP地址接受数据。除非客户端带有类似的服务器功能，否则不可以使用该参数。

以上对包含服务器信息的结构型变量`addr`配置了协议族、IP地址和端口参数后，可以使用`bind`函数消费该结构性变量，配置服务器端的套接字：
```
#include <sys/socket.h>
int bind(int sockfd, struct sockaddr * addr, socklen_t addrlen); 
```
其中，`sockfd`为之前创建现在需要配置的套接字文件描述符，`addr`即包含服务器地址信息的结构型变量，`addlen`即第二个参数结构型变量的长度。

回顾之前创建服务器端套接字的程序，涉及初始化的部分代码如下：
```
int serv_sock;
struct sockaddr_in serv_addr;

// 创建服务器端套接字
serv_sock=socket(PF_INET, SOCK_STREAM, 0);


// 服务器地址信息初始化
memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family=AF_INET;
serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
// 端口由运行命令时的第一个参数指定
serv_addr.sin_port=htons(atoi(argv[1]));

// 分配IP地址和端口号
if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
```

至此对之前建立的套接字的服务器端和客户端程序都已经解释完成。回顾执行服务器和客户端的命令如下：
```
$ ./hserver 9190
$ ./hclient 127.0.0.1 9190
```
服务器端程序因为使用了`INADDR_ANY`可以自动分配IP地址，只需要指定端口号即可。客户端程序的第一个参数为本地回环IP地址，因为服务器运行在本地，如果运行在其他电脑也可以改为相应的IP地址，后面接相同的在服务器端程序中指定的端口。

## 基于TCP的S/C

之前解释了创建套接字和分配地址，这一部分开始讨论面向连接的服务器和客户端（S/C）。TCP套接字面向连接且基于流（stream），对数据传输有更多控制（TCP Transmission Control Protocol）。根据四层网络TCP/IP协议栈模型（由下到上：链路层、IP层、TCP层/UDP层、应用层），TCP或UDP位于第三层，即IP层和应用层之间。分层的目的是为了把网络通讯这个复杂问题化小，并通过逐个标准化设计开放式系统。所谓开放式系统，就是制定行业标准并公开，以便各个厂家遵守以生产可以相互兼容的设备。

这里补充一下OSI的7层网络模型，由下到上分别为物理层、数据链路层、网络层、传输层、会话层、表示层、应用层，IP协议工作在自下而上的第三层网络层，负责网络地址的分配和管理，TCP协议工作在第四层传输层，负责对数据传输过程的控制和管理。

回到四层模型，链路层对网络的物理连接做标准化，对应局域网LAN(Local Area Network)、广域网WAN(Wide)和城域网MAN(metropolitan)等标准。IP层管理传输中的路径，TCP或UDP层完成实际的数据传输。IP层本身对数据的传输是不可靠的，数据包有可能不按顺序抵达，或有丢失和损毁。TCP层负责重新封装数据、校验数据完整性、有必要则重新传送部分数据。在最上层的应用层就是套接字，该层的协议的应用是网络编程的主要内容，对低层TCP和IP协议的调用都已经包含其中。

### 服务器端流程

基于TCP的服务器端套接字创建和数据传输过程调用如下函数：
```
socket()	创建套接字
bind()		分配套接字地址
listen()	等待连接请求
accept()	接受连接请求
read()/write()	读写数据
close()		断开连接
```
之前的部分已经介绍了创建套接字和分配地址的`socket()`和`bind()`函数，以下介绍剩余部分的函数。具体可以结合参考`hello_server.c`中的实际代码阅读以下的各个函数说明部分。

等待连接请求
在服务器端为套接字分配了地址后，调用`listen()`函数进入等待连接请求的状态，之后客户端才可以调用`connect()`函数发出连接请求，如果提前调用会发生客户端错误。`listen()`函数的调用方法如下：
```
#include <sys/socket.h>
int listen(int sock, int backlog); 
```
* 以上函数成功返回0，失败返回-1。
* sock：需要等待连接请求的服务器端套接字文件描述符
* backlog：连接请求队列长度

接受连接请求
等待连接请求和接受连接请求需要不同的套接字来完成，可以理解为保安和收发室的各司其职的关系。这里接受套接字使用的函数`accept()`可以自动创建套接字并连接到发起请求的客户端。
```
#include <sys/socket.h>
int accept(int sock, struct sockaddr * adds, socklen_t * addrlen);
```
* 该函数成功返回创建的套接字文件描述符，失败时返回-1
* sock：服务器套接字的文件描述符
* addr：保存发起连接请求的客户端地址信息
* addrlen：客户端地址的结构型数据长度
这里需要说明一下，因为该`accept()`函数的参数与`bind()`函数相同，但意义有所不同。`bind()`函数读入的第二个和第三个参数是本地配置好的服务器地址信息，是需要做读取操作的对象，用于应用到服务器套接字上。而`accept()`函数的第二个和第三个参数是用来存放接受客户端请求后拿到的客户端的地址信息，是需要做保存操作的对象。

读写数据和关闭连接
这里使用写数据作为示例，调用方法如下：
```
int write(int sock, char* msg, int msg_len);
close(int sock)
```
* `write()`函数成功返回写入的字符数，失败返回-1
* sock：之前由`accept()`函数创建的服务器端套接字
* msg：需要传输的字符串
* msg_len：需要传输的字符串长度

### 客户端流程

基于TCP的客户端套接字创建和数据传输过程调用如下函数：
```
socket()	创建套接字
connect()	请求连接
read()/write()	读写数据
close()		断开连接
```
相对服务器端，客户端的操作相对简单，主要是使用`connect()`函数发起连接请求。对于以下函数说明可以结合`hello_client.c`文件中的具体代码以加深理解。
```
#include <sys/socket.h>
int connect(int sock, struct sockaddr* servaddr, socklen_t addrlen);
```
* 以上函数成功返回0，失败返回-1
* sock：客户端套接字文件描述符
* servaddr：保存服务器端地址信息的结构型变量
* addrlen：服务器端地址信息的长度
与服务器端的`accept()`函数类似，第二和第三个参数分别用户保存接受到的连接对方，这里也就是服务器端，的地址信息及其长度。在`connect()`函数调用后，只有服务器端接受连接请求或因为断网等原因中断了连接请求时才会返回，受到连接请求队列长度的限制，服务器端可能不会立即调用`accept()`函数而是把请求放入等待队列。其他函数`read()`和`close()`与服务器方面的类似，这里不再赘述。

### 回声服务器

目前的`hello_server.c`服务器程序返回固定的字符串，功能过于简单。这里更新代码强化其功能，接受客户端发来的信息并返回，流程如下：
* 服务器连接客户端
* 客户端接受用户输入并发送到服务器
* 服务器接受客户端传输的数据并原样返回客户端
* 服务器保持连接直到客户端输入`Q`
这里需要注意的是，服务器在同一时间只能接受一个客户端的连接请求，且服务器对每个连接可以响应五个请求，之后就会退出。

更新后另存服务器程序文件为`echo_server.c`，主要更新的代码部分如下：
```
    // 在每次服务器活动期间，可接受五次来自客户端的连接请求
    for(i=0; i<5; i++)
    {
        // 接受客户端的连接亲求，并创建用于数据传输的服务器端套接字
        clnt_sock=accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
        if(clnt_sock==-1)
            error_handling("accept() error!");
        else
            printf("Connected client %d \n", i+1);

        // 读入客户端发送的信息并原样返回
        while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
            write(clnt_sock, message, str_len);
        
        close(clnt_sock);
    }
```

更新客户端程序文件并另存为，主要更新部分如下：
```
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error!");
    else
        // 在套接字连接接受后，显示连接成功的消息
        puts("Connected......");

    while(1)
    {
        // 注意这里使用的`fputs()`和`fgets()`函数
        // 分别向标准输出打印信息和从标准输入读取信息
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        // 在用户输入大小写Q之后终止运行
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        
        // 将用户的输入写入与服务器连接的套接字再读取和打印服务器的回声
        write(sock, message, strlen(message));
        str_len=read(sock, message, BUF_SIZE-1);
        if(str_len==-1)
            error_handling("read() error!");
        // 在读取的服务器回声信息末尾添加休止符`0`
        message[str_len]=0;
        printf("Message from server : %s \n", message);
    }
    close(sock);
```

编译服务器与客户端代码并运行查看结果：
```
$ gcc 11_echo_server.c -o eserver
$ ./eserver 9091
Connected client 1 
Connected client 2 
Connected client 3
```
以上每个连接的客户端表示一次打开又关闭的套接字，即客户端请求连接、成功连接、按下Q或q终止连接。直到先后服务五个客户端，服务器会一直工作，在服务满五个客户端后终止运行。

注意服务器端运行后就无法继续操作终端，客户端需要在独立终端中分别运行：
```
$ gcc 12_echo_client.c -o eclient
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): Good morning
Message from server : Good morning
```
连接建立后，客户端用户可以一直输入并得到来自服务器的回声反馈，直到按下Q或q终止程序的执行。在服务器终止运行之前，用户可以发起和终止五次基于套接字的数据传输。之后需要重新运行服务器端程序，才能再建立连接。

理论上，以上代码存在问题隐患。即由于采用TCP方式连接，基于TCP传输的数据无边界的特性，客户端发送的多个字符串可能因为同时抵达服务器而被当作一个字符串，或者服务器端会将大的数据拆分成多个数据包返回客户端，结果时客户端收到的服务器回声可能包含多个原始字符串或某个原始字符串的片段。当服务器与客户端都在本地电脑，且测试字符串较小时不会发生，但仍然是一个技术隐患，需要在后面做出改善和解决。

## 更新TCP/IP的S/C

对于之前提到的客户端接收信息可能遇到的问题，这里通过预知接收数据的大小定义读取逻辑可以解决。具体更新客户端程序中，增加两个`int`变量`recv_len`和`recv_cnt`，并循环代码块如下：
```
    while(1)
    {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        
        // 记录下发送的字符串长度
        str_len=write(sock, message, strlen(message));
        // 初始化记录接收到的字符串总长度的变量为0
        recv_len=0;
        // 在没有读取到全部发送内容前始终保持继续读取操作
        while(recv_len<str_len)
        {
            // 记录每次读取到的字符数
            recv_cnt=read(sock, &message[recv_len], BUF_SIZE-1);
            if(recv_len==-1)
                error_handling("read() error!");
            // 累计每次读取到的字符数到接收的字符数总和
            recv_len+=recv_cnt;
        }
        message[recv_len]=0;
        printf("Message from server : %s \n", message);
    }
```
以上用于反复读取操作的内循环条件判断语句使用`<`条件，因为考虑到可能发生的数据传输异常会造成读取的总字符数（因为包含乱码等）超过发送的字符数，如果使用`!=`作为条件可能永远无法达到而无限循环下去。验证与之前的程序类似，过程省略。

### 应用层协议

在一般应用中，从服务器可以拿到的数据量可能无法预估，为避免类似问题出现需要在应用层定义数据边界。这里举例一个简单的问题，由客户端输入几个整数并指定运算方法，服务器端负责运算和返回结果。为保持简单，只做加减乘运算，不做除法以避免非整数型数据的出现。这里就涉及到了数据边界的定义问题，具体流程如下：
* 客户端连接服务器
* 以1字节整数形式保存待计算的数字个数
* 以4字节为单位保存每个待计算的数字
* 以1字节保存运算符
* 客户端发送以上运算信息到服务器
* 服务器完成运算，用4字节保存结果并返回客户端
* 客户端显示结果并终止与服务器的连接

具体代码和说明可以参考`15_op_client.c`和`14_op_server.c`文件及其内部的注释。代码的编译和运算可以参考如下实例。
```
// 在一个终端中运行服务器端程序
$ gcc 14_op_server.c -o opserver
$ ./opserver 9091
// 在另一个终端中运行客户端程序
$ gcc 15_op_client.c -o opclient
$ ./opclient 127.0.0.1 9091
Connected......
Operand count: 2
Operand 1: 12
Operand 2: 13
Operator: +
Operation result: 25 
$ ./opclient 127.0.0.1 9091
Connected......
Operand count: 2
Operand 1: 12
Operand 2: 13
Operator: -
Operation result: -1 
$ ./opclient 127.0.0.1 9091
Connected......
Operand count: 2
Operand 1: 12
Operand 2: 13
Operator: *
Operation result: 156 
```
以下挑选客户端和服务器程序代码中有趣的部分出来单独看看。

客户端程序
```
for(i=0; i<opnd_cnt; i++)
    {
        printf("Operand %d: ", i+1);
        // 以4字节为单位保存待计算数字，转换为整型指针
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);
    }
```
这里读入的每个待计算数字是强制转换为整型指针存入数组的，保存空间都是4个字节大小，在数组中的位置由循环体的内部变量i定位和调整。这里的整型指针变量需要进一步的解释，详见本文的备注部分。

```
    // 删掉缓冲中的字符`\n`
    fgetc(stdin);
```
以上函数从客户端程序截取，在用户输入需要计算的数字之后，在用户需要输入运算操作符之前。如果没有这一行代码，用户在提示需要输入运算操作符的位置无法完成输入，程序会自动跳转到输出运算结果的部分，结果并不会进行任何运算，而是直接输出了第一个待计算的数字。

服务器程序
```
while((opnd_cnt*OPSZ+1)>recv_len)
{
    recv_cnt=read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);
    recv_len+=recv_cnt;
}
```
以上循环在接收到的运算信息总长度小于预期的信息长度前持续保持读取。这里可以注意到预期的信息长度是使用待算数字个数乘以4加1得到，也就是运算信息中包含运算数字和运算符号，但不包含待算数字个数信息。查看之前的代码，待算数字个数信息已经在该循环开始之前得到，且该信息会被用来计算预期得到的信息长度。
```
result=calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);
```
以上语句调用自定义的计算函数，使用待算数字数、包含待算数字的运算信息数组和运算符号作为参数。这里的问题是，调用运算函数时保存待算数字的对象`(int*)opinfo`是之前使用循环语句中的参数`&opinfo[recv_len]`写入信息的。而`recv_len`数值并不是连续整数的`1234`，但是在运算函数的参数定义中使用的`int opnds[]`是使用了带有连续整数值的`i`来遍历`opnds[]`中保存的待算数字的，这里的这个出入不知道应该如何解释。

### TCP/IP通讯

原文的下面进入关于TCP协议建立通讯过程的简单介绍，这里也总结如下。套接字的`read()`和`write()`函数不是在调用的瞬间立即开始读取或写入操作，而是分别从输入缓冲读取和写数据到输出缓冲。I/O缓冲在每个TCP套接字创建时自动生成且单独存在，关闭套接字不会影响已经传输到输出缓冲中的数据，但会中断从输入缓冲中接收数据。TCP协议使用滑动窗口协议，不会发生缓冲溢出导致数据丢失的问题。所谓的`write()`函数在数据传输完成时返回，是指数据传输到输出缓冲时，此后保证缓冲的数据抵达客户端的工作由TCP协议负责完成，套接字只负责到输出缓冲即可。

TCP使用双全工方式工作，可以同时发送和接收数据，在实际通讯建立的过程会经历三次握手，主要是通讯的主机双方互相发送同步数据和确认同步数据的接受，过程如下：
1. SEQ: 1000, ACK: -	A -> B
2. SEQ: 2000, ACK: 1001	B -> A
3. SEQ: 1001, ACK: 2001	A -> B

第一次握手，从主机A到主机B，只包含同步信息，数据包序列号1000为同步目的而发送，如果接受无误主机B会向A发送序号为1001的数据包用于确认。第二次握手，从主机B到主机A，包含同步和确认信息，数据包序列号2000也是为同步目的而发送，如果接受无误主机A会向B发送序列号为2001的数据包用于确认，数据包序列号1001表示之前从A主机发来的同步数据已经得到确认。第三次握手，从主机A到主机B，主要是包含确认信息，可选包含进一步的通讯信息，数据包序列号2001确认之前接受到来自主机B的序列号2000的数据包，如果再有数据需要从主机A发向B，可以使用序列号1001。

通讯通道建立后即可进行数据交换，这里假设主机A需要向主机B发送200字节的数据，分两个大小为100字节的数据包发送：
1. SEQ: 1200 with 100 byte data	A -> B
2. ACK: 1301			B -> A
3. SEQ: 1301 with 100 byte data	A -> B
4. ACK: 1402			B -> A

这里为了简化说明，只设计了单向传输数据的场景，因此每次单方向的通讯只包含一种类型（SEQ或ACK）数据。实际上由于TCP工作在双全工模式下，通讯的主机之间可以同时收发数据，因而在单方向的通讯中也可以与建立通讯时一样，同时包含SEQ和ACK信息。对应对发送方数据包序列号1200的确认序列号是1301而不是1201，是因为其中包含了之前接受的数据大小信息，即在序列号1200上添加了100的字节信息。如果发生通讯故障，例如主机A没有收到主机B对第二个数据包的确认，会在超时后重传没有得到确认的第二个数据包。

断开TCP连接的过程也需要通讯双方共同确认，一个例子如下：
1. FIN, SEQ: 5000, ACK -		A -> B
2. SEQ: 7500, ACK: 5001		B -> A
3. FIN, SEQ: 7501, ACK: 5001	B -> A
4. SEQ: 5001, ACK: 7502		A -> B

首先，主机A向主机B发出断开链接的请求，并带有一个同步数据包5000。主机B回复数据包5001确认接受到同步数据，并发送一个反向同步请求数据包7500。同时，主机B也向主机A发送一个断开连接的请求，额外带有同步数据包7501和同之前一样的确认数据包5001。最后，主机A确认中端链接的请求，回复同步数据包5001和确认数据包7502。这里，主机B向主机A连续发送的两次数据包中都有同步信息，但第一次主要是为确认主机A的断开请求，因此不了解同步信息的存在是否有必要，但比较之前建立通讯时的方式，也许在建立和中断通讯阶段所有的通讯都是需要添加同步信息的。

备注 - 关于指针

与其他高级语言相比，C语言可以更高效的对计算机内存物理地址做指针操作。计算机内存中的单元以字节计算，每个内存单元有独立的编号，指向内存单元地址的标记就叫做指针。指针变量保存的是对象的地址值，例如整型指针变量，只能保存整形变量的地址。不论变量的基本类型如何，相应的指针变量只能保存其地址，而不能保存其实际值。

## UDP的S/C

UDP在通讯方式上比TCP更简洁，不会发送ACK应答信息或给数据包分配SEQ序号，因此性能更高，编程的实现方法也更简单。在数据流量和传输通道质量控制方面，TCP更有优势，但效率时UDP的优势。一般来说，TCP的速度无法超过UDP，在发送某些数据或者单次交换的数据量越大的时候，TCP的传输速率就越接近UDP。

在应用场景上，如果是传输压缩文件，文件的完整性就十分重要，需要保证所有数据包的正确接收，否则无法完成接压缩，此时就需要用TCP协议传输。如果是实时通讯，为传输多媒体数据，速度就称为相对更为重要的因素，需要使用UDP。TCP并不是总是慢于UDP，收到的影响因素包括在收发数据前后进行的链接设置和中断过程，以及在数据收发过程中为保证可靠性尔添加的流控。因此，如果收发的数据量小且需要频繁连接是，UDP就更高效。

### UDP通讯测试

UDP的通讯不基于连接， 也就是其套接字不需要调用`listen()`或`accept()`函数，只需要创建套接字和交换数据。另外，同样由于不基于连接，服务器和客户端的UDP套接字也不必一一对应，双方只要有一个UDP套接字就可以在多台主机之间进行UDP通讯。没有基础连接也有缺点，UDP套接字不知道通讯对方的地址信息，需要在每次数据交换的函数中指定：
```
#include <sys/socket.h>
ssize_t sendto(int sock, void *buff, size_t nbytes, int flags, 
		struct sockaddr *to, socklen_t addrlen);
```
* 以上函数成功时返回传输的字节数，失败时返回-1
* 参数sock	用于传输数据的UDP套接字文件描述符
* 参数buff	保存待传输的数据的缓冲地址值
* 参数nbytes	待传输的数据长度，以字节为单位
* 参数flags	可选参数，没有设定则传输0
* 参数to		保存有目标地址的结构变量的地址
* 参数addrlen	传递给to参数的地址值的结构体变量长度

在接收UDP数据时，由于UDP的发送端不固定，接收函数可以接收发送端信息：
```
#include <sys/socket.h>
ssize_t recvfrom(int sock, void *buff, size_t nbytes, int flats,
		struct sockaddr *from, socklen_t *addrlen);
```
* 以上函数成功返回接收的字节数，失败返回-1
* 参数sock	接收数据的UDP套接字的文件描述符
* 参数buff	保存接收数据的缓冲地址
* 参数nbytes	可接受的最大字节数，无法超过参数buff指定的大小
* 参数flags	可选参数，没有则传入0
* 参数from	存有发送端地址信息的结构体变量的地址
* 参数addrlen	保存参数from的结构体变量长度的变量地址

编译和运行服务器与客户端程序，程序见文档库中的文件，测试结果如下：
```
// 在一个终端中运行服务器程序
$ gcc 16_uecho_server.c -o userver
$ ./userver 9190
// 在另一个终端中运行客户端程序
$ gcc 17_uecho_client.c -o uclient
$ ./uclient 127.0.0.1 9190
Insert message(q to quit): HI UDP Server
Message from server: HI UDP Server
Insert message(q to quit): Nice to meet you!
Message from server: Nice to meet you!
Insert message(q to quit): Good bye~~
Message from server: Good bye~~
Insert message(q to quit): q
```

### 数据边界

TCP的数据传输不存在数据边界，结果就是数据传输中调用I/O函数的次数没有意义。相反，UDP的数据传输存在数据边界，调用I/O函数的次数也就非常重要。具体来说，发送和接收数据的次数必须完全一致才能保证接收全部已经发送的数据。这里创建和编译两个数据传输程序用于测试数据边界。
```
// 在一个终端上运行hostA
$ gcc 18_bound_hostA.c -o hosta
$ ./hosta 9190
Message 1: Hi! 
Message 2: I'm another UDP host! 
Message 3: Nice to meet you! 
// 在另一个终端上运行hostB
$ gcc 19_bound_hostB.c -o hostb
$ ./hostb 127.0.0.1 9190
```

以上程序中，hostB是一次性发出的三条信息，hostA在每次调用接收信息的函数间延时了5秒。如果是TCP方式传输，接收信息的函数只用调用一次就可以读取到之前发送的三条信息，因为TCP方式下不存在数据边界。但实在UDP方式下，读取信息的函数需要与发送信息的函数成对出现，所以虽然发送来的信息有足够时间存入缓存，读取的时候还是要匹配发送次数调用函数三次。

### 带连接的UDP

默认的UDP套接字连接每次传输数据前注册目标IP和端口号，传输数据之后注销IP和端口号，这两个操作发生在正式传输数据的前后，会占据整个通信时间的三分之一。对于与同一主机的长时间通讯，UDP也支持带连接的数据通信，使用`connect()`函数建立连接即可，相应的发送和接收数据的方法也由`write()`和`read()`代替了`sendto()`和`recvfrom()`。这里也更新了UDP客户端程序并验证如下，实际效果与原有程序相同，但执行效率会更高：
```
// 在一个终端中运行服务器程序
$ ./userver 9190
// 在另一个终端中运行更新的客户端程序
$ gcc 20_uecho_con_client.c -o ucclient
$ ./ucclient 127.0.0.1 9190
Insert message(q to quit): UDP with connection
Message from server: UDP with connection
Insert message(q to quit): it works as well
Message from server: it works as well
Insert message(q to quit): q
```

## 断开套接字连接

相对建立套接字连接，断开的过程可能出现更多变数，而使用Linux下的`close()`或Windows下的`closesocket()`函数断开套接字其实是一种相对暴力的强行断开方式，本节会介绍半关闭的概念，以明确具体的连接断开过程。回顾之前提到的发送和接收信息的缓冲操作，任何发送或接收放单方使用全中断方式关闭连接是不稳妥的，因为可能存在缓冲数据，特别在没有处理好数据边界的问题时，对方缓冲的发送数据即本方待接收数据的丢失。

这里引入Stream，即所谓数据流，的概念。在固定连接的方式下，通讯双方中一方的`read()`和另一方的`write()`是成对存在的，构成一个数据流，而一般会有两个不同方向的流同时存在于通讯双方的主机之间。之前介绍的关闭函数会关闭整个套接字的所有通信，也就是同时切断了两个方向的数据流，这里介绍一个半关闭函数，只关闭指定方向的数据流，保留另一方向开放：
```
#include <sys/socket.h>
int shutdown(int sock, int howto); 
```
* 以上函数成功返回0，失败返回-1
* sock	需要做断开操作的套接字文件描述符
* howto	连接断开的方式
* -> SHUT_RD	断开输入流同时清除输入缓存
* -> SHUT_WR	断开输出流但不会清除输出缓存
* -> SHUT_RDWR	同时断开输入输出流

一个使用了半关闭函数的数据交换流程如下：
1. 客户端向服务器请求连接
2. 服务器发送数据到客户端
3. 服务器发送EOF的文件结尾标志，半关闭通讯连接
4. 客户端返回服务器确认接收结束

这里编写了两个小程序用于测试服务器端和客户端对半连接的应用：
```
// 在一个终端中运行服务器程序
$ gcc 21_file_server.c -o fserver
$ ./fserver 9190
// 在客户端程序运行结束后，服务器端收到如下结果
Message from client: Thank you! 
// 在另一个终端中运行客户端，确认接收到了文件
$ gcc 22_file_client.c -o fclient
$ ./fclient 127.0.0.1 9190
Received file data!
// 确认接受到的保存了服务器源代码的文件
$ head -n 4 22_receive.dat 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
```

具体的程序代码和注释可以参考地21和第22个程序文件，有一个地方需要强调：
```
    puts("Received file data!");
    // 发送最后的提示消息，如果服务器端未关闭输入流则可以接收
    // 以下的第三个参数原文用的是`10`，会造成服务器端接收到多余字符
    // 修改为`BUF_SIZE`可以修正此问题
    write(sock, "Thank you!", BUF_SIZE);
    fclose(fp);
    close(sock);
    return 0;
```
在返回服务器信息以测试半连接时，使用`BUF_SIZE`代替原书中使用的`10`可以清除服务器端接收的多余信息。

其他部分的代码没有特别只处，只是在服务器端的如下代码涉及到了半连接：
```
    // 发送文件后对输出流半关闭，即向客户端发送了EOF
    shutdown(clnt_sock, SHUT_WR);
    // 关闭输出流后，依然可以接收数据
    read(clnt_sock, buf, BUF_SIZE);
    printf("Message from client: %s \n", buf);
```
在服务器使用半连接关闭了发送信息后依然可以接受信息，在后面接收到客户端发送的`Thank you`后即完成了验证。

## 域名和网址

域名系统（DNS Domain Name System）负责对应相对直观和容易理解的域名到IP地址，网络上的域名服务器使用层级结构提供域名到IP地址的查询服务。如果需要查询某个域名对应的IP地址，可以使用`ping`或者`nslookup`命令，接域名地址。在程序中使用域名可以避免把IP地址，特别是某些服务器的IP地址，硬编码到程序中，也就不必在IP地址更新时更新程序。

使用如下函数获取某个域名的IP地址：
```
#include <netdb.h>
Struct hostent * gethostbyname(const char * hostname);
```
* 以上函数成功返回hostent类的结构类型地址信息，失败时返回NULL指针
* hostent结构体的定义如下
```
struct hostent
{
    char * h_name;	//主机名称-官方域名
    char ** h_aliases;	//别名列表-其他域名
    int h_addrtype;	//地址类型-IPv4地址返回AF-INET
    int h_length;	//地址长度-IPv4地址返回4（个字节）
    char ** h_addr_list;//地址列表-可能返回多个地址
}   
```

这里编写一个程序，代号23，说明函数`gethostbyname`的应用方式和`hostent`结构体中保存的地址信息：
```
$ gcc 23_gethostbyname.c -o hostname
IP addr 1: 104.75.164.236 
$ ./hostname www.google.com
Official name: www.google.com 
Address type: AF_INET 
IP addr 1: 172.217.24.36 
```

反向的操作也可以通过函数实现，即通过IP地址获取域名：
```
#include <netdb.h>
struct hostent * gethostbyaddr(const char * addr, socklen_t Len, int family); 
```
* 以上函数成功返回hostent结构类型的地址，失败时返回NULL指针
* addr	含有地址信息的in_addr结构类型指针。为传递IPv4地址以外的信息，声明变量为char类型指针
* len	向第一个参数传递的地址信息的字节数，IPv4时为4，IPv6时为16
* family传递地址族信息，IPv4时为AF_INET，IPv6时为AF_INET6

这里也编写一个小程序，代号24，说明`gethostbyaddr`函数的应用方式：
```
$ gcc 24_gethostbyaddr.c -o hostaddr
$ ./hostaddr 172.217.24.36
Official name: hkg07s23-in-f4.1e100.net 
Aliases 1: 36.24.217.172.in-addr.arpa 
Aliases 2: hkg07s23-in-f36.1e100.net 
Address type: AF_INET 
IP addr 1: 172.217.24.36 
```
程序源码可以参考代码文档库中的文件。这里可以看到从IP到域名的反向查找有时并不能得到有用信息，从域名解析IP查到的谷歌IP地址在反向查找时并没有指向带有谷歌域名的地址。

## 套接字选项

套接字有一些可选项，在IP、TCP和UDP层分别可以做出不同的设置。可以使用`getsockopt()`和`setsockopt()`函数来读取和设置套接字可选项。

在不同协议层（`SOL_SOCKET`,`IPPROTO_IP`和`IPPROTO_TCP`）上有不同的选项可以查看和定义。这里给出一个小程序查看和修改相关选项，部分选项在后面有说明。
```
#include <sys/socket.h>
int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen); 
```
* 以上函数成功返回0，失败返回-1
* sock		查看或更改选型的目标套接字文件描述符
* level		查看或更改的可选项所属协议层
* optname	查看或更改的可选项名称
* optval	查看或更改的可选项值
* optlen	向可选项值传递的缓冲大小

这里写一个小程序，代号25，检验默认的的TCP和UDP套接字选项值和充置后的值：
```
$ gcc 25_getset_sock.c -o gssock
$ ./gssock 
The setting value used in configuration:
SOCK_STREAM: 1 
SOCK_DGRAM: 2 
The real default value in new Sockets:
Socket type TCP: 1 
Socket type UDP: 2 
Send buffer TCP: 131072 
Send buffer UDP: 9216 
Send buffer TCP after reset: 3072 
Send buffer UDP after reset: 3072 
Receive buffer TCP: 131072 
Receive buffer UDP: 786896 
Receive buffer TCP after reset: 3072 
Receive buffer UDP after reset: 3072 
Reuse addr TCP: 0 
Reuse addr UDP: 0 
Keep alive TCP: 0 
Keep alive UDP: 0 
Broadcast TCP: 0 
Broadcast UDP: 0 
Do not route TCP: 0 
Do Not Route UDP: 0 
OOB inline TCP: 0 
OOB inline UDP: 0 
ERROR TCP: 0 
ERROR UDP: 0 
IP TOS TCP: 0 
IP TOS UDP: 0 
IP TTL TCP: 64 
IP TTL UDP: 64 
state of getsockopt() for IP_MULTICAST_TTL in TCP is: -1 
IP Multicast TTL TCP: 107589632 
IP Multicast TTL UDP: 1 
state of getsockopt() for IP_MULTICAST_LOOP in TCP is: -1 
IP Multicast Loop TCP: 32766 
IP Multicast Loop UDP: 1 
state of getsockopt() for IP_MULTICAST_IF in TCP is: -1 
IP Multicast If TCP: -380221720 
IP Multicast If UDP: 0 
Keep Alive TCP: 7200 
When no-delay (Nagle) is off for TCP: 4 
Max Seg TCP: 512 
```
通过以上程序可以看到，设置参数的实际值和相应套接字的属性值都是一样的`1`和`2`。而其他设置值中除了缓冲设置值外大部分默认为0。

### 服务器端口重用

这里关于`SO_REUSEADDR`选项稍作解释。在默认设置下，服务器端在程序强制终止后并没有完全杀死进程，会有一个默认为几分钟的后台存活和等待时间，这样的默认设置导致服务器端在意外或强制的程序终止后无法立即重启，生产环境下可能影响服务可用性。这样的设计是基于套接字终止的四次握手机制：
方向	握手类型	信息内容
S->C	FIN	SEQ 5000 / ACK -
C->S	ACK	SEQ 7500 / ACK 5001
C->S	FIN	SEQ 7501 / ACK 5001
S->C	ACK	SEQ 5001 / ACK 7502
在四次握手机制中，如果是客户端先发出FIN握手，也同样会触发等待时间，但由于客户端生成的套接字选用随机端口，每次生成的端口都不同，所以不同担心无法重启客户端程序问题。如果服务器端需要在意外的程序终止后立即重用端口，可以设置改选项为真（即整型的值1，默认为0）。这里做了一个小程序，代码26，基于之前的回声服务器修改而成。
```
// 在源代码中注释掉新添加的用于打开端口重用的代码
$ gcc 26_reuseaddr_echo_server.c -o reserver
$ ./reserver 9091
Connected client 1 
hello
^C
$ ./reserver 9091
bind() error!
// 重新启用打开服务器端端口重用的代码，再次运行
$ ./reserver 9091
Connected client 1 
hello
^C
$ ./reserver 9091
Connected client 1 
hello again
// 以下为客户端在另一个终端的运行状况
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): hello
Message from server : hello
 
Input message(Q to quit): ^C
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): hello again
Message from server : hello again
 
Input message(Q to quit): q
```

### `Nagle`算法

关于可选项设置，再介绍一个`Nagle`算法的概念。该算法在1984年诞生，用于优化TCP层的网络传输效率，默认为打开状态。在打开状态，系统会等待数据填满一个数据包再发送，在关闭状态，系统只要发现缓存中有数据就立即发送，且不用等待前一个数据包返回的ACK确认信息。

在早期网络带宽较窄时，启用该算法可以避免发送数据包的包头等对最终用户无效的数据，的确可以改善网络效率。但现在网络的带宽很高，将数据写入输出缓冲不会用太多时间，有时需要传输大文件，或不必等待每个前数据包的ACK再发送下个数据包，此时关闭`Nagle`算法就可以提高传输速度。
```
// 以下代码通过修改`tcp_nodelay`的可选项值关闭`Nagle`算法
int opt_val=1; 
setsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val));
// 查看`tcp_nodelay`可选项的设置值
int tcp_nodelay;
socklen_t opt_len;
opt_len=sizeof(opt_val)
state=getsockopt(tcp_sock, IPPROTO_TCP, TCP_NODELAY, (void*)&tcp_nodelay, &optlen);
if(state)
    error_handling("getsockopt() error!");
printf("When no-delay (Nagle) is off for TCP: %d \n", tcp_nodelay);
```
以上代码已经包含在代号为25的小程序中，可以查看和设置相关的套接字可选项。

## 多进程服务器

如果无法支持多个客户端的并发访问，或者对后连接的客户端无法提供与第一个发起连接的客户端同样的服务水平，客户满意度都会收到影响。这里有三个办法构建可以处理并发请求的服务器端：多进程，多路复用（捆绑并统一管理I/O对象），多线程。本部分讨论多进程的实现方法，该方法在Windows平台上因为不支持而无法实现。

进程（Process）是程序流的基本单位，独立占用内存和计算资源，不同程序会创建各自进程，同一个程序也可创建多个进程，操作系统可以运行多个进程。多核CPU可以真正同时运行与核数相同的进程，更多进程需同时运行要分时共享CPU资源以达到使用户体验为同时运行的目的。在终端中运行命令`ps au`可以查看所有正在运行的进程详情。

### 复制进程

创建进程的方法不止一种，创建多进程服务器端使用`fork()`函数。该函数创建当前父进程的副本子进程，子进程与父进程同时运行服务器端的程序，函数返回子进程ID的为父进程，返回0的为子进程，如果创建子进程失败则返回-1。以下用一个小程序演示创建进程副本的过程：
```
$ gcc 27_fork.c -o fork
$ ./fork
Original Val: [10 20]
Before Process: [11 25]
Parent Proc: [9 23]
Child Proc: [13 27]
```
以上程序的源码可以参考代号27的c文件。从结果可以得到一个有趣的猜测，在创建子进程后，父进程依旧是先运行。源码中使用`if`判别式做了父子进程不同结果的输出设计，如果程序只运行一遍则只会得到一个结果，同时得到两个结果表明父子进程都执行了程序。

### 僵尸进程

进程完成工作后需要调用`exit()`函数或在该进程所运行的`main()`函数中执行`return`语句并返回值。对于子进程，需要父进程传递子进程的`exit（）`函数参数或`return`语句的返回值。这里也创建一个小程序演示僵尸进程的情况：
```
$ gcc 28_zombie.c -o zombie
$ ./zombie
parent process working!
child process ID: 39305
child process working!
End of child process!
End of parent process!
```
程序的源码可以参考代号为28的c文件，程序的运行逻辑在文件注释中也有，即父进程先运行然后进入等待，接着子进程运行并宣布结束，但实际上子进程并没有真正的结束，因为父进程还在等待超时，主程序中的`return`命令还没有触发，此时可以在另一个终端中运行`ps au`命令查看到有两个名称为`zombie`的存活进程。直到父进程也宣布结束，即主程序的`return`命令触发后父子进程才真正的结束，同样可以通过运行`ps au`命令确认。

### 后台运行

在程序运行中查看后台进程，这样的需求需要打开额外的终端，有些麻烦。如果将之前的程序放到后台运行，当前的终端就可以继续操作，这里演示如下：
```
// 在终端命令后加入`&`以放置该命令到后台去运行
$ ./zombie &
[1] 40090
$ parent process working!
child process ID: 40091
child process working!
End of child process!
// 程序在这里暂停，输入以下命令可以查看进程列表
ps au
...
// 父进程超时后，程序正式结束
$ End of parent process!
```

### 销毁子进程

要销毁子进程，从子进程中调用`return`方法返回值是没有用的，因为子进程的设计初衷之一就是要返回值给父进程，如果没有确认父进程得到返回值就销毁，设计的任务等于没有完成。销毁子进程有两种方法，这里先看使用`wait()`函数的方法：
```
#include <sys/wait.h>
pid_t wait(int * statloc);
```
* 该函数在成功时返回终止的子进程ID，失败时返回-1

该函数在终止子进程的同时，保存子进程相关信息到其参数所对应的内存空间,可以用如下宏命令处理：
* WIFEXITED	Wait If Exited		子进程正常终止则返回true
* WEXITSTATUS	Wait Exit Status	返回子进程的返回值

这里同样编写一个小程序，代号29，测试`wait()`函数和其参数的返回值：
```
$ gcc 29_wait.c -o wait
$ ./wait
child PID: 61075
child PID: 61076
child exit one: 3
child exit one: 7
// 这里程序会等待30秒，其间可以查看进程状况，确认子进程已经销毁
$
```
在同上程序中，可以注释掉包括`wait()`函数及查看其返回值的语句，再次运行程序并查看进程，可以发现有两个名称为`(wait)`的子进程在运行，直到父进程超时后才被销毁。这里有一个问题需要提醒注意，就是调用`wait()`函数时系统中需要的确有子进程在运行，如果没有的话程序会停留在这里等待直到有子进程可以操作，因此需要谨慎调用该函数。

销毁子进程的另一个方法是`waitpid()`函数：
```
#include <sys/wait.h>
pid_t waitpid(pid_t pid, int * statloc, int options);
```
* 该函数成功时返回终止的子进程ID或0，失败时返回-1
* pid: 等待被终止的目标子进程ID。如果传递-1，则与`wait()`函数相同，等待任意子进程被终止
* statloc: 保存被终止的子进程状态的内存地址
* options: 传递头文件sys/wait.h中声明的常量WNOHANG，没有需要终止的子进程则返回0并退出函数

这里同样编写一个小程序，代号30，验证`waitpid()`函数的功能：
```
$ gcc 30_waitpid.c -o waitpid
$ ./waitpid 
waitpid sleeps 3sec.
waitpid sleeps 3sec.
waitpid sleeps 3sec.
waitpid sleeps 3sec.
waitpid sleeps 3sec.
child exit: 24
```
从以上的程序执行结果可以看出，函数`waitpid()`在等待期间不会因为没有子程序可销毁而暂停整个程序的执行，反复的尝试不会对程序的正常运行有任何影响。如果是`wait()`函数就会导致程序死锁在等待阶段，直到子程序超时。在代号30的小程序源码中也在`waitpid()`函数附近保留了一条处于注释状态的`wait()`函数语句，可以注销前者反注销后者运行：
```
$ gcc 30_waitpid.c -o waitpid
$ ./waitpid 
child exit: 24
```
可以看到包含了`wait()`函数的`while`判别式一次也没有执行，因为`wait()`函数从一开始就处于等待状态，直到成功执行一次后返回子进程ID，在`!`操作符下判别为`false`，故一次也没有执行。

### 信号处理

以上终止或销毁子进程的两种方法都需要父进程的监护，操作系统如果可以帮忙将子进程的运行状况通知到父进程可以提高程序运行效率。这里提到的信号处理也叫做信号注册，就是让操作系统在特定事件发生时调用特定函数，方法为`signal()`函数：
```
#include <signal.h>
void (*signal(int signo, void (*func)(int)))(int); 
```
* signo: 触发信号的特殊事件信息，可以包含如下几种情况：
* -> SIGALRM: 等待`alarm`函数超时
* -> SIGINT: 输入CTRL+C
* -> SIGCHLD: 子程序终止
* (*func)(int): 特殊事件发生时需要调用的函数地址值（指针）
* -> 该函数的参数值需要为整型，返回值类型需要为空

因为`signo`参数涉及到`alarm()`函数，这里对此介绍如下：
```
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
```
* 以上函数返回0或到SIGALARM信号发生为止的剩余秒数。
* seconds: 正值整数，定义发送信号的剩余时间。如果是0则取消发送信号。
* 该函数超时后发出的信号如果没有被`signal()`捕捉，则不做任何处理。

这里也用一个小程序，代号31，来演示`signal()`函数的处理过程：
```
$ gcc 31_signal.c -o signal
$ ./signal 
wait...Time out!
wait...Time out!
wait...Time out!
$ ./signal 
wait...Time out!
^Cwait...CTRL+C pressed!
wait...Time out!
```
编译后，第一次运行不做特殊操作，程序自动超时三次。第二次在第二次超时前按下`CTRL+C`可以被程序捕捉到，另外两次依旧自动超时。对于默认情况下的执行过程解释如下：
1. 注册两个信号处理器，默认情况下只触发SIGALRM信号
2. 准备一个2秒后发出的ALARM信号
3. 进入第一次循环，打印关于等待的通知文字
4. 进入100秒的超时等待时间
5. 之前准备的ALARM信号达到2秒超时，触发ALARM信号
6. ALARM信号触发timeout函数的运行
7. 打印超时通知文字，同时再准备一个2秒后发出的ALARM信号
8. 进入第二次循环，重复之前的3到7步两次
由于每次循环内的100秒等待命令之前都有一个准备好的2秒超时后的ALARM信号，而每次任意信号的触发都会中止系统的等待，因此无论是默认的每2秒触发的ALARM信号还是手动按下的`CTRL+C`都会终结系统的100秒等待。

不同系列的UNIX系统中`signal()`函数的执行方式可能存在细微差别，但`sigaction()`函数执行方式完全相同，因此后者也更稳定：
```
#include <signal.h>
int sigaction(int signo, const struct sigaction * act, struct sigaction * oldact);
```
* 成功返回0，失败返回-1
* signo：传递信号信息
* act：对应第一个参数的信号处理函数
* oldact：之前注册的信号处理函数指针，没有则设置为0

结构类型`sigaction`的结构体定义如下：
```
struct sigaction
{
    void (*sa_handler)(int);
    sigset_t sa_mask;
    int sa_flags; 
}
```
以上结构类型定义中，`sa_handler`保存信号处理函数的指针，`sa_mask`和`sa_flags`指定信号相关的选项和特性，由于这里只讨论如何终止僵尸进程，后两者设置为0即可。这里也编写了小程序，代号32，验证`sigaction()`函数作用：
```
$ gcc 32_sigaction.c -o sigaction
$ ./sigaction 
wait...
Time out!
wait...
Time out!
wait...
Time out!
```
实现的功能与之前类似，具体执行过程逻辑也类似，这里不再解释，有必要可看源码32号c文件及其注释。

应用以上的函数功能到服务器程序的僵尸进程清理，这里参照程序代号33:
```
$ gcc 33_remove_zombie.c -o remove_zombie
$ ./remove_zombie 
Child proc id: 95356 
Subchild proc id: 95357 
wait 5sec!
This is child process!
This is subchild process!
wait 5sec!
Removed proc id: 95357 
child send: 24 
Removed proc id: 95356 
child send: 12 
wait 5sec!
wait 5sec!
wait 5sec!
```
具体的源码可以参考33号c文件，没有注释因为所有内容都在前述中解释过。简单的再说明，就是在子进程通过`return`或`exit`命令完成任务后并没有完全退出自己，而是处在等待状态。此时通过`sigaction`函数的`SIGCHLD`参数发送信号给系统调用`read_childproc`方法，其中通过`waitpid`正式终止子进程并返回子进程信息。在主程序中，外层父进程只打印外层子进程的ID，内层父进程除了打印内层子进程ID外还会运行一个5秒的等待命令5次，其间内层和外层子进程各自的10秒等待时间会在内层父进程结束前完成超时和退出，被`sigaction`函数捕捉进而被`waitpid`销毁。

这里再说明比较有趣的内容。第一是虽然有内外两层子进程，但其执行顺序并没有先后关系而是平行关系，也就是两个子进程中的等待时间会同时超时而不是在中间再等待10秒。第二是具体执行程序得到的结果与原书中的不同，主要是等待5秒的提示文字出现顺序不同，且每次运行该程序都会略有不同。从结果来看，原书中子进程中的命令先执行，而我的结果中总是父进程的命令先执行。

### 并发服务器

通过创建子进程，服务器可以真正的处理并发请求。父进程通过调用`accept`函数受理连接请求，获得客户端的套接字文件并传递给`fork`函数复制出的子进程，子进程提供服务并在服务结束后销毁。这里用程序代号34验证：
```
// 在第一个终端中运行服务器程序
$ ./mpserv 9091
new client connected...
new client connected...
client disconnected...
removed proc id: 2447 
client disconnected...
removed proc id: 2415
// 在第二个终端中运行客户端一
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): Hi there! 
Message from server : Hi there!
 
Input message(Q to quit): This server is working for client one!
Message from server : This server is working for client one!
 
Input message(Q to quit): q
// 在第三个终端中运行客户端二
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): Hi there!
Message from server : Hi there!
 
Input message(Q to quit): This server is working for client two as well!
Message from server : This server is working for client two as well!
 
Input message(Q to quit): q
```
关于服务器源码的解释参考代号34的c文件中的备注。这里提及几个有趣的地方，一个是我的代码把缓冲大小的设置由`30`提升到了`1024`，否则如上长度的测试信息无法一次发送成功，会导致第二次发送的信息丢失，实际会发送上次没有发送完成的片段。第二个是客户端地址长度变量`addr_size`的设置，原书放在`while`循环中，每次单独客户端都需要运行一次，由于实际上该值的初始化基于固定大小的结构类`sockaddr_in`变量`clnt_addr`，我的代码中在声明时即做了初始化。最后一个是对`accept`和`fork`函数的异常处理，基本上采取直接跳过的`continue`方法，不知道这样是否合理。

### 分割I/O

之前运行的回声客户端程序运行`read`和`write`函数在一个进程中，需要等待前序任务完成后才能继续下一个。如果使用创建子进程的方法分割读写操作到父子进程，不仅可以提升数据的交换效率，也可以使程序本身在结构和逻辑上更简单和稳定，具体参考代号35的c源码文件。执行方式和结果与之前的客户端没有区别：
```
$ gcc 35_echo_mpclnt.c -o mpclnt
$ ./mpclnt 127.0.0.1 9091
Connected......
Input message(Q to quit)
TEST
Message from server : TEST
hello there
Message from server : hello there
q
```

## 进程间通信

原理很简单，两个进程可以同时访问同一个内存空间即可。但进程有独立的内存结构，通过`fork()`函数创建的子进程不会与父进程共享内存空间，这里介绍使用管道函数`pipe()`的方法：
```
#include <unistd.h>
int pipe(int filedes[2]);
```
* 以上函数成功返回0，失败返回-1
* filedes[0]: 接收数据使用的文件描述符，即管道出口
* filedes[1]: 发送数据使用的文件描述符，即管道入口
父进程调用`pipe()`函数时创建管道，根据需求将出口或入口的文件描述符传递给子进程。这里要特别注意，文件描述符数组中的第一个成员时管道出口，第二个是管道入口。例如从子进程读取数据的场景下，父进程使用`read()`方法从`filedes[0]`读取信息，子进程使用`write()`方法从`filedes[1]`写入信息，示例程序如下：
```
$ gcc 36_pipe_1d.c -o pipe_1d
$ ./pipe_1d 
How are you?
```
以上程序的源码见代号36的c文件，下面再用一个程序演示双向通信：
```
$ gcc 37_pipe_2d.c -o pipe_2d
$ ./pipe_2d 
Parent proc output: How are you? 
Child proc output: I'm good. How about you? 
```
具体的过程和逻辑并不复杂。但根据原书，以上程序有一个问题，需要在父子进程的读取间严格的控制流程，否则数据一旦写入管道就会被任意先到的读取命令带走。注释掉子进程部分代码中写入`write`操作后的2秒延迟`sleep`命令，再次编译和运行程序应该会出错。因为子进程写入后立即读取了数据，父进程的读取命令进入无限期等待。但在我的MacOS系统上没有能够成功演示这个错误。

即便MacOS上没有出现原书中描述的错误，也许其他系统的实施方式不同，还是有出错可能。稳妥的方法是为进程间双向的读写建立独立的管道，演示程序如下：
```
$ gcc 38_pipe_2xd.c -o pipe_2xd
$ ./pipe_2xd 
Parent proc output: How are you? 
Child proc output: I'm good. How about you? 
```
可以看到结果没有什么不同，的确如此，但程序本身变得更健壮了。以下使用管道功能改进之前建立的多进程回声服务器程序，将客户端发送的信息保存到一个外部文本文件中，演示如下：
```
// 编译、运行和终止服务器端
$ gcc 39_echo_storeserv.c -o storeserv
$ ./storeserv 9092
new client connected...
new client connected...
removed proc id: 80644 
client disconnected...
removed proc id: 80647 
client disconnected...
removed proc id: 80650 
^C
// 显示服务器端保存的回声记录
$ cat echomsg.txt 
1
2
3
a
b
c
4
5
d
e
// 客户端1
$ ./mpclnt 127.0.0.1 9092
Connected......
Input message(Q to quit)
a
Message from server : a
b
Message from server : b
c
Message from server : c
d
Message from server : d
e
Message from server : e
q
// 客户端2
$ ./mpclnt 127.0.0.1 9092
Connected......
Input message(Q to quit)
1
Message from server : 1
2
Message from server : 2
3
Message from server : 3
4
Message from server : 4
5
Message from server : 5
q
```
需要注意的是，以上服务器端程序需要在接收到十条通话记录后才会保存文件，因此需要两个客户端累计发送超过十条信息，否则服务器端不会执行保存文件的操作。具体使用通道在父子进程之间传递回声信息的操作可以参考代号39的程序源码C文件。

## I/O复用

之前介绍了使用`fork()`函数复制进程而实现的并发服务器，这一部分介绍使用I/O复用功能实现的并发服务器，可以根据不同使用场景和两种技术手段的优缺点在生产中灵活应用。

为每个新的客户端连接建立子进程会消耗运算资源和内存空间。各进程间内存空间相互独立，因此数据交换的实现也更为复杂。实施IPC（Inter Process Communication）进程间通讯也会增加编程难度和程序复杂度。这里介绍一种新的I/O复用（Multiplexing）技术，在某些场景下可以不创建新的进程而实现对多个客户端的响应。

### select函数

简单说，复用就是在一个通道中建立多个会话，可以有时分复用和频分复用两种方式，即不同的会话在不同的时间窗口或不同的频率上共享一个通道。这里介绍的`select()`函数可以监视接收、传输和发生异常的套接字。该函数的使用较复杂，需要做准备和善后。准备工作包括，设置文件描述符、指定监视范围和设置超时时间。善后工作就是检查调用结果。
```
#include <sys/select.h>
#include <sys/time.h>
int select(int nfd,fd_set *rset,fd_set *wset,fd_set *eset, const struct timeval *timeout);
```
* 以上函数错误返回-1，超时返回0，发生关注的事件返回发生事件的文件描述符数量
* nfd 监视的文件描述符数组中文件描述符最多的数量
* rset 监视读取数据的文件描述符数组指针
* wset 监视传输数据的文件描述符数组指针
* eset 监视发生异常的文件描述符数组指针
* timeout 函数的等待和超时时间

`select()`函数的第一个函数是监视的文件描述符数量，或者说监视范围。由于可以监视三个不同的数组，这里定义的监视范围是三个数组中文件描述符最多的对象数量。

`select()`函数的第二到第四个参数都是文件描述符数组（fd_set）类型的对象指针，分别对应对读取、发送和异常的监视。`fd_set`数组实际上是一个位操作数列，每个位可取值为`0`或`1`，对应相应文件描述符的监视结果。对`fd_set`数组的操作可以由以下宏命令完成：
* FD_ZERO(fd_set *xset): 将`xset`数组的所有位初始化为0
* FD_SET(int fd, fd_set *xset): 在`xset`数组中注册文件描述符fd的信息
* FD_CLR(int fd, fd_set *xset): 在`xset`数组中清除文件描述符fd的信息
* FD_ISSET(int fd, fd_set *xset): 在`xset`数组中如果有文件描述符fd的信息则返回`true`

`select()`函数的最后一个参数设置超时时间，因为该函数只在监视的文件描述符发生变化时才返回，否则会一直停留在监视状态。为了防止无限期的监视使程序陷入死锁，这里配置超时时间以便在合理的监视时间后退出。如果不希望设置超时时间可以传递`NULL`，否则该参数需要遵循`timeval`结构体类型的定义：
```
struct timeval
{
    long tv_sec: //整秒数
    long tv_usec; //微秒数
}
```

这里创建一个简单的小程序，代号40，用来验证`select()`函数的功能：
```
$ gcc 40_select.c -o select
$ ./select
Input something within 5 seconds.
Press Ctrl + C to quit the program.
hello
message from console: hello
Time out!
Time out!
goodbye
message from console: goodbye
^C
```
以上程序初始化了一个`fd_set`类型的变量`read`，设置0位的文件描述符为`1`，启用对标准输入的监视。之后复制`read`文件描述符数组到`temps`，调用`select`函数后，除发生变化的文件描述符对应位外其他数组位会初始化为0。

### I/O复用服务器

使用以上`select`函数实现I/O复用的服务器端，源码参见代号41的C文件：
```
// 服务器
$ gcc 41_echo_selectserv.c -o selserv
$ ./selserv 9091
connected client: 4 
connected client: 5 
closed client: 4 
closed client: 5
// 客户端1
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): hello
Message from server : hello
Input message(Q to quit): good bye
Message from server : good bye
Input message(Q to quit): q
// 客户端2
$ ./eclient 127.0.0.1 9091
Connected......
Input message(Q to quit): hi there
Message from server : hi there
Input message(Q to quit): see you
Message from server : see you
Input message(Q to quit): q
```
需要注意的是，文件描述符就是从0开始递增的正整数，而套接字由系统自动注册得到文件描述符，套接字对象的值就是其文件描述符的值，因此可以采用`fd_max=serv_sock`和`fd_max=clnt_sock`的操作直接将套接字赋值给表示监视范围的变量。再就是代码中反复出现的`fd_max+1`也是因为文件描述符从0开始，比监视对象实际的数量少1。

## 其他I/O函数
之前涉及到读取和写入数据使用的都是`read()`和`write()`函数，这里介绍一些其他关于数据传输的函数和参数。

### `send()`和`recv()`

`send()`和`recv()`函数在Windows平台上也有应用，这里基于Linux平台介绍如下：
```
#include <sys/socket.h>
ssize_t send(int sockfd, const void * buf, size_t nbytes, int flags);
ssize_t recv(int sockfd, void * buf, size_t nbytes, int flags);
```
* 以上发送函数成功返回发送的字节数，失败返回-1
* 以上接收函数成功返回接收字节数，或在受到EOF时返回0，失败返回-1
* sockfd 数据发送或接收对象的套接字文件描述符
* buf 保存待发送数据或接收数据的缓冲地址
* nbytes 发送的待传输字节数或接收的最大可接收字节数
* flags 发送或接收数据时指定的可选项信息，具体如下：
* —> MSG_OOB 传输带外数据（out of band data），用于发送或接收函数
* —> MSG_PEEK 验证输入缓冲中是否存在接收的数据，用于接收函数
* —> MSG_DONTROUTE 传输中不参照路由，在本地网络中查找目标，用于发送函数
* —> MSG_DONTWAIT 调用函数时不做无效等待，用于发送或接收函数 
* —> MSG_WAITALL 防止函数在没有接收到全部请求的字节数前返回，用于接收

以上可选项在不同操作系统中的支持情况可能不同，以下介绍不同系统间通用的可选项：

### 紧急消息

MSG_OOB可选项用于创建特殊发送方法和通道以发送紧急消息，发送和接收端样例程序代号42和43，验证如下：
```
// 先运行接收端程序
$ gcc 43_oob_recv.c -o obbrecv
$ ./obbrecv 9190
// 在发送端运行后可以得到如下结果
Urgent message: 4 
Urgent message: 0 
123
56789
// 再运行发送端程序
$ gcc 42_oob_send.c -o obbsend
$ ./obbsend 127.0.0.1 9190
```
以上程序中有一处未解释过的函数`fcntl(recv_sock, F_SETOWN, getpid())`，该函数将`recv_sock`套接字文件描述符的所有者指向到`getpid()`返回的进程。因为多个进程可以共用拥有一个套接字文件描述符，如果有进程复制或子进程创建发生，会有多个进程同时拥有`recv_sock`。而`urg_hanlder()`函数涉及处理`recv_sock`，且`sigaction(SIGURG, &act, 0)`注册的触发器`act`中调用了`urg_hanlder()`函数。一旦该触发器作用，响应的进程会有多个从而引发问题，在定义触发器前把`recv_sock`套接字文件描述符的所有者统一到当前同一进程是必要的。

根据原书，如上使用MSG_OOB选项后并不会真正实现紧急信息的优先发送，因为`MSG_OOB原意中的带外数据是指通过不同的通信路径传输数据`，并不会提供更快的传输速度或优先级，但是如上结果中可以看到，在我的电脑上的确实现了优先传送。只是在传输的数据量方面依旧如原书所指出，只能优先传递或者说读取最后一个字节（890中的0），其他数据依旧按照普通方式传送。

为什么只传递了一个字节的紧急信息，需要从TCP数据包的包头信息说起。函数`send(sock, "890", strlen("890"), MSG_OOB)`使用`MSG_OOB`设置了一个紧急指针，位于实际传输数据的下一个位，即从第二个参数所指定的传输数据（这里为"890"）第一位开始向右的由第三个参数（这里为3）指定的位置上。除了紧急指针之前的一个字节外，其他数据依旧按照普通方式传送。

### 检查输入缓冲

同时使用MSG_PEEK和MSG_DONTWAIT选项，可以验证输入缓冲中是否存在待接收数据，以下使用程序代号44和45验证：
```
// 客户端程序运行结果
$ gcc 45_peek_recv.c -o peek_recv 
$ ./peek_recv 9091
recv() with MSG_PEEK got 15 bytes: something there 
recv() without MSG_PEEK: something there 
the read buf is empty now!
// 客户端程序运行情况
$ gcc 44_peek_send.c -o peek_send 
$ ./peek_send 127.0.0.1 9091 "something there"
```

### `readv()`和`writev()`

在发送和接收数据时整合分散保存在缓存中的数据，可以减少后台调用I/O函数的次数，提升传输效率。
```
#include <sys/uio.h>
ssize_t writev(int filedes, const struct iovec * iov, int iovcnt);
ssize_t readv(int filedes, const struct iovec * iov, int iovcnt);
```
* filedes 需要发送或接收的套接字文件描述符，也支持传输文件或标准输出输入
* iov iovec结构型数组的地址，包含待发送或待接收数据对象的地址和大小信息
* -> 该参数使用的结构型数组声明如下
```
struct iovec
{
    void * iov_base; // 缓冲地址
    size_t iov_len; // 缓冲大小
}
```
* iovcnt 前一个参数中的数组长度

演示程序代号46，验证过程如下：
```
$ gcc 46_writev_readv.c -o wrv
$ ./wrv
ABC1234
Write bytes: 7 
Input something for testing:
hi there, this is a testing message!
Read bytes: 37 
First message: hi there,  
Second message: this is a testing message!
```
可以查看46号C文件的源码，注意确认`iovec`结构体的定义和`writev`与`readv`函数各变量的应用。注意`writev`函数对`iovec`结构体中各个成员变量指定位数的输出是连续的，从结果看就像一条数据。`readv`函数在指定写入对象的`iovec`结构体中各成员及其位数后也是连续的写入所读取的数据。

由于可以从多个地址读取或写入数据，`readv`和`writev`函数可以减少函数的调用次数，减少数据包个数，使程序拥有更高的效率和更灵活的运行方式，只要适用就可以多多使用。

## 多播和广播

### 多播（Multicast）

基于UDP方式完成的，数据会同时传输到加入或注册到特定组的大量主机。
* 针对目标多播组，只需要发送一次数据
* 只发送一次，组内所有客户都会接收到
* 多播组数可以在IP地址范围内任意添加
* 加入组的客户即可接受发到该组的数据
多播组使用D类IP地址（224.0.0.0到239.255.255.255），多播使用UDP数据包，一次发送会被路由器传递到多播组内的所有主机，对同一个物理子网内的主机只需要发送一次数据即可。补充一点关于多播的知识，一些路由器不支持多播，一些会因为网络拥堵问题故意阻断多播，为解决这些问题可以使用隧道技术。多播基于MBone(Multicast Backbone)多播主干网工作，该网络建构在物理网络上，是一种通过特殊协议工作的软件虚拟网络。这里不必深究，我们只用使用它就可以。

多播数据包包含`TTL(Time TO Live)`生存时间信息，该值设置过高会影响网络流量，设置过小会影响数据包传输距离，需要注意。其他需要注意的代码部分包括对`IPPROTO_IP`协议下的`IP_MULTICAST_TTL`值的设置，对`IPPROTO_IP`协议下的`IP_ADD_MEMBERSHIP`选项的设置。以下创建发送和接受两个程序，代码47和48，演示多播信息的发送和接收：
```
// 首先打开接收端的程序
$ gcc 48_news_receiver.c -o mrecv
$ ./mrecv 224.1.1.2 9190
// 在发送端打开后，可以收到如下结果
1
2
3
a
b
c
4
5
d
e
e
// 再打开发送端程序
$ gcc 47_news_sender.c -o msender
$ ./msender 224.1.1.2 9190 echomsg.txt 
```
查看源码可以了解，发送端的程序与普通UDP套接字使用没有太大区别，只是需要在套接字选项中指定多播数据包的生存时间、使用`sendto`函数发送数据、且在运行时地址需要使用多播地址。在接收端，涉及到多播地址的设定和多播组的加入，有点意思。

首先是多播组的加入，是通过对套接字选项的设置完成的，具体方法如下：
```
setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
    (void*)&join_addr, sizeof(join_addr));
```
以上代码中出现的唯一变量是`join_addr`，其中包含了需要加入到的多播组IP地址，和需要加入的本地主机的IP地址。该变量使用`ip_mreq`结构类型创建，定义如下：
```
struct ip_mreq
{
    struct in_addr imr_multiaddr; // 要加入到的多播组IP地址
    struct in_addr imr_interface; // 要加入的本地主机IP地址
}
```
注意到以上验证程序中使用了`sendto()`和`recvfrom()`函数，允许指定数据接收方或返回通知到数据发送方，一般用作发送UDP数据包，而`send()`和`recv()`函数用作发送TCP数据包。

### 广播（Broadcast）

同样是一次性向多个主机发送数据，多播可以跨越网络送达加入多播组的指定主机，广播只能送达同一网络中的所有主机。广播可以有两种类型：直接广播和本地广播。直接广播向指定网络发送广播，例如对网络地址为192.12.34的主机发送广播，可以使用广播地址192.12.34.255。本地广播向主机所在网络发送广播，例如本地主机所在网络为192.12.34，可以用通用的本地广播地址255.255.255.255向同网络内的所有主机发送广播。

这里使用代号为49和50的程序演示广播：
```
// 先运行接收端程序
$ gcc 50_news_receiver_brd.c -o breceiver
$ ./breceiver 9190.
// 发送端程序运行后会收到如下内容
1
2
3
a
b
c
4
5
d
e
e
// 再运行发送端
$ gcc 49_news_sender_brd.c -o bsender
$ ./bsender 255.255.255.255 9190 echomsg.txt
```
具体代码没有特别需要解释的，只是这里发送端的实施方式上与原书有一点不同，将需要发送的文件作为第三个参数传入，之前多播的代码中也是做了同样更改。再就是广播也需要设置套接字选项，与多播设置加入多播组类似，这里需要设置开启广播功能。具体可以参考C文件内的注释。

## 标准I/O

从这里开始原书进入第二部分，即Linux编程。之前使用的`send()`和`read()`等函数为系统函数，这里提到的标准函数是C语言中使用的诸如`fopen()`，`feof()`，`fgetc()`，`fputs()`等。标准函数具有良好的移植性，并且可以通过利用缓冲提高运行性能。

在使用系统函数时，系统已经为套接字设置了缓冲，但此缓冲并不是为了提高性能而是为满足TCP协议的健壮性要求，以便在数据丢失或损毁时可以重新发送。标准函数提供的缓冲主要是为了提高性能，特别是需要传输的数据量较多或需要多次读取或发送数据时。总的来说，缓冲可以减少零散数据包的处理和发送次数，从而减少冗余包头信息的写入，节省移动数据的总时间。

这里用两个小程序代号51和52验证系统和标准函数的拷贝效率：
```
// 使用系统函数拷贝文件
$ gcc 51_syscpy.c -o syscpy
$ ./syscpy 
Usage : ./syscpy <FROM_FILE> <TO_FILE>
$ time ./syscpy README.md README.cp 
real	0m0.089s
user	0m0.013s
sys	0m0.073s
// 使用标准函数拷贝文件
$ gcc 52_stdcpy.c -o stdcpy
$ time ./stdcpy README.md README.cp 
real	0m0.012s
user	0m0.006s
sys	0m0.003s
```
通过以上示例可以看到标准函数在复制效率上是相对较高的，但标准函数也有缺点或限制条件：
* 不容易进行双向通讯，如果希望同时读写需要以`r+`，`w+`或`a+`模式打开文件
* 可能频繁调用fflush函数，用来切换读写工作状态，会影响缓冲性能
* 需要使用FILE结构体指针来返回文件描述符，但创建套接字时会直接返回文件描述符

为转换文件描述符为FILE结构体类型的指针，使用`fdopen()`函数：
```
#include <stdio.h>
FILE * fdopen(int fildes, const char * mode);
```
* 以上函数成功时返回FILE结构体类型指针，失败时返回NULL
* filedes 需要做转换的文件描述符
* mode 要创建的FILE结构体指针模式，与`fopen`函数相同的`w`或`r`模式

这里使用一个小程序，代号53，验证`fdopen`函数的使用：
```
$ gcc 53_desto.c -o desto
$ ./desto
Usage : ./desto <FILE>
$ ./desto data.txt
$ cat data.txt
cat: data.txt: Permission denied
$ sudo cat data.txt
Network C programming 
```
具体关于FILE指针的应用可以查看C文件源码和注释，这里可以看到新建的文本文件由于从系统函数创建，对普通用户没有操作权限，需要配合`sudo`命令提升权限后才可以查看内容。

与`fdopen()`函数功能相反，`fileno()`函数可以将FILE指针转换为文件描述符：
```
#include <stdio.h>
int fileno(FILE * stream);
```
* 成功返回转换后的文件描述符，失败返回-1
以下使用代号54的小程序验证：
```
$ gcc 54_todes.c -o todes
$ ./todes
Usage : ./todes <FILE>
$ ./todes data.txt
original file descriptor: 3 
converted file descriptor: 3 
```
具体程序的说明见C文件的源码和注释，下面使用以上函数更新回声服务器和客户端，验证如下：
```
// 先运行服务器端程序
$ gcc 55_echo_stdserv.c -o stdserv
$ ./stdserv 
Usage : ./stdserv <port>
$ ./stdserv 9190
Connected client 1 
^C
// 再运行客户端程序
$ gcc 56_echo_stdclnt.c -o stdclnt
$ ./stdclnt 127.0.0.1 9190
Connected......
Input message(Q to quit): hi
Message from server : hi
Input message(Q to quit): bye
Message from server : bye
Input message(Q to quit): q
```
由于标准I/O函数提供了额外的缓冲，在信息发送量较小时需要使用`fflush()`函数手动触发发送动作。

## I/O流分离

流（Stream）即数据传输的路径，通过分开输入和输出流可以降低相应功能的实现难度，同时也会提升各自的操作效率。之前有通过`fork()`函数复制文件描述符，使用不同的文件描素符分别作发送和接收数据用途，也有用`fdopen()`函数创建分别用于发送和节搜数据的FILE指针，利用标准函数的缓冲提高数据传输性能。

在使用`fork()`函数复制文件描述符时，对于发送信息的客户端，可以在发送终止会话的`q`或`Q`信息后调用`shutdown(sock, SHUT_WR);`方法半关闭套接字，这也是正确的半关闭方法。但在用`fdopen()`函数时，虽然有读写功能分开的FILE指针，但通过`fclose()`方法将其分别关闭时却并不能达到半关闭效果，使用小程序代号57和58验证如下：
```
// 先运行服务器端
$ gcc 57_sep_serv.c -o sepserv
$ ./sepserv 9190
$ 
// 再运行客户端
$ gcc 58_sep_clnt.c -o sepclnt
$ ./sepclnt 
Usage : ./sepclnt <IP> <port>
$ ./sepclnt 127.0.0.1 9190
FROM SERVER: Hi~ client? 
I love all of the world 
You are awesome! 
$
```
以上程序中，在服务器与客户端的套接字建立后，双方分别分流I/O到两个各自负责读取和写入操作的FILE结构体对象。服务器向客户端发送了三条消息后尝试半关闭负责写入操作的FILE结构体，保持负责接收信息的FILE结构体，看能否收到来自客户端的最后信息，确认之前的半关闭操作是否有效。从以上结果来看，服务器没有收到客户端的最后信息，之前的半关闭操作失败。

这是由于服务器端创建的两个分别负责读写的FILE结构体指向同一个文件描述，即同一个套接字。由于一个套接字可以关联多个文件描述符，这里可以重新实施FILE结构体，将其指向到关联到同一套接字的不同文件描述符，如此即可实现半关闭读写的数据流通道。与之前使用`fork()`函数复制进程的操作不同，用`dup()`和`dup2()`函数可以在同一个进程内复制文件描述符。
```
#include <unistd.h>
int dup(int fildes);
int dup2(int fildes, int fildes_num);  
```
* 以上函数成功返回复制的文件描述符，失败返回-1
* fildes 需要复制的文件描述符
* fildes_num 指定的文件描述符整数值
验证`dup()`函数基本功能，使用小程序代号59，过程如下：
```
$ gcc 59_dup.c -o dup
$ ./dup
fd1=3, fd2=7 
From the 1st copied fildes! 
From the 2nd copied fildes! 
From the original fildes! 
```
该程序通过`dup()`和`dup2()`分别从标准输出复制了文件描述符，用复制的文件描述符输出两条信息，关闭复制的文件描述符，再从标注输出的文件描述符输出第三条信息，关闭标准输出的文件描述符，再尝试从标准输出的文件描素符输出。其中最后一条信息由于通道提前关闭而没能完成输出。

使用类似的方法改进之前的代号57程序，在更新的代号60程序中，不是从同一个套接字的文件描述符创建FILE指针，而是在使用`dup()`函数复制文件描述符后再创建指针，且在服务器发送完信息后调用`shutdown()`函数半关闭通道并发送EOF给客户端，主要的更新就在以上两点，验证如下：
```
// 服务器端
$ gcc 60_sep_serv2.c -o sepserv2
$ ./sepserv2 9190
FROM CLIENT: Thank you! 
$ 
// 客户端
$ ./sepclnt 127.0.0.1 9190
FROM SERVER: Hi~ client? 
I love all of the world 
You are awesome! 
```
如上，可以看到服务器最终可以接收到来自客户端的最后一条信息。验证了`dup()`函数复制的文件描述符有效，通道半关闭的实施是成功的。