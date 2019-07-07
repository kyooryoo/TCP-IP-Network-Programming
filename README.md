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