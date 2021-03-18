#include "client.h"

int main(int argc, char **argv){
    tcpclient::Client client("LEFT","127.0.0.1",2000);
    usleep(1000000);
    return 0;
}