#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#define PORT 8080
#define ARRAY_SIZE 8

int main(void) {
    int client_socket;
    uint32_t array[ARRAY_SIZE] = {10, 20, 30, 40, 50, 60, 70, 80};
    int broadcast_enable;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Error: socket");
        return -1;
    }

    broadcast_enable = 1;

    if (setsockopt(client_socket, SOL_SOCKET, 
        SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("Error: setsocket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("255.255.255.255");


    int bytes_sent = sendto(client_socket, array, sizeof(array), 0, 
                            (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (bytes_sent == -1) {
        perror("Error: sendto");
    }

    close(client_socket);
    return 0;
}