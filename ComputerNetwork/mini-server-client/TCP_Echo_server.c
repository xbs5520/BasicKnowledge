#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    char buffer[BUF_SIZE];
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // allow reuns of address/port
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // bind to anyip:8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if(listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listen on port %d ...\n", PORT);

    // 5 accept and handle one client at a time
    while(1)
    {
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if(client_fd < 0)
        {
            perror("accept failed");
            continue;
        }

        printf("Client connected!\n");

        // receive & echo back
        int n = read(client_fd, buffer, BUF_SIZE);
        buffer[n] = '\0';
        printf("Receive: %s\n", buffer);

        send(client_fd, buffer, strlen(buffer), 0);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}