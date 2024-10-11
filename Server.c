#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //memset()
#include <sys/socket.h>
#include <unistd.h> //read(), close()

#define BUFF_SIZE 1024
#define LIMIT_LISTEN_QUE 5

void Usage();
void CheckArguments(const int argc, char * argv[]);
void SetHintsForServer(struct addrinfo *hints);
void TryBindingAndCheck(struct addrinfo* const results, int *socketFDForServer);
void TryAcceptingAndCheck(const int serverSock, struct sockaddr *clientSockAddr, int *clientSockFD);

int main(int argc, char* argv[]){
    int serverSock; 
    int clientSock; 
    struct sockaddr_in serverSockAddr; 
    struct sockaddr_in clientSockAddr; 
    struct addrinfo hints;
    struct addrinfo *results;
    char buf[BUFF_SIZE];
    int numOfBytesRead;
    //unsigned int clientLength;

    CheckArguments(argc, argv);

    SetHintsForServer(&hints);

    int statusOfGetaddrinfo = getaddrinfo(argv[1], argv[2], &hints, &results);
    if (statusOfGetaddrinfo != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusOfGetaddrinfo));
        exit(1);
    }

    TryBindingAndCheck(results, &serverSock);

    freeaddrinfo(results); // No longer needed

    int statusOfListen = listen(serverSock, LIMIT_LISTEN_QUE);
    if (statusOfListen != 0) {
        perror("listening on a socket");
        exit(1);
    }
    
    while(1){
        TryAcceptingAndCheck(serverSock, (struct sockaddr *) &clientSockAddr, &clientSock);

        do{
            memset(buf, 0, sizeof(buf));
            numOfBytesRead = read(clientSock, buf, sizeof(buf));
                
            printf("content : %s\n", buf); // just for a test
                
            if (numOfBytesRead == -1)
                perror("reading stream message");
            if (numOfBytesRead == 0){
                printf("Ending connection\n");
            }
        } while (numOfBytesRead > 0);
    }

    exit(0);
} // end of main()







void Usage(){
    printf("--------------------------------------------------------------------------- \n");
    printf("USAGE : Server(node, servPort) \n");
    printf("    This program starts up a server that communicate through sockets. \n");
    printf("    The argments node and servPort will be directly passed to getaddrinfo() \n");
    printf("    as the 1st and 2nd argment respectively. \n");  
    printf("    The argment servPort is the port this program will wait on, \n" );
    printf("    and servPort needs to be an integer between 0 and 65535. \n");
    printf("EXAMPLE : Server(localhost, 8080) \n");
    printf("--------------------------------------------------------------------------- \n");
}

void CheckArguments(const int argc, char* argv[]){
    if ( argc != 3) {
        perror("mismatch in the number of argument(should be 2)");
        Usage();
        exit(1);
    }

    char *endptr;
    long serverPort = strtol(argv[2], &endptr, 10);
    //if some part of argment is invalid or if port is not in the correct range(0~65535)
    if (*endptr != '\0' || serverPort <= 0 || serverPort > 65535) { 
        printf("Invalid port number: %s\n", argv[2]);
        Usage();
        exit(1);
    }
}

void SetHintsForServer(struct addrinfo *hints){
    memset(hints, 0, sizeof(hints));
    hints->ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints->ai_protocol = 0;          /* Any protocol */
    hints->ai_canonname = NULL;
    hints->ai_addr = NULL;
    hints->ai_next = NULL;
}

void TryBindingAndCheck(struct addrinfo * const results, int *socketFDForServer){
    /*  getaddrinfo() returns a list of address structures.
        Try each address until we successfully bind(2).
        If socket(2) (or bind(2)) fails, we (close the socket
        and) try the next address. */

    int statusOfBind;
    struct addrinfo *res;

    for (res = results; res != NULL; res = res->ai_next){
        *socketFDForServer = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (*socketFDForServer == -1)
            continue;

        statusOfBind = bind(*socketFDForServer, res->ai_addr, res->ai_addrlen);
        if (statusOfBind == 0)
            break;

        close(*socketFDForServer); 
    }

    if (res == NULL) { //No address succeeded
        perror("binding");
        exit(1);
    }
}

void TryAcceptingAndCheck(const int serverSock, struct sockaddr *clientSockAddr, int *clientSockFD){
    unsigned int clientLength = sizeof(clientSockAddr); //accept() の出力を受け取るらしいがローカル変数で良いのか？
    *clientSockFD = accept(serverSock, (struct sockaddr *) &clientSockAddr, &clientLength);
    if (*clientSockFD == -1){
        perror("accepting");
        exit(1);
    }
}
