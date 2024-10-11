#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //memset()
#include <sys/socket.h>
#include <unistd.h> //read(), close()

/*
USAGE : Server(node, servPort)
    This program starts up a server that communicate through sockets.
    The argments node and servPort will be directly passed to getaddrinfo() 
    as the 1st and 2nd argment respectively.
    The argment servPort is the port this program will wait on, 
    and servPort needs to be an integer between 0 and 65535.
EXAMPLE : Server(localhost, 8080)
*/


int main(int argc, char* argv[]){
    int serverSock; //server socket file discriptor
    int clientSock; //client socket file discriptor
    struct sockaddr_in serverSockAddr; //server internet socket address
    struct sockaddr_in clientSockAddr; //client internet socket address
    struct addrinfo hints;
    struct addrinfo *results;
    struct addrinfo *res;
    char buf[1024];
    int numOfBytesRead;
    long serverPort;
    unsigned int clientLength;

    if ( argc != 3) {
        perror("mismatch in the number of argument(should be 2)");
        exit(1);
    }

/*
    char *endptr;
    serverPort = strtol(argv[2], &endptr, 10); // will be casted to unsigned short in the call of htons()
    //if some part of argment is invalid or if port is not in the range of unsigned short(0~65535)
    if (*endptr != '\0' || port <= 0 || port > 65535) { 
        printf("Invalid port number: %s\n", argv[1]);
        exit(1);
    }
*/

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int statusOfGetaddrinfo = getaddrinfo(argv[1], argv[2], &hints, &results);
    if (statusOfGetaddrinfo != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusOfGetaddrinfo));
        exit(1);
    }

    /*  getaddrinfo() returns a list of address structures.
        Try each address until we successfully bind(2).
        If socket(2) (or bind(2)) fails, we (close the socket
        and) try the next address. */
    int statusOfBind;
    for (res = results; res != NULL; res = res->ai_next){
        serverSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (serverSock == -1){ continue; }

        statusOfBind = bind(serverSock, res->ai_addr, res->ai_addrlen);
        if (statusOfBind == 0){ break; }

        close(serverSock); 
    }
    freeaddrinfo(results);           /* No longer needed */
    if (res == NULL) { //No address succeeded
        perror("binding");
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
                numOfBytesRead = read(clientSock, buf, sizeof(buf));
                
                // for test
                printf("content : %s\n", buf); 
                
                if (numOfBytesRead == -1)
                    perror("reading stream message");
                if (numOfBytesRead == 0){
                    printf("Ending connection\n");
                }
            } while (numOfBytesRead > 0);
            //close(clientSock);
        }
    }

    exit(0);
}
