#include "main.h"


int main(int argc, char **argv){
    tcpserver::Server server(2000);
    int c;
    if((c = server.start()) < 0){
        std::cout << "Eh " << c << std::endl;
        return -1;
    }
    struct tcpserver::packet *ldata, *rdata;
    tcpserver::Client *leftcam, *rightcam;
    while(server.running){
        while(server.getNumConnections() == 2){
            leftcam = server.getConnByName("LEFT");
            rightcam = server.getConnByName("RIGHT");
            //restituisce il pacchetto come puntatore a struttura
            if((leftcam != NULL) && (rightcam != NULL)){
                //std::cout << "Connessione completa" << std::endl;
                ldata = leftcam->getData();
                rdata = rightcam->getData();
                if(rdata != NULL){
                    //arriva il messaggio fin qui
                    std::cout << (char *)rdata << std::endl;
                }
            }
            usleep(1000);
            //std::cout << ((char *)rdata)[0];
        }
        usleep(100);
    }
    return 0;
}
/*
int main(int argc, char **argv){
    tcpserver::Server server(2000);
    int c;
    if(c = server.start() < 0){
        std::cout << "Eh " << c << std::endl;
        return -1;
    }
    while(server.running){
        while(connessioni diverse da due){
            pass
        }
        //inizia routine di sincronizzazione
        syncroutine(&server);
        
    }
    return 0;
}
*/

//avvia server
//attendi due connessioni, identificabili da una stringa data dal client
//verifica il ritardo di propagazione richiedendo il time locale della macchina per sincronizzarle
//richiedi l'invio ogni 1/30esimo di secondo
//ottieni i due flussi di dati e convertili usando mat cv
//mostrali al display

//pt2
// dobbiamo rendere disponibili i pacchetti in arrivo, opzioni:
// 1)mettiamo i pacchetti in un array del server con la tecnica fifo e scriviamo una funzione di ricezione da chiamare ricorsivamente
// 2)inviamo direttamente dopo la ricezione il pacchetto ad una funzione definita dall'utente
// 3)inviamo i pacchetti ad un altro modulo del programma che si occupa di gestirli, dovremmo però diversificare i vari client, con id
// proviamo la prima risoluzione