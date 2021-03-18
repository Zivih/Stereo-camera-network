#include <stdio.h>
#include <iostream> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

namespace tcpclient{
    class Client{
    public:
        bool connected = 0;
        int sfd;
        char name[10];
        struct sockaddr_in serverAddr;
        Client(const char *connName, const char *ip, uint16_t port);
        int send();
        void *connect(const char *ip, uint16_t port);
    };
}