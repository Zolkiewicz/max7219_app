#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#define PORT 8080
#define ARRAY_SIZE 8

int send_array(uint32_t *array) {
    int client_socket;
    int broadcast_enable;
    struct sockaddr_in server_addr;
    uint32_t network_array[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
        network_array[i] = htonl(array[i]);
    }

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Error: socket");
        return -1;
    }

    broadcast_enable = 1;

    if (setsockopt(client_socket, SOL_SOCKET, 
        SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("Error: setsocket");
        close(client_socket);
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("255.255.255.255");


    int bytes_sent = sendto(client_socket, network_array, sizeof(network_array), 0, 
                            (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (bytes_sent == -1) {
        perror("Error: sendto");
        close(client_socket);
        return -1;
    }

    close(client_socket);
    return 0;
}