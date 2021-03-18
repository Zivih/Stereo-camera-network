#include "client.h"

namespace tcpclient{
    Client::Client(const char *connName, const char *ip, uint16_t port){
        if((sfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
            std::cout << "Errore di connessione" << std::endl;
            return;
        }
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        if(inet_pton(AF_INET, ip, &serverAddr.sin_addr) <= 0){
            std::cout << "Errore in fase di conversione dell'host" << std::endl;
            return;
        }
        if(::connect(sfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
            std::cout << "Connessione non riuscita" << std::endl;
            return;
        }
        connected = 1;
        memcpy(name, connName, (strlen(name)<=10) ? strlen(name) : 10 );
        ::send(sfd,connName,strlen(connName),0);
    }
}