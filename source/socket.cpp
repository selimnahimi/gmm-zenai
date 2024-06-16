#include <stdio.h>
#include <iostream>
#include <cstring>
#include "socket.hpp"

// TODO: add windows support
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "8080"
#define IP "127.0.0.1"

using namespace std;
using namespace serversock;

int sockfd, n;
struct sockaddr_in serv_addr;
char buffer[256];

void serversock::createConnection()
{
    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    else if (sockfd > 0)
    {
        cout << "SOCKET OPENED" << endl;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(PORT));
    inet_pton(AF_INET, IP, &(serv_addr.sin_addr.s_addr));

    cout << "attempting to connect to server" << endl;

    int conn_success = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (conn_success < 0)
    {
        perror("ERROR connecting");
    }
    else
    {
        cout << "connection successful" << endl;
    }
}

int serversock::readValues(gameActionData *a)
{

    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
    select(sockfd + 1, &fds, NULL, NULL, &tv);

    if (FD_ISSET(sockfd, &fds))
    {
        /* The socket_fd has data available to be read */
        n = recv(sockfd, buffer, sizeof(buffer), 0);
        cout << "Client received: " << n << endl;
        if (n != sizeof(struct gameActionData))
        {
            return 0;
        }
        // struct gameActionData data = *((struct gameActionData *)buffer);
        *a = *((struct gameActionData *)buffer);
        // cout << a->value << endl;
    }
    else
    {
        cout << "nothing received" << endl;
    }

    return 0;
}

void serversock::writeValues(gameStateData *a)
{
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
    select(sockfd + 1, NULL, &fds, NULL, &tv);

    if (FD_ISSET(sockfd, &fds))
    {
        n = send(sockfd, a, sizeof(struct gameStateData), 0);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
}