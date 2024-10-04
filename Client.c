#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

int main(){
    int socketFD;
    struct sockaddr_in6 ip6Addr_server;

    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // IPv4またはIPv6
    hints.ai_socktype = SOCK_STREAM; // TCPソケット

    int status = getaddrinfo(NULL, "http", &hints, &res); //localhost と通信したいので NULL
    if (status != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    socketFD = socket(AF_INET6, SOCK_STREAM, 0);
    if (socketFD == -1){
        perror("opening stream socket");
        exit(1);
    }

    memset(&ip6Addr_server, 0, sizeof(ip6Addr_server));    
    ip6Addr_server.sin6_family = AF_INET6;

    int isConnected = connect(socketFD, &clientAddress, sizeof(clientAddress)); 
    if (isConnected != 0) {
        perror(connection failed);
    }
}