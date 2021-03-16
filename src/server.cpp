#include "server.h"

namespace tcpserver{
    Server::Server(uint16_t argport){
        running = 0;
        port = argport;
        memset((void *)&address, 0, sizeof(address));
    }
    Client::Client(){
        inPacket = outPacket = 0;
    }
    void *Client::addToQueue(packet *packet){
        if(inPacket == (( outPacket - 1 + QUEUESIZE) % QUEUESIZE)){
            return NULL; /* Queue Full*/
        }
        packetQueue[inPacket] = packet;
        inPacket = (inPacket + 1) % QUEUESIZE;
        return NULL; // No errors
    }
    packet *Client::getData(){
        packet *data;
        if(inPacket == outPacket){
            return NULL; /* Queue Empty - nothing to get*/
        }
        data = packetQueue[outPacket];
        outPacket = (outPacket + 1) % QUEUESIZE;
        return data; // No errors
    }
    void* Server::thread(void *ref){
        struct threadData *thdata = (struct threadData *)ref;
        std::cout << "Thread in apertura" << std::endl;
        switch(thdata->cmd){
            case 'S':
                ((Server*)(thdata->serverRef))->mainthread();
                break;
            case 'C':
                ((Server*)(thdata->serverRef))->clientHandler(thdata->cfd);
                break;
            default:
                std::cout << "Comando non riconosciuto" << std::endl;
                break;
        }
        pthread_exit(NULL);
    }
    int Server::start(){
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[BUFFERSIZE];
        if((sfd = socket(AF_INET,SOCK_STREAM,0)) == 0){
            return -1;
        }
        if(setsockopt(sfd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            return -2;
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if(bind(sfd, (struct sockaddr *)&address, sizeof(address))<0){
            return -3;
        }
        if(listen(sfd,3) < 0){
            return -4;
        }
        pthread_t mainthread;
        struct threadData thdata;
        thdata.cmd = 'S';
        thdata.serverRef = this;
        int th = pthread_create(&mainthread,NULL,thread,(void*)&thdata);
        running = 1;
        return 0;
    }
    void* Server::mainthread(){
        while(1){
            int cfd;
            //struct sockaddr* addr = (struct sockaddr*)&address;
            struct sockaddr_in clientAddr;
            int clilen = sizeof(clientAddr);
            memset((void *)&clientAddr, 0, sizeof(clientAddr));
            socklen_t *addrlen = (socklen_t *)&clilen;
            if((cfd = accept(sfd,(struct sockaddr *)&clientAddr,addrlen) )< 0){
                continue;
            }
            std::cout << cfd << std::endl;
            pthread_t threadRef;
            struct threadData thdata;
            thdata.cmd = 'C';
            thdata.serverRef = this;
            thdata.cfd = cfd;
            int th = pthread_create(&threadRef,NULL,thread,(void *)&thdata);
            //running = 0;
        }
    }
    Client *Server::getConnByName(const char *name){
        uint8_t i;
        void *conn;
        for(i = 0; i < MAXCLIENT; i++){
            if(pool[i]==NULL) continue;
            if(strcmp(pool[i]->name, name) == 0){
                return pool[i];
            }
        }
        return NULL;
    }

    uint8_t Server::getNumConnections(){
        uint8_t i;
        uint8_t cont = 0;
        for(i = 0; i < MAXCLIENT; i++){
            if(pool[i] != NULL){
                cont += 1;
            }
        }
        return cont;
    }
    void Server::clientHandler(int cfd){
        bool pending = 0;
        uint32_t datalen, value, index;
        char *data = (char *)calloc(BUFFERSIZE, 1);
        const char *syncreq = "SYNC";
        char name[10] = {0};
        struct timeval now, then;
        gettimeofday(&now,NULL);
        send(cfd,syncreq,strlen(syncreq),0);
        read(cfd,name, 10);
        gettimeofday(&then,NULL);
        unsigned long delay = (long)(then.tv_usec - now.tv_usec);
        std::cout << name << std::endl;
        Client state;
        memcpy(&(state.name),name,10);
        bool enomem = 1;
        int poolIndex;
        for(poolIndex = 0; poolIndex < MAXCLIENT; poolIndex++){
            if(pool[poolIndex] == NULL){
                pool[poolIndex] = &state;
                enomem = 0;
                break;
            }
        }
        if(enomem) {
            //raise error
            return;
        }
        std::cout << poolIndex << std::endl;
        //invia richiesta nome send(richiesta)
        //forse conviene inserire un magic number per evitare il sovraccarico
        while(running){
            char buffer[BUFFERSIZE];
            if(!pending){
                char len[4];
                if(read(cfd,len,4) <= 0){
                    std::cout << "errore" << std::endl;
                    free(pool[poolIndex]);
                    pool[poolIndex] = NULL;
                    break;
                }
                datalen = *((uint32_t *)len);
                memset(len,0,4);
                index = datalen;
                if((datalen > 10000000) || (datalen == 0)){
                    std::cout << "Size Issue" << std::endl;
                    break;
                }
                std::cout << "Data Lenght: " << datalen << std::endl;
                //allocazione memoria per il messaggio
                data = (char *)realloc(data,(size_t)datalen);
                pending = 1;
            }
            value = BUFFERSIZE > index ? index : BUFFERSIZE;
            read(cfd,buffer,value);
            memcpy(data+(datalen-index),buffer,value); //problema attuale
            index -= value;
            //std::cout << data << " " << datalen << " " << index << std::endl;
            memset(&buffer,0,BUFFERSIZE);
            if(index <= 0 ){
                pending = 0;
                std::cout << "Blocco Ricevuto" << std::endl;
                //qui costruiamo il pacchetto da inserire nel pool, deve essere riferito a questo client
                char *newpacket = (char *)calloc(datalen,sizeof(char));
                memcpy(newpacket, data, datalen);

                struct packet *finalpacket = (struct packet *)newpacket;

                state.addToQueue(finalpacket);
                //ogni client ha come riferimento uno stato clientstate
                //free(data);
            }
            //send(cfd,c,6,0);
            //running = 0;
        }
    }
}