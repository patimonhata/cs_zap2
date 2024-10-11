#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> //memset()
#include <sys/socket.h>
#include <unistd.h> //read(), close()

void Usage();
void CheckArguments(const int argc, char * argv[]);
void SetHintsForClient(struct addrinfo *hints);
void TryConnectingAndCheck(struct addrinfo* const results, int *socketFDForClient);

int main(int argc, char* argv[]){
    int clientSocket;
    struct addrinfo hints;
    struct addrinfo *results;

    CheckArguments(argc, argv);

    SetHintsForClient(&hints);

    int statusOfGetaddrinfo = getaddrinfo(argv[1], argv[2], &hints, &results);
    if (statusOfGetaddrinfo != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusOfGetaddrinfo));
        exit(1);
    }

    TryConnectingAndCheck(results, &clientSocket);

    freeaddrinfo(results);           // No longer needed

    const char *message = "Can you hear me?";
    if (send(clientSocket, message, strlen(message), 0) == -1) {
        perror("send");
    }

    close(clientSocket);

    exit(0);
} //end of main()





void Usage(){
    printf("--------------------------------------------------------------------------- \n");
    printf("USAGE : Client(node, portConnecting) \n");
    printf("    This program starts up a client that communicate through sockets. \n");
    printf("    The argments node and portConnecting will be directly passed to getaddrinfo() \n");
    printf("    as the 1st and 2nd argment respectively. \n");  
    printf("    The argment portConnecting is the port this program will try to connect, \n" );
    printf("    and portConnecting needs to be an integer between 1 and 65535. \n");
    printf("EXAMPLE : Client(localhost, 8080) \n");
    printf("--------------------------------------------------------------------------- \n");
}

void CheckArguments(const int argc, char* argv[]){
    if ( argc != 3) {
        printf("mismatch in the number of argument(should be 2) \n");
        Usage();
        exit(1);
    }

    char *endptr;
    long portConnecting = strtol(argv[2], &endptr, 10);
    //if some part of argment is invalid or if port is not in the correct range(1~65535)
    if (*endptr != '\0' || portConnecting <= 0 || portConnecting > 65535) { 
        printf("Invalid port number: %s\n", argv[2]);
        Usage();
        exit(1);
    }
}

void SetHintsForClient(struct addrinfo *hints){
    memset(hints, 0, sizeof(*hints));
    hints->ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = 0;
    hints->ai_protocol = 0;          /* Any protocol */
}

void TryConnectingAndCheck(struct addrinfo* const results, int *socketFDForClient){
    /*  getaddrinfo() returns a list of address structures.
        Try each address until we successfully connect(2).
        If socket(2) (or connect(2)) fails, we (close the socket
        and) try the next address. */

    int statusOfConnect;
    struct addrinfo *res;

    for (res = results; res != NULL; res = res->ai_next) {
        *socketFDForClient = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (*socketFDForClient == -1)
            continue; 

        statusOfConnect = connect(*socketFDForClient, res->ai_addr, res->ai_addrlen);
        if (statusOfConnect == 0) 
            break;                 // Success

        close(*socketFDForClient);
    }

    if (res == NULL) {               // No address succeeded
        fprintf(stderr, "Could not connect\n");
        exit(1);
    }

}
