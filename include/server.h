#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <iostream> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
namespace tcpserver{
    #define MAXCLIENT 2
    #define QUEUEELEMENTS 10
    #define QUEUESIZE (QUEUEELEMENTS+1)
    #define BUFFERSIZE 4096
    typedef struct clientstate clientstate;
    
    struct packet{ //da rivedere
        uint32_t lenght;
        char *data;
    };

    struct clientstate{
        pthread_t threadRef;
        char name[10] = {0};
    };
    struct threadData{
        unsigned char cmd;
        void *serverRef;
        int cfd;
    };
    class Client{
    public:
        char name[10] = {0};
        packet *packetQueue[QUEUESIZE];
        unsigned int inPacket, outPacket;
        void *addToQueue(packet *packet);
        packet *getData();
        Client();
    };
    class Server{
    public:
        uint16_t port, magicNum = 1337;
        struct sockaddr_in address;
        int sfd;
        bool running;
        Client *pool[MAXCLIENT] = {NULL};
        Server(uint16_t argport);
        int start();
        static void *thread(void *arg);
        void *mainthread();
        uint8_t getNumConnections();
        void clientHandler(int cfd);
        Client *getConnByName(const char *);
    };
}