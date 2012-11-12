#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int agc, char** argv)
{
    int sock;
    //char message[] = "";
    char buf[1024];
    //int message = 44;



    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3420);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);//((((3 << 8) | 0) << 8) | 0) << 8) | 128;

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    printf("\nconnect open\n");

    memcpy(buf,argv[1],1024);
    send(sock,buf,sizeof(buf),0);
    //send(sock, message, sizeof(message), 0);
    //printf("send\n");
    while (1)
    {
        int bytes_read = recv(sock, buf, sizeof(buf), 0);
        if(bytes_read <= 0) break;

        //printf("recv\n");
        printf("%s",buf);
    }
    printf("end\n");
    close(sock);
}
