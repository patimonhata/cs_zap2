#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h> //memset()
#include <unistd.h> //read(), close()

int main(int argc, char* argv[]){
    int clientSocket;
    struct sockaddr_in6 ip6Addr_server;

    struct addrinfo hints, *results, *res;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    int statusOfGetaddrinfo = getaddrinfo(argv[1], argv[2], &hints, &results);
    if (statusOfGetaddrinfo != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(statusOfGetaddrinfo));
        exit(1);
    }

    /*  getaddrinfo() returns a list of address structures.
        Try each address until we successfully connect(2).
        If socket(2) (or connect(2)) fails, we (close the socket
        and) try the next address. */

    int statusOfConnect;
    for (res = results; res != NULL; res = res->ai_next) {
        clientSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (clientSocket == -1)
            continue; 

        statusOfConnect = connect(clientSocket, res->ai_addr, res->ai_addrlen);
        if (statusOfConnect == 0) 
            break;                 /* Success */

        close(clientSocket);
    }
    freeaddrinfo(results);           /* No longer needed */
    if (res == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(1);
    }

    const char *message = "Can you hear me?";
    if (send(clientSocket, message, strlen(message), 0) == -1) {
        perror("send");
    }

    close(clientSocket);

    exit(0);
}
