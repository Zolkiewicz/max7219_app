#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define ARRAY_SIZE 8

int main(void) {
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
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
        socklen_t client_len = sizeof(client_addr);

        int bytes_received = recvfrom(server_socket, array, sizeof(array), 0, 
                                      (struct sockaddr*)&client_addr, &client_len);
        
        if (bytes_received > 0) {
            for (int i = 0; i < ARRAY_SIZE; i++) {
                printf("[%d] = %u\n", i, array[i]);
            }
        }
    }

    close(server_socket);
    return 0;
}