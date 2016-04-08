/*

    Implementation of a broadcast server hosted on CDF at the University of
    Toronto.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define PORT_NUM 53666
#define MAX_BACKLOG 5
#define MAX_MESSAGE 28
#define ERROR 1


int main() {

    // create the socket
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
    perror("socket");
        close(soc);
        exit(ERROR);
    }

    // reuseable socket
    int on = 1;
    setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));

    // bind socket
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NUM);
    server.sin_addr.s_addr = INADDR_ANY;
    memset(&server.sin_zero, 0, 8);

    if (bind(soc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("bind");
        close(soc);
        exit(ERROR);
    }

    // wait for connections
    if (listen(soc, MAX_BACKLOG) < 0) {
        perror("listen");
        close(soc);
        exit(ERROR);
    }

    char welcome_message[] = "Forum 4/7/16\n> ";
    welcome_message[strlen(welcome_message) - 1] = '\0';
    char broadcast[MAX_MESSAGE];
    int client;

    while (1) {

        // client
        struct sockaddr_in request;
        socklen_t size = (socklen_t) sizeof(client);

        // accept connections
        if ((client = accept(soc, (struct sockaddr *) &request, &size)) < 0) {
            perror("accept");
            close(soc);
            exit(ERROR);
        }
        
        fd_set set;
        FD_ZERO(&set);
        FD_SET(client, &set);

        if (select(client + 1, &set, NULL, NULL, NULL) < 0) {
            perror("select");
            close(soc);
            exit(ERROR);
        }

        if (FD_ISSET(client, &set)) {
                
            // write message to client
            write(client, welcome_message, strlen(welcome_message));
            
            // read data from client
            read(client, broadcast, MAX_MESSAGE - 1);
            broadcast[strlen(broadcast)] = '\0';

            // display message internally
            fprintf(stdout, "Broadcast: %s\n", broadcast);

            // broadcast message to all clients
            write(client, broadcast, strlen(broadcast));
        }
    }

    close(soc);
    return 0;
}
