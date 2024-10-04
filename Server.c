#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h> //memset()
#include <unistd.h> //read()
#include <stdlib.h>

/*
usage : Server(, servPort)
*/


int main(int argc, char* argv[]){
    int serverSock; //server socket file discriptor
    int clientSock; //client socket file discriptor
    struct sockaddr_in serverSockAddr; //server internet socket address
    struct sockaddr_in clientSockAddr; //client internet socket address
    char buf[1024];
    int rval;
    unsigned short serverPort;

    if ( argc != 2) {
        perror("mismatch in the number of argument(should be 2)");
        exit(1);
    }

    serverPort = (unsigned short) atoi(argv[1]);
    if (serverPort = -1) {
        perror("invalid port number");
        exit(1);
    }

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == -1){
        perror("opening stream socket");
        exit(1);
    }

    memset(&serverSockAddr, 0, sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    serverSockAddr.sin_port = htons(serverPort);

    int bindSucceed = bind(serverSock, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)); //第２引数のキャストはなぜ必要?
    if (bindSucceed == -1){
        perror("binding stream socket");
        exit(1);
    }

    listen(serverSock, 5); //set que limit to be 5
    
    while(1){
        clientSock = accept(serverSock, (struct sockaddr *) &clientSockAddr, (unsigned int *) sizeof(clientSockAddr)); //3つ目の引数が正しいか不安
        if (clientSock == -1){
            perror("accepting");
            exit(1);
        } else { 
            do{
                memset(buf, 0, sizeof(buf));
                if ((rval = read(clientSock, buf, sizeof(buf))) == -1)
                    perror("reading stream message");
                if (rval == 0)
                    printf("Ending connection\n");
            } while (rval > 0);

            close(clientSock);
        }
    }

    exit(0);
}