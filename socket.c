    /*************************************************************************
		基于TCP流协议的socket网络文件传输Demo：

		实现：C语言
		功能：文件传输（可以传任何格式的文件）
        > File Name: Server.c
        > Author: SongLee
        > E-mail: lisong.shine@qq.com
        > Created Time: 2014年03月13日 星期四 22时17分43秒
        > Personal Blog: http://songlee24.github.io/
     ************************************************************************/
     
    #include<netinet/in.h>  // sockaddr_in
    #include<sys/types.h>   // socket
    #include<sys/socket.h>  // socket
    #include<stdio.h>       // printf
    #include<stdlib.h>      // exit
    #include<string.h>      // bzero
     
    #define SERVER_PORT 8000
    #define LENGTH_OF_LISTEN_QUEUE 20
    #define BUFFER_SIZE 1024
    #define FILE_NAME_MAX_SIZE 512
     
    int main(void)
    {
        // 声明并初始化一个服务器端的socket地址结构
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
        server_addr.sin_port = htons(SERVER_PORT);
     
        // 创建socket，若成功，返回socket描述符
        int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
        if(server_socket_fd < 0)
        {
            perror("Create Socket Failed:");
            exit(1);
        }
        int opt = 1;
        setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
     
        // 绑定socket和socket地址结构
        if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))))
        {
            perror("Server Bind Failed:");
            exit(1);
        }
        
        // socket监听
        if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE)))
        {
            perror("Server Listen Failed:");
            exit(1);
        }
     
        while(1)
        {
            // 定义客户端的socket地址结构
            struct sockaddr_in client_addr;
            socklen_t client_addr_length = sizeof(client_addr);
     
            // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信
            // accept函数会把连接到的客户端信息写到client_addr中
            int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length);
            if(new_server_socket_fd < 0)
            {
                perror("Server Accept Failed:");
                break;
            }
     
            // recv函数接收数据到缓冲区buffer中
            char buffer[BUFFER_SIZE];
            bzero(buffer, BUFFER_SIZE);
            if(recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
            {
                perror("Server Recieve Data Failed:");
                break;
            }
     
            // 然后从buffer(缓冲区)拷贝到file_name中
            char file_name[FILE_NAME_MAX_SIZE+1];
            bzero(file_name, FILE_NAME_MAX_SIZE+1);
            strncpy(file_name, buffer, strlen(buffer)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buffer));
            printf("%s\n", file_name);
     
            // 打开文件并读取文件数据
            FILE *fp = fopen(file_name, "r");
            if(NULL == fp)
            {
                printf("File:%s Not Found\n", file_name);
            }
            else
            {
                bzero(buffer, BUFFER_SIZE);
                int length = 0;
                // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
                while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
                {
                    if(send(new_server_socket_fd, buffer, length, 0) < 0)
                    {
                        printf("Send File:%s Failed./n", file_name);
                        break;
                    }
                    bzero(buffer, BUFFER_SIZE);
                }
     
                // 关闭文件
                fclose(fp);
                printf("File:%s Transfer Successful!\n", file_name);
            }
            // 关闭与客户端的连接
            close(new_server_socket_fd);
        }
        // 关闭监听用的socket
        close(server_socket_fd);
        return 0;
    }



    /*************************************************************************
        > File Name: Client.c
        > Author: SongLee
        > E-mail: lisong.shine@qq.com
        > Created Time: 2014年03月14日 星期五 09时41分46秒
        > Personal Blog: http://songlee24.github.io/
     ************************************************************************/
     
    #include<netinet/in.h>   // sockaddr_in
    #include<sys/types.h>    // socket
    #include<sys/socket.h>   // socket
    #include<stdio.h>        // printf
    #include<stdlib.h>       // exit
    #include<string.h>       // bzero
     
    #define SERVER_PORT 8000
    #define BUFFER_SIZE 1024
    #define FILE_NAME_MAX_SIZE 512
     
    int main()
    {
        // 声明并初始化一个客户端的socket地址结构
        struct sockaddr_in client_addr;
        bzero(&client_addr, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = htons(INADDR_ANY);
        client_addr.sin_port = htons(0);
     
        // 创建socket，若成功，返回socket描述符
        int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(client_socket_fd < 0)
        {
            perror("Create Socket Failed:");
            exit(1);
        }
     
        // 绑定客户端的socket和客户端的socket地址结构 非必需
        if(-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
        {
            perror("Client Bind Failed:");
            exit(1);
        }
     
        // 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == 0)
        {
            perror("Server IP Address Error:");
            exit(1);
        }
        server_addr.sin_port = htons(SERVER_PORT);
        socklen_t server_addr_length = sizeof(server_addr);
     
        // 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接
        if(connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
        {
            perror("Can Not Connect To Server IP:");
            exit(0);
        }
     
        // 输入文件名 并放到缓冲区buffer中等待发送
        char file_name[FILE_NAME_MAX_SIZE+1];
        bzero(file_name, FILE_NAME_MAX_SIZE+1);
        printf("Please Input File Name On Server:\t");
        scanf("%s", file_name);
     
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        strncpy(buffer, file_name, strlen(file_name)>BUFFER_SIZE?BUFFER_SIZE:strlen(file_name));
        
        // 向服务器发送buffer中的数据
        if(send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
        {
            perror("Send File Name Failed:");
            exit(1);
        }
     
        // 打开文件，准备写入
        FILE *fp = fopen(file_name, "w");
        if(NULL == fp)
        {
            printf("File:\t%s Can Not Open To Write\n", file_name);
            exit(1);
        }
     
        // 从服务器接收数据到buffer中
        // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
        bzero(buffer, BUFFER_SIZE);
        int length = 0;
        while((length = recv(client_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
        {
            if(fwrite(buffer, sizeof(char), length, fp) < length)
            {
                printf("File:\t%s Write Failed\n", file_name);
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }
     
        // 接收成功后，关闭文件，关闭socket
        printf("Receive File:\t%s From Server IP Successful!\n", file_name);
        close(fp);
        close(client_socket_fd);
        return 0;
    }
/*
---------------------
作者：神奕
来源：CSDN
原文：https://blog.csdn.net/lisonglisonglisong/article/details/22699675
版权声明：本文为博主原创文章，转载请附上博文链接！
*/