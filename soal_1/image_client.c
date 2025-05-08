#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define IP "127.0.0.1"
#define CLIENT_DIR "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/client"
#define CLIENT_SECRETS "/home/zele/college/programming/sms2/sisop/module3/Sisop-03-2025-IT05/soal_1/client/secrets"

int connect_to_server() {
    int sock = 0; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  // create socket
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET, // IPv4
        .sin_port = htons(PORT) // port number
    };

    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) { // convert IP address
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { // connect to server
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void decrypt(int sock) {
    char filename[128];
    printf("Enter text file name: "); 
    scanf("%s", filename); // get filename from user

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", CLIENT_SECRETS, filename); // create full path

    FILE *file = fopen(path, "r"); // open file for reading
    if (file == NULL) { // check if file opened successfully
        perror("Your file is an impostor. The filename doesn't exist.\n");
        return;
    }

    fseek(file, 0, SEEK_END); // move to end of file
    long file_size = ftell(file); // get file size
    fseek(file, 0, SEEK_SET); // move back to start of file

    char *content = malloc(file_size + 1); // allocate memory for file content
    if (content == NULL) { // check if memory allocation failed
        perror("You are a failure! Memory allocation has failed!\n");
        fclose(file);
        return;
    }

    fread(content, 1, file_size, file); // read file content
    content[file_size] = '\0'; // null-terminate the string 
    fclose(file); // close file

    char command[7000];
    snprintf(command, sizeof(command), "DECRYPT %s\n%s", filename, content); // create command
    send(sock, command, strlen(command), 0); // send command to server

    /* free(content); // free allocated memory */

    char response[256];
    read(sock, response, sizeof(response)); // read response from server
    printf("Server: %s\n", response); // print response
    return;
}

void download(int sock) {
    char filename[128];
    printf("Enter file name to download: "); 
    scanf("%s", filename); // get filename from user

    char command[256];
    snprintf(command, sizeof(command), "DOWNLOAD %s", filename); // create command
    send(sock, command, strlen(command), 0); // send command to server

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", CLIENT_DIR, filename); // create full path
    FILE *file = fopen(path, "wb"); // open file for writing
    if (file == NULL) { // check if file opened successfully
        perror("Failed to open file");
        return;
    }

    char buffer[7000]; 
    int bytes_received;
    while ((bytes_received = read(sock, buffer, sizeof(buffer))) > 0) { // receive data from server
        if (bytes_received <= 0) break;
        fwrite(buffer, 1, bytes_received, file); // write data to file
    }
    
    fclose(file); // close file
    printf("Server: File %s downloaded successfully.\n", filename); // print success message
    return;
}

int main() {
    int choice; 
    while(1) {
        printf("\n==========================\n");
        printf("||  ROOTKIDS DECRYPTOR  ||\n"); 
        printf("==========================\n");
        printf("1. Decrypt file\n");
        printf("2. Download the decrypted file\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice); // get user choice

        if (choice == 3) { // exit if user chooses 3
            int exit_sock = connect_to_server(); // connect to server
            if (exit_sock < 0) { // check if connection failed
                perror("Failed to connect to server");
                continue;
            }

            send(exit_sock, "EXIT", 4, 0); // send exit command to server
            close(exit_sock); // close socket

            printf("Done already, rootkid?...\n");
            break;
        }

        int sock = connect_to_server(); // connect to server
        if (sock < 0) { // check if connection failed
            perror("Failed to connect to server");
            continue;
        }

        switch (choice) { // handle user choice
            case 1:
                decrypt(sock); // decrypt file
                break;
            case 2:
                download(sock); // download file
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

        close(sock); // close socket
    }
    
    return 0;
}