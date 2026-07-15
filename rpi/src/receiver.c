#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include "../inc/max7219.h"

#define PORT 8080
#define ARRAY_SIZE 8

void cleanup(void);

int server_socket;

int main(void) {
    atexit(cleanup);
    signal(SIGINT, exit);

    if (max7219_init() == -1) exit(EXIT_FAILURE);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr, client_addr;
    uint32_t array[ARRAY_SIZE];

    if (server_socket == -1) {
        perror("Error: socket");
        return -1;
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    socklen_t client_len = sizeof(client_addr);
    while(1) {
        client_len = sizeof(client_addr);

        int bytes_received = recvfrom(server_socket, array, sizeof(array), 0, 
                                      (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received > 0) {
            uint32_t host_val_array[8];
            for (int i = 0; i < ARRAY_SIZE; i++) {
                host_val_array[i] = ntohl(array[i]);
                printf("[%d] = %u\n", i, host_val_array[i]);
            }
            max7219_write_array(host_val_array);
        }
    }

    close(server_socket);
    return 0;
}

void cleanup(void) {
    max7219_cleanup();
    if (server_socket) close(server_socket);
}