#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define IP "127.0.0.1"
#define BUFFER_SIZE 2048

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        printf("%s", buffer);
        
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(sockfd, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "5") == 0) break;


        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes <= 0) break;
            buffer[bytes] = '\0';
            printf("%s", buffer);

        
            if (strstr(buffer, "Enter") || strstr(buffer, "Command:")) {
                printf("> ");
                fgets(buffer, BUFFER_SIZE, stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                send(sockfd, buffer, strlen(buffer), 0);
            } else {
                break;
            }
        }
    }

    close(sockfd);
    return 0;
}
