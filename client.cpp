#pragma warning(disable:4996)  
#include "Winsock.h"
#include "windows.h"
#include "stdio.h"
#include "time.h"
#include <iostream>
#include<stdlib.h>
#include "client.h"

using namespace std;
#define RECV_PORT 4523
#define SEND_PORT 4623  
#pragma comment(lib, "wsock32.lib")
SOCKET sockclient;
char filename[20];                       //文件名  
sockaddr_in ServerAddr;                 //服务器地址
char rbuff[1024];                        //接收缓冲区  
char sbuff[1024];                       //发送缓冲区  
char InputIP[20];                       //存储输入的服务器IP

void welcome()
{
    cout << "                                                                                              " << endl
        << "                                                                                              " << endl
        << "                                                                                              " << endl
        << "                                                                                              " << endl
        << "          ##########    ###        ###     ###   ##########j       ##########  ############, D#########." << endl
        << "          ##########    ####       ###    ###,   ##########j       ##########  ############, D##########." << endl
        << "          ##D          i####       ###   i###    ###               ###              ##       D##      ###" << endl
        << "          ##D          ##W##;      ###   ###     ###               ###              ##       D##      ###" << endl
        << "          ##D          ## ###      ###  ###      ###               ###              ##       D##      D##" << endl
        << "          ##D         K##  ##      ### ###       ###               ###              ##       D##      ###" << endl
        << "          ##D         ##t  ###     ######L       #########         ###              ##       D##      ###" << endl
        << "          #########  ;##   ###     #######       #########         #########j       ##       D##########E" << endl
        << "          #########  ###   i##,    #######,      ###jjjjjj         #########j       ##       D#########E" << endl
        << "          ##D        ###    ###    #### ###      ###               ###              ##       D#######G" << endl
        << "          ##D       K##########    ###  .###     ###               ###              ##       D##" << endl
        << "          ##D       ###########    ###   W##D    ###               ###              ##       D##" << endl
        << "          ##D       ##D      ###   ###    ###    ###               ###              ##       D##" << endl
        << "          ##D      E##       W##   ###     ###   ###jjjjjjji       ###              ##       D##" << endl
        << "          ##D      ###        ##D  ###     W##W  ##########D       ###              ##       D##" << endl
        << "          ##D      ##t        ###  ###      ###  ##########D       ###              ##       D##" << endl
        << endl << endl;
    system("pause");
    system("cls");
}

void help()//帮助菜单
{
    cout << "                      weolcome to the menu of FAKE FTP              " << endl
        << "                 _____________________________________________       " << endl
        << "                  1.get:                                          " << endl
        << "                              function: download files                       " << endl
        << "                              usage   : get filename                         " << endl << endl
        << "                  2.put:                                          " << endl
        << "                              function: upload files                       " << endl
        << "                              usage   : put filename                         " << endl << endl
        << "                  3.pwd:                                          " << endl
        << "                              function: Displays the current absolute path" << endl
        << "                              usage   : pwd                        " << endl << endl
        << "                  4.ls:                                           " << endl
        << "                              function: Displays all files in the current directory" << endl
        << "                              usage   : ls                        " << endl << endl
        << "                  5.cd:       " << endl
        << "                              function: switch working Directory             " << endl
        << "                              usage: cd pathname             " << endl
        << "                  6.help       " << endl
        << "                  7.quit       " << endl
        << "                  _____________________________________________       " << endl;
}
void list(SOCKET sockfd)
{
    int nRead;
    while (true)
    {
        nRead = recv(sockclient, rbuff, 1024, 0); //接收服务端返回的
             
        if (nRead == SOCKET_ERROR)
        {
            printf("read response error!\n");
            exit(1);
        }
        if (nRead == 0)//数据读取结束        
            break;
        //显示数据   
        rbuff[nRead] = '\0';
        printf("%s", rbuff);
    }
}

int SendFile(SOCKET datatcps, FILE* file)
{
    printf(" sending file data..");
    for (;;)  //从文件中循环读取数据并发送客户端       
    {
        int r = fread(sbuff, 1, 1024, file);//fread函数从file文件读取1个1024长度的数据到sbuff，返回成功读取的元素个数            
        if (send(datatcps, sbuff, r, 0) == SOCKET_ERROR)
        {
            printf("lost the connection to client!\n");
            closesocket(datatcps);
            return 0;
        }
        if (r < 1024)                      //文件传送结束    
            break;
    }
    closesocket(datatcps);
    printf("done\n");
    return 1;
}

DWORD StartSock()//启动winsock
{
    WSADATA WSAData;
    char buf[20];
    memset(buf, 0, 20);// 清空buf
    WORD wVersionRequested;
    wVersionRequested = MAKEWORD(2, 2); //设置版本号
    if (WSAStartup(wVersionRequested, &WSAData) != 0)//加载winsock版本
    {
        printf("sock init fail!\n");
        return (-1);
    }
    if (strncmp(InputIP, buf, 20) == 0)
    {
        printf("Please enter the IP address of the connected host：");
        scanf("%s", &InputIP);
    }
    //设置地址结构
    ServerAddr.sin_family = AF_INET;//指定IP地址版本IPV4
    ServerAddr.sin_addr.s_addr = inet_addr(InputIP);//指定服务器IP
    ServerAddr.sin_port = htons(RECV_PORT);//设置端口号
    return(1);
}
  
DWORD CreateSocket()
{
    sockclient = socket(AF_INET, SOCK_STREAM, 0);//当socket函数成功调用时返回一个新的SOCKET(Socket Descriptor)
    if (sockclient == SOCKET_ERROR)
    {
        printf("sockclient create fail! \n");
        WSACleanup();
        return(-1);
    }
    return(1);
}
DWORD CallServer() //发送连接请求  
{
    CreateSocket();

    if (connect(sockclient, (struct  sockaddr*)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
    {
        printf("Connect fail \n");
        memset(InputIP, 0, 20); //连接失败则清空之前的输入
        return(-1);
    }
    return(1);
}

DWORD TCPSend(char data[])   //发送命令  
{
    int length;
    length = send(sockclient, data, strlen(data), 0);
    //send函数通过sockclient接口发送data里面的数据，发送成功返回发送的字节数  
    if (length <= 0)
    {
        printf("send data error ! \n");
        closesocket(sockclient);
        WSACleanup();
        return(-1);
    }
    return(1);
}

int main()
{
    welcome();
    while (1)
    {
        char messge1[10];           //定义输入要处理的文件名  
        char messge2[20];           //定义输入要处理的文件名
        char order[30];             //输入的命令    
        order[0] = '\0';
        char buff[80];              //用以存储经过字串格式化的order  
        FILE* fd;                   //File协议主要用于访问本地计算机中的文件，fd指针指向要访问的目标文件  
        FILE* fd2;
        int count;
        int sin_size = sizeof(ServerAddr);
        StartSock();
        if (CallServer() == -1)
            return main();          //发送连接请求失败，返回主函数

        printf("\nEnter your order（enther 'help' Enter the menu）:\n");
        memset(buff, 0, 80);            //清空数组   
        memset(messge2, 0, 20);
        memset(order, 0, 30);
        memset(messge1, 0, 10);
        memset(rbuff, 0, 1024);
        memset(sbuff, 0, 1024);

        scanf("%s", &messge1);//s%输入字符串
        if (strncmp(messge1, "get", 3) == 0)
            scanf("%s", &messge2);
        if (strncmp(messge1, "put", 3) == 0)
            scanf("%s", &messge2);
        if (strncmp(messge1, "cd", 2) == 0)
            scanf("%s", &messge2);
        strcat(order, messge1);         //把messge1加在order的末尾   
        strcat(order, " ");             //命令中间的空格    
        strcat(order, messge2);         //把messge2加在order的末尾     
        sprintf(buff, order);           //把调整格式的order存入buff

    //help  
        if (strncmp(messge1, "help", 4) == 0) {
            help();
        }


        if (strncmp(messge1, "quit", 4) == 0)
        {
            printf("                    欢迎再次进入FAKE_FTP，谢谢使用！\n");
            closesocket(sockclient);
            WSACleanup();
            return 0;
        }
        TCPSend(buff);//发送buff里面的数据        
        recv(sockclient, rbuff, 1024, 0);
        printf(rbuff);

        if (strncmp(rbuff, "get", 3) == 0)
        {
            fd = fopen(messge2, "wb");
            if (fd == NULL)
            {
                printf("open file %s for weite failed!\n", messge2);
                return 0;
            }
            while ((count = recv(sockclient, rbuff, 1024, 0)) > 0)
            {
                fwrite(rbuff, sizeof(rbuff), count, fd);
            }

            fclose(fd);
        }

        if (strncmp(rbuff, "put", 3) == 0)   //put
        {
            strcpy(filename, rbuff + 9);
            fd2 = fopen(filename, "rb");
            if (fd2)
            {
                if (!SendFile(sockclient, fd2)) {
                    printf("send failed!");
                    return 0;
                }
                fclose(fd2);
            }

            else
            {
                strcpy(sbuff, "can't open file!\n");
                if (send(sockclient, sbuff, 1024, 0))
                    return 0;
            }
        }

        if (strncmp(rbuff, "ls", 2) == 0)      //ls
        {
            printf("\n");
            list(sockclient);               //列出接受到的列表内容
        }
        if (strncmp(rbuff, "pwd", 3) == 0)
        {
            list(sockclient);               //列出接受到的内容--绝对路径
        }
        if (strncmp(rbuff, "cd", 2) == 0) {}      //cd

        closesocket(sockclient);            //关闭连接
        WSACleanup(); 
    } 
    return 0;
}