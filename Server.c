#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h> //memset()
#include <unistd.h> //read()
#include <stdlib.h>

/*
USAGE : Server(char servAddress, int servPort)
    This program starts up a server that communicate through sockets.
    The argment servPort is the port this program will wait on, 
    and servPort needs to be an integer between 0 and 65535
EXAMPLE : Server(8080)
*/


int main(int argc, char* argv[]){
    int serverSock; //server socket file discriptor
    int clientSock; //client socket file discriptor
    struct sockaddr_in serverSockAddr; //server internet socket address
    struct sockaddr_in clientSockAddr; //client internet socket address
    char buf[1024];
    int rval;
    long serverPort;
    unsigned int clientLength;

    if ( argc != 3) {
        perror("mismatch in the number of argument(should be 2)");
        exit(1);
    }

    char *endptr;
    serverPort = strtol(argv[2], &endptr, 10); // will be casted to unsigned short in the call of htons()
    //if some part of argment is invalid or if port is not in the range of unsigned short(0~65535)
    if (*endptr != '\0' || port <= 0 || port > 65535) { 
        printf("Invalid port number: %s\n", argv[1]);
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

    int statusOfBind = bind(serverSock, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)); //第２引数のキャストはなぜ必要?
    if (statusOfBind != 0){
        perror("binding stream socket");
        exit(1);
    }

    int statusOfListen = listen(serverSock, 5); //set que limit to be 5
    if (statusOfListen != 0) {
        perror("listening on a socket");
        exit(1);
    }
    
    while(1){
        clientLength = sizeof(clientSockAddr);
        clientSock = accept(serverSock, (struct sockaddr *) &clientSockAddr, &clientLength);
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
